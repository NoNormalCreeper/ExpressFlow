#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "exf/app/stage3/Stage3Protocol.hpp"
#include "exf/app/stage3/Stage3SessionManager.hpp"
#include "exf/domain/Courier.hpp"
#include "exf/domain/Item.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelItemType.hpp"
#include "exf/domain/User.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/service/CourierService.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/service/UserService.hpp"

namespace exf {

/**
 * Stage3RequestDispatcher 将网络请求转换为现有业务 service 调用。
 */
class Stage3RequestDispatcher {
   public:
    /**
     * 使用现有业务服务创建网络请求分发器。
     */
    Stage3RequestDispatcher(UserService& users,
                            AdminService& admins,
                            CourierService& couriers,
                            ParcelService& parcels,
                            Stage3SessionManager& sessions);

    /**
     * 处理一行请求文本并返回一行响应文本。
     */
    std::string handleLine(const std::string& line);

    /**
     * 处理已解码请求。
     */
    Stage3Response handleRequest(const Stage3Request& request);

   private:
    /** 用户服务。 */
    UserService& users_;

    /** 管理员服务。 */
    AdminService& admins_;

    /** 快递员服务。 */
    CourierService& couriers_;

    /** 快递服务。 */
    ParcelService& parcels_;

    /** 会话服务。 */
    Stage3SessionManager& sessions_;

    /** 保护文件仓储和内存仓储的业务互斥锁。 */
    std::mutex businessMutex_;

    Stage3Response handleLogin(const std::vector<std::string>& fields);
    Stage3Response handleSendParcel(const std::vector<std::string>& fields);
    Stage3Response handleListUnassigned(const std::vector<std::string>& fields);
    Stage3Response handleAssignCourier(const std::vector<std::string>& fields);
    Stage3Response handleListUsers(const std::vector<std::string>& fields);
    Stage3Response handleListCouriers(const std::vector<std::string>& fields);
    Stage3Response handleAddCourier(const std::vector<std::string>& fields);
    Stage3Response handleDeleteCourier(const std::vector<std::string>& fields);
    Stage3Response handleListAllParcels(
        const std::vector<std::string>& fields);
    Stage3Response handleListMyParcels(const std::vector<std::string>& fields);
    Stage3Response handleListCourierParcels(
        const std::vector<std::string>& fields);
    Stage3Response handleListPickupTasks(
        const std::vector<std::string>& fields);
    Stage3Response handlePickup(const std::vector<std::string>& fields);
    Stage3Response handleListWaitingSign(
        const std::vector<std::string>& fields);
    Stage3Response handleSign(const std::vector<std::string>& fields);
    Stage3Response handleQuit(const std::vector<std::string>& fields);

    std::optional<Stage3Session> requireSession(
        const std::vector<std::string>& fields,
        Stage3Role role) const;
    std::optional<Stage3Session> requireAnySession(
        const std::vector<std::string>& fields) const;

    static std::unique_ptr<Item> makeItem(ParcelItemType type, double amount);
    static std::optional<ParcelItemType> parseItemType(
        const std::string& text);
    static std::string encodeParcel(const Parcel& parcel);
    static std::string encodeCourier(const Courier& courier);
    static std::string encodeUser(const User& user);
    static Stage3Response parcelError(ParcelServiceError error);
    static Stage3Response userError(UserServiceError error);
    static Stage3Response courierError(CourierServiceError error);
};

}  // namespace exf
