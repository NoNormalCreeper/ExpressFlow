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

Stage3ClientService::Stage3ClientService(std::string host, uint16_t port)
    : host_(std::move(host)), port_(port) {}

bool Stage3ClientService::connect() {
    if (!client_.connectTo(host_, port_)) {
        return reconnect();
    }
    lastError_.clear();
    return true;
}

const std::string& Stage3ClientService::lastError() const {
    return lastError_;
}

bool Stage3ClientService::isLoggedIn() const {
    return !token_.empty();
}

bool Stage3ClientService::isConnected() const {
    return client_.connection().isOpen();
}

std::optional<Stage3Role> Stage3ClientService::currentRole() const {
    return role_;
}

const std::string& Stage3ClientService::username() const {
    return username_;
}

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

std::optional<Stage3Response> Stage3ClientService::logout() {
    if (!isLoggedIn()) {
        return std::nullopt;
    }
    auto response = sendRequest(Stage3Request("QUIT", {token_}));
    clearLoginState();
    return response;
}

std::optional<Stage3Response> Stage3ClientService::sendParcel(
    const std::string& receiver,
    const std::string& itemType,
    const std::string& amount,
    const std::string& description) {
    return sendRequest(Stage3Request(
        "SEND_PARCEL", {token_, receiver, itemType, amount, description}));
}

std::vector<Parcel> Stage3ClientService::listWaitingSign() {
    return fetchParcels("LIST_WAITING_SIGN");
}

std::optional<Stage3Response> Stage3ClientService::signParcel(
    const std::string& parcelId) {
    return sendRequest(Stage3Request("SIGN", {token_, parcelId}));
}

std::vector<Parcel> Stage3ClientService::listUnassigned() {
    return fetchParcels("LIST_UNASSIGNED");
}

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

std::optional<Stage3Response> Stage3ClientService::addCourier(
    const std::string& username,
    const std::string& name,
    const std::string& phone,
    const std::string& password) {
    return sendRequest(Stage3Request(
        "ADD_COURIER", {token_, username, name, phone, password}));
}

std::optional<Stage3Response> Stage3ClientService::deleteCourier(
    const std::string& courierUsername) {
    return sendRequest(
        Stage3Request("DELETE_COURIER", {token_, courierUsername}));
}

std::vector<Parcel> Stage3ClientService::listAllParcels() {
    return fetchParcels("LIST_ALL_PARCELS");
}

std::vector<Parcel> Stage3ClientService::listMyParcels() {
    return fetchParcels("LIST_MY_PARCELS");
}

std::vector<Parcel> Stage3ClientService::listCourierParcels(
    const std::string& courierUsername) {
    if (courierUsername.empty()) {
        return fetchParcels("LIST_COURIER_PARCELS");
    }
    return fetchParcels(
        Stage3Request("LIST_COURIER_PARCELS", {token_, courierUsername}));
}

std::optional<Stage3Response> Stage3ClientService::assignCourier(
    const std::string& parcelId,
    const std::string& courierUsername) {
    return sendRequest(
        Stage3Request("ASSIGN_COURIER", {token_, parcelId, courierUsername}));
}

std::vector<Parcel> Stage3ClientService::listPickupTasks() {
    return fetchParcels("LIST_PICKUP_TASKS");
}

std::optional<Stage3Response> Stage3ClientService::pickupParcel(
    const std::string& parcelId) {
    return sendRequest(Stage3Request("PICKUP", {token_, parcelId}));
}

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

void Stage3ClientService::clearLoginState() {
    token_.clear();
    role_.reset();
    username_.clear();
}

std::vector<Parcel> Stage3ClientService::fetchParcels(
    const std::string& command) {
    return fetchParcels(Stage3Request(command, {token_}));
}

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
