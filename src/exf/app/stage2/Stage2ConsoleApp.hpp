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

    void handleUserRegister();
    void handleUserLogin(Stage2Context& ctx);
    void handleCourierLogin(Stage2Context& ctx);
    void handleAdminLogin(Stage2Context& ctx);
    void handleLogout(Stage2Context& ctx);
    void handleChangePassword(Stage2Context& ctx);

    void handleGetBalance(const Stage2Context& ctx);
    void handleTopUpBalance(const Stage2Context& ctx);

    void handleSendParcel(const Stage2Context& ctx);
    void handleSignParcels(const Stage2Context& ctx);
    void handleQueryUserParcels(const Stage2Context& ctx);

    void handleListWaitingPickupTasks(const Stage2Context& ctx);
    void handlePickupParcels(const Stage2Context& ctx);
    void handleQueryCourierParcels(const Stage2Context& ctx);
    void handleCourierBalance(const Stage2Context& ctx);

    void handleListUsers(const Stage2Context& ctx);
    void handleListCouriers(const Stage2Context& ctx);
    void handleAddCourier(const Stage2Context& ctx);
    void handleDeleteCourier(const Stage2Context& ctx);
    void handleAssignCourier(const Stage2Context& ctx);
    void handleQueryAdminParcels(const Stage2Context& ctx);
    void handleQueryCourierTasksAsAdmin(const Stage2Context& ctx);

    static ParcelQuery promptParcelQuery();
    static ParcelQuery promptAdminParcelQuery();
    static ParcelQuery promptCourierParcelQuery();
    static std::optional<ParcelStatus> promptOptionalParcelStatus(
        std::string_view label);
    static std::optional<ParcelItemType> promptOptionalItemType(
        std::string_view label);
    static std::unique_ptr<Item> promptItem();
    static double promptPositiveDouble(std::string_view label);
    static int promptPositiveInt(std::string_view label);
    static std::optional<std::vector<size_t>> parseSelectionIndices(
        std::string_view input,
        size_t maxCount);
    static std::string parcelStatusText(ParcelStatus status);
    static std::string parcelItemTypeText(ParcelItemType itemType);
    static std::string formatTimestampForDisplay(std::string_view timestamp);
    static void printUserList(const std::vector<User>& users);
    static void printCourierList(const std::vector<Courier>& couriers);
    static void printParcelList(const std::vector<Parcel>& parcels);
    static void printParcelServiceError(ParcelServiceError error);
};

}  // namespace exf
