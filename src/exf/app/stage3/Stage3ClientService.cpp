#include "exf/app/stage3/Stage3ClientService.hpp"

#include <chrono>
#include <exception>
#include <thread>
#include <utility>

#include "exf/storage/RecordCodec.hpp"

namespace exf {
namespace {

constexpr int kReconnectAttempts = 3;

}  // namespace

// 创建连接指定服务端的客户端服务。
Stage3ClientService::Stage3ClientService(std::string host, uint16_t port)
    : host_(std::move(host)), port_(port) {}

// 建立到服务端的连接。
bool Stage3ClientService::connect() {
    if (!client_.connectTo(host_, port_)) {
        return reconnect();
    }
    lastError_.clear();
    return true;
}

// 返回最近一次错误消息。
const std::string& Stage3ClientService::lastError() const {
    return lastError_;
}

// 返回当前是否保存了登录 token。
bool Stage3ClientService::isLoggedIn() const {
    return !token_.empty();
}

// 返回底层 TCP 连接是否打开。
bool Stage3ClientService::isConnected() const {
    return client_.connection().isOpen();
}

// 返回当前登录角色。
std::optional<Stage3Role> Stage3ClientService::currentRole() const {
    return role_;
}

// 返回当前登录用户名。
const std::string& Stage3ClientService::username() const {
    return username_;
}

// 发送登录请求并保存服务端返回的登录态。
std::optional<Stage3Response> Stage3ClientService::login(
    Stage3Role role,
    const std::string& username,
    const std::string& password) {
    auto response = sendRequest(Stage3Request(
        "LOGIN", {Stage3RoleCodec::encode(role), username, password}));
    if (response.has_value() && response->isOk() &&
        response->fields().size() >= 3) {
        const auto decodedRole = Stage3RoleCodec::decode(response->fields()[1]);
        if (decodedRole.has_value()) {
            token_ = response->fields()[0];
            role_ = *decodedRole;
            username_ = response->fields()[2];
        }
    }
    return response;
}

// 发送退出登录请求并清理本地登录态。
std::optional<Stage3Response> Stage3ClientService::logout() {
    if (!isLoggedIn()) {
        return std::nullopt;
    }
    auto response = sendRequest(Stage3Request("QUIT", {token_}));
    clearLoginState();
    return response;
}

// 发送寄件请求。
std::optional<Stage3Response> Stage3ClientService::sendParcel(
    const std::string& receiver,
    const std::string& itemType,
    const std::string& amount,
    const std::string& description) {
    return sendRequest(Stage3Request(
        "SEND_PARCEL", {token_, receiver, itemType, amount, description}));
}

// 查询待签收快递。
std::vector<Parcel> Stage3ClientService::listWaitingSign() {
    return fetchParcels("LIST_WAITING_SIGN");
}

// 发送签收请求。
std::optional<Stage3Response> Stage3ClientService::signParcel(
    const std::string& parcelId) {
    return sendRequest(Stage3Request("SIGN", {token_, parcelId}));
}

// 查询未分配快递。
std::vector<Parcel> Stage3ClientService::listUnassigned() {
    return fetchParcels("LIST_UNASSIGNED");
}

// 查询所有用户。
std::vector<User> Stage3ClientService::listUsers() {
    std::vector<User> users;
    auto response = sendRequest(Stage3Request("LIST_USERS", {token_}));
    if (!response.has_value()) {
        return users;
    }
    if (!response->isOk()) {
        lastError_ = response->message();
        return users;
    }
    for (const auto& field : response->fields()) {
        try {
            users.push_back(UserRecordCodec::decode(field));
        } catch (const std::exception&) {
        }
    }
    return users;
}

// 查询所有快递员。
std::vector<Courier> Stage3ClientService::listCouriers() {
    std::vector<Courier> couriers;
    auto response = sendRequest(Stage3Request("LIST_COURIERS", {token_}));
    if (!response.has_value()) {
        return couriers;
    }
    if (!response->isOk()) {
        lastError_ = response->message();
        return couriers;
    }
    for (const auto& field : response->fields()) {
        try {
            couriers.push_back(CourierRecordCodec::decode(field));
        } catch (const std::exception&) {
        }
    }
    return couriers;
}

// 发送新增快递员请求。
std::optional<Stage3Response> Stage3ClientService::addCourier(
    const std::string& username,
    const std::string& name,
    const std::string& phone,
    const std::string& password) {
    return sendRequest(Stage3Request(
        "ADD_COURIER", {token_, username, name, phone, password}));
}

// 发送删除快递员请求。
std::optional<Stage3Response> Stage3ClientService::deleteCourier(
    const std::string& courierUsername) {
    return sendRequest(
        Stage3Request("DELETE_COURIER", {token_, courierUsername}));
}

// 查询全部快递。
std::vector<Parcel> Stage3ClientService::listAllParcels() {
    return fetchParcels("LIST_ALL_PARCELS");
}

// 查询当前用户相关快递。
std::vector<Parcel> Stage3ClientService::listMyParcels() {
    return fetchParcels("LIST_MY_PARCELS");
}

// 查询指定或当前快递员相关快递。
std::vector<Parcel> Stage3ClientService::listCourierParcels(
    const std::string& courierUsername) {
    if (courierUsername.empty()) {
        return fetchParcels("LIST_COURIER_PARCELS");
    }
    return fetchParcels(
        Stage3Request("LIST_COURIER_PARCELS", {token_, courierUsername}));
}

// 发送分配快递员请求。
std::optional<Stage3Response> Stage3ClientService::assignCourier(
    const std::string& parcelId,
    const std::string& courierUsername) {
    return sendRequest(
        Stage3Request("ASSIGN_COURIER", {token_, parcelId, courierUsername}));
}

// 查询当前快递员待揽收任务。
std::vector<Parcel> Stage3ClientService::listPickupTasks() {
    return fetchParcels("LIST_PICKUP_TASKS");
}

// 发送揽收请求。
std::optional<Stage3Response> Stage3ClientService::pickupParcel(
    const std::string& parcelId) {
    return sendRequest(Stage3Request("PICKUP", {token_, parcelId}));
}

// 发送单个协议请求并解析响应。
std::optional<Stage3Response> Stage3ClientService::sendRequest(
    const Stage3Request& request) {
    lastError_.clear();
    if (!client_.connection().isOpen()) {
        clearLoginState();
        if (reconnect()) {
            lastError_ = "连接已恢复，请重新登录后重试。";
        }
        return std::nullopt;
    }
    if (!client_.connection().sendLine(
            Stage3Protocol::encodeRequest(request))) {
        clearLoginState();
        if (reconnect()) {
            lastError_ = "请求发送失败，连接已恢复，请重新登录后重试。";
        }
        return std::nullopt;
    }
    auto line = client_.connection().receiveLine();
    if (!line.has_value()) {
        clearLoginState();
        if (reconnect()) {
            lastError_ = "服务端连接已断开，连接已恢复，请重新登录后重试。";
        }
        return std::nullopt;
    }
    auto response = Stage3Protocol::decodeResponse(*line);
    if (!response.has_value()) {
        lastError_ = "服务端响应格式错误。";
        return std::nullopt;
    }
    return response;
}

// 尝试重连服务端。
bool Stage3ClientService::reconnect() {
    for (int i = 0; i < kReconnectAttempts; ++i) {
        if (client_.connectTo(host_, port_)) {
            lastError_.clear();
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    lastError_ = client_.lastError();
    return false;
}

// 清空本地登录态字段。
void Stage3ClientService::clearLoginState() {
    token_.clear();
    role_.reset();
    username_.clear();
}

// 按命令查询快递列表。
std::vector<Parcel> Stage3ClientService::fetchParcels(
    const std::string& command) {
    return fetchParcels(Stage3Request(command, {token_}));
}

// 按请求查询快递列表并解码响应字段。
std::vector<Parcel> Stage3ClientService::fetchParcels(
    const Stage3Request& request) {
    std::vector<Parcel> parcels;
    auto response = sendRequest(request);
    if (!response.has_value()) {
        return parcels;
    }
    if (!response->isOk()) {
        lastError_ = response->message();
        return parcels;
    }
    for (const auto& field : response->fields()) {
        try {
            parcels.push_back(ParcelRecordCodec::decode(field));
        } catch (const std::exception&) {
        }
    }
    return parcels;
}

}  // namespace exf
