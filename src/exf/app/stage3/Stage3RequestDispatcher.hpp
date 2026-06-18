#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
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
    /** 阶段 3 协议命令枚举。 */
    enum class Stage3Command {
        Login,
        SendParcel,
        ListUnassigned,
        AssignCourier,
        ListUsers,
        ListCouriers,
        AddCourier,
        DeleteCourier,
        ListAllParcels,
        ListMyParcels,
        ListCourierParcels,
        ListPickupTasks,
        Pickup,
        ListWaitingSign,
        Sign,
        Quit,
        Unknown
    };

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

    /** 处理登录请求。 */
    Stage3Response handleLogin(const std::vector<std::string>& fields);

    /** 处理用户寄件请求。 */
    Stage3Response handleSendParcel(const std::vector<std::string>& fields);

    /** 查询未分配快递。 */
    Stage3Response handleListUnassigned(const std::vector<std::string>& fields);

    /** 处理管理员分配快递员请求。 */
    Stage3Response handleAssignCourier(const std::vector<std::string>& fields);

    /** 查询用户列表。 */
    Stage3Response handleListUsers(const std::vector<std::string>& fields);

    /** 查询快递员列表。 */
    Stage3Response handleListCouriers(const std::vector<std::string>& fields);

    /** 处理新增快递员请求。 */
    Stage3Response handleAddCourier(const std::vector<std::string>& fields);

    /** 处理删除快递员请求。 */
    Stage3Response handleDeleteCourier(const std::vector<std::string>& fields);

    /** 查询全部快递。 */
    Stage3Response handleListAllParcels(
        const std::vector<std::string>& fields);

    /** 查询当前用户相关快递。 */
    Stage3Response handleListMyParcels(const std::vector<std::string>& fields);

    /** 查询指定或当前快递员相关快递。 */
    Stage3Response handleListCourierParcels(
        const std::vector<std::string>& fields);

    /** 查询快递员待揽收任务。 */
    Stage3Response handleListPickupTasks(
        const std::vector<std::string>& fields);

    /** 处理快递员揽收请求。 */
    Stage3Response handlePickup(const std::vector<std::string>& fields);

    /** 查询用户待签收快递。 */
    Stage3Response handleListWaitingSign(
        const std::vector<std::string>& fields);

    /** 处理用户签收请求。 */
    Stage3Response handleSign(const std::vector<std::string>& fields);

    /** 处理退出登录请求。 */
    Stage3Response handleQuit(const std::vector<std::string>& fields);

    /** 按指定角色校验 token 并返回会话。 */
    std::optional<Stage3Session> requireSession(
        const std::vector<std::string>& fields,
        Stage3Role role) const;

    /** 校验任意已登录 token 并返回会话。 */
    std::optional<Stage3Session> requireAnySession(
        const std::vector<std::string>& fields) const;

    /** 按物品类型创建快递物品对象。 */
    static std::unique_ptr<Item> makeItem(ParcelItemType type, double amount);

    /** 将协议命令文本转换为命令枚举。 */
    static Stage3Command parseStage3Command(std::string_view text);

    /** 将协议字段转换为物品类型。 */
    static std::optional<ParcelItemType> parseItemType(
        const std::string& text);

    /** 将快递对象编码为协议字段。 */
    static std::string encodeParcel(const Parcel& parcel);

    /** 将快递员对象编码为协议字段。 */
    static std::string encodeCourier(const Courier& courier);

    /** 将用户对象编码为协议字段。 */
    static std::string encodeUser(const User& user);

    /** 将快递服务错误转换为响应。 */
    static Stage3Response parcelError(ParcelServiceError error);

    /** 将用户服务错误转换为响应。 */
    static Stage3Response userError(UserServiceError error);

    /** 将快递员服务错误转换为响应。 */
    static Stage3Response courierError(CourierServiceError error);
};

}  // namespace exf
