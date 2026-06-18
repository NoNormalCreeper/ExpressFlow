#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "exf/app/stage3/Stage3Protocol.hpp"
#include "exf/app/stage3/Stage3Role.hpp"
#include "exf/domain/Courier.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/User.hpp"
#include "exf/net/TcpClient.hpp"

namespace exf {

/**
 * Stage3ClientService 封装客户端到服务端的协议请求。
 */
class Stage3ClientService {
   public:
    /**
     * 用服务端地址创建客户端服务。
     */
    Stage3ClientService(std::string host, uint16_t port);

    /**
     * 连接服务端。
     */
    bool connect();

    /**
     * 返回连接或请求错误提示。
     */
    const std::string& lastError() const;

    /**
     * 返回是否已有登录态。
     */
    bool isLoggedIn() const;

    /**
     * 返回服务端连接是否仍然可用。
     */
    bool isConnected() const;

    /**
     * 返回当前登录角色。
     */
    std::optional<Stage3Role> currentRole() const;

    /**
     * 返回当前登录用户名。
     */
    const std::string& username() const;

    /**
     * 登录并保存 token。
     */
    std::optional<Stage3Response> login(Stage3Role role,
                                        const std::string& username,
                                        const std::string& password);

    /**
     * 退出登录并清理 token。
     */
    std::optional<Stage3Response> logout();

    /**
     * 发送快递。
     */
    std::optional<Stage3Response> sendParcel(
        const std::string& receiver,
        const std::string& itemType,
        const std::string& amount,
        const std::string& description);

    /**
     * 查询待签收快递。
     */
    std::vector<Parcel> listWaitingSign();

    /**
     * 签收快递。
     */
    std::optional<Stage3Response> signParcel(const std::string& parcelId);

    /**
     * 查询未分配快递。
     */
    std::vector<Parcel> listUnassigned();

    /**
     * 查询所有用户。
     */
    std::vector<User> listUsers();

    /**
     * 查询快递员。
     */
    std::vector<Courier> listCouriers();

    /**
     * 添加快递员。
     */
    std::optional<Stage3Response> addCourier(const std::string& username,
                                             const std::string& name,
                                             const std::string& phone,
                                             const std::string& password);

    /**
     * 删除快递员。
     */
    std::optional<Stage3Response> deleteCourier(
        const std::string& courierUsername);

    /**
     * 查询全部快递。
     */
    std::vector<Parcel> listAllParcels();

    /**
     * 查询当前用户相关快递。
     */
    std::vector<Parcel> listMyParcels();

    /**
     * 查询快递员相关快递。
     */
    std::vector<Parcel> listCourierParcels(
        const std::string& courierUsername = "");

    /**
     * 分配快递员。
     */
    std::optional<Stage3Response> assignCourier(
        const std::string& parcelId,
        const std::string& courierUsername);

    /**
     * 查询当前快递员待揽收任务。
     */
    std::vector<Parcel> listPickupTasks();

    /**
     * 揽收快递。
     */
    std::optional<Stage3Response> pickupParcel(const std::string& parcelId);

   private:
    /** 服务端地址。 */
    std::string host_;

    /** 服务端端口。 */
    uint16_t port_;

    /** TCP 客户端。 */
    TcpClient client_;

    /** 当前登录 token。 */
    std::string token_;

    /** 当前登录角色。 */
    std::optional<Stage3Role> role_;

    /** 当前登录用户名。 */
    std::string username_;

    /** 最近一次错误。 */
    std::string lastError_;

    /** 重新建立 TCP 连接。 */
    bool reconnect();

    /** 清理本地登录状态。 */
    void clearLoginState();

    /** 发送协议请求并读取响应。 */
    std::optional<Stage3Response> sendRequest(
        const Stage3Request& request);

    /** 按命令获取快递列表。 */
    std::vector<Parcel> fetchParcels(const std::string& command);

    /** 按完整请求获取快递列表。 */
    std::vector<Parcel> fetchParcels(const Stage3Request& request);
};

}  // namespace exf
