#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "Stage2ConsoleMenu.hpp"
#include "exf/domain/Courier.hpp"
#include "exf/domain/Item.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/User.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/service/CourierService.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/service/UserService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

namespace exf {

/**
 * Stage2ConsoleApp 管理阶段 2 的控制台入口流程。
 */
class Stage2ConsoleApp {
   public:
    /**
     * 使用独立数据目录创建阶段 2 控制台程序。
     */
    explicit Stage2ConsoleApp(std::filesystem::path dataDir);

    /**
     * 运行阶段 2 控制台程序。
     */
    int run();

   private:
    /** 阶段 2 独立文件存储。 */
    FileStorage storage_;

    /** 阶段 2 复用普通用户仓储。 */
    UserRepository userRepository_;

    /** 阶段 2 复用管理员仓储。 */
    AdminRepository adminRepository_;

    /** 阶段 2 快递员仓储。 */
    CourierRepository courierRepository_;

    /** 阶段 2 复用并扩展包裹仓储。 */
    ParcelRepository parcelRepository_;

    /** 用户服务。 */
    UserService userService_;

    /** 管理员服务。 */
    AdminService adminService_;

    /** 快递员服务。 */
    CourierService courierService_;

    /** 快递服务。 */
    ParcelService parcelService_;

    /** 处理用户注册流程。 */
    void handleUserRegister();

    /** 处理用户登录流程。 */
    void handleUserLogin(Stage2Context& ctx);

    /** 处理快递员登录流程。 */
    void handleCourierLogin(Stage2Context& ctx);

    /** 处理管理员登录流程。 */
    void handleAdminLogin(Stage2Context& ctx);

    /** 处理退出登录流程。 */
    void handleLogout(Stage2Context& ctx);

    /** 处理修改密码流程。 */
    void handleChangePassword(Stage2Context& ctx);

    /** 查询当前账号余额。 */
    void handleGetBalance(const Stage2Context& ctx);

    /** 处理用户充值流程。 */
    void handleTopUpBalance(const Stage2Context& ctx);

    /** 处理用户寄件流程。 */
    void handleSendParcel(const Stage2Context& ctx);

    /** 处理用户签收快递流程。 */
    void handleSignParcels(const Stage2Context& ctx);

    /** 处理用户快递查询流程。 */
    void handleQueryUserParcels(const Stage2Context& ctx);

    /** 查询当前快递员待揽收任务。 */
    void handleListWaitingPickupTasks(const Stage2Context& ctx);

    /** 处理快递员揽收流程。 */
    void handlePickupParcels(const Stage2Context& ctx);

    /** 处理快递员任务查询流程。 */
    void handleQueryCourierParcels(const Stage2Context& ctx);

    /** 查询当前快递员余额。 */
    void handleCourierBalance(const Stage2Context& ctx);

    /** 处理管理员查看用户列表流程。 */
    void handleListUsers(const Stage2Context& ctx);

    /** 处理管理员查看快递员列表流程。 */
    void handleListCouriers(const Stage2Context& ctx);

    /** 处理管理员新增快递员流程。 */
    void handleAddCourier(const Stage2Context& ctx);

    /** 处理管理员删除快递员流程。 */
    void handleDeleteCourier(const Stage2Context& ctx);

    /** 处理管理员分配快递员流程。 */
    void handleAssignCourier(const Stage2Context& ctx);

    /** 处理管理员快递查询流程。 */
    void handleQueryAdminParcels(const Stage2Context& ctx);

    /** 处理管理员查询快递员任务流程。 */
    void handleQueryCourierTasksAsAdmin(const Stage2Context& ctx);

    /** 提示输入通用快递查询条件。 */
    static ParcelQuery promptParcelQuery();

    /** 提示输入管理员快递查询条件。 */
    static ParcelQuery promptAdminParcelQuery();

    /** 提示输入快递员任务查询条件。 */
    static ParcelQuery promptCourierParcelQuery();

    /** 提示输入可选快递状态。 */
    static std::optional<ParcelStatus> promptOptionalParcelStatus(
        std::string_view label);

    /** 提示输入可选物品类型。 */
    static std::optional<ParcelItemType> promptOptionalItemType(
        std::string_view label);

    /** 提示输入物品类型和计费数量。 */
    static std::unique_ptr<Item> promptItem();

    /** 提示输入正数小数。 */
    static double promptPositiveDouble(std::string_view label);

    /** 提示输入正整数。 */
    static int promptPositiveInt(std::string_view label);

    /** 解析批量选择的序号。 */
    static std::optional<std::vector<size_t>> parseSelectionIndices(
        std::string_view input,
        size_t maxCount);

    /** 返回快递状态显示文本。 */
    static std::string parcelStatusText(ParcelStatus status);

    /** 返回物品类型显示文本。 */
    static std::string parcelItemTypeText(ParcelItemType itemType);

    /** 格式化时间戳用于控制台显示。 */
    static std::string formatTimestampForDisplay(std::string_view timestamp);

    /** 打印用户列表。 */
    static void printUserList(const std::vector<User>& users);

    /** 打印快递员列表。 */
    static void printCourierList(const std::vector<Courier>& couriers);

    /** 打印快递列表。 */
    static void printParcelList(const std::vector<Parcel>& parcels);

    /** 打印快递服务错误。 */
    static void printParcelServiceError(ParcelServiceError error);
};

}  // namespace exf
