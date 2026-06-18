#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "ConsoleInput.hpp"
#include "ConsoleMenu.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/service/AuthService.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/service/UserService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"

namespace exf {

/**
 * Stage1ConsoleApp 管理阶段 1 的控制台入口流程。
 */
class Stage1ConsoleApp {
   public:
    /**
     * 创建阶段 1 的控制台占位程序。
     */
    explicit Stage1ConsoleApp(std::filesystem::path dataDir);

    /**
     * 运行阶段 1 的控制台占位程序。
     */
    int run();

   private:
    /** 阶段 1 文件存储。 */
    FileStorage storage_;

    /** 用户仓储。 */
    UserRepository userRepository_;

    /** 管理员仓储。 */
    AdminRepository adminRepository_;

    /** 快递仓储。 */
    ParcelRepository parcelRepository_;

    /** 登录认证服务。 */
    AuthService authService_;

    /** 用户业务服务。 */
    UserService userService_;

    /** 管理员业务服务。 */
    AdminService adminService_;

    /** 快递业务服务。 */
    ParcelService parcelService_;

    /** 处理用户注册流程。 */
    void handleUserRegister();

    /** 处理用户登录流程。 */
    void handleUserLogin(MainMenu::Context& ctx);

    /** 处理管理员登录流程。 */
    void handleAdminLogin(MainMenu::Context& ctx);

    /** 处理退出登录流程。 */
    void handleLogout(MainMenu::Context& ctx);

    /** 处理用户修改密码流程。 */
    void handleChangePassword(const MainMenu::Context& ctx);

    /** 查询当前用户余额。 */
    void handleGetBalance(const MainMenu::Context& ctx);

    /** 处理当前用户充值流程。 */
    void handleTopUpBalance(const MainMenu::Context& ctx);

    /** 处理用户寄件流程。 */
    void handleSendParcel(const MainMenu::Context& ctx);

    /** 处理用户签收快递流程。 */
    void handleSignParcels(const MainMenu::Context& ctx);

    /** 处理用户快递查询流程。 */
    void handleQueryUserParcels(const MainMenu::Context& ctx);

    /** 处理管理员查看用户列表流程。 */
    void handleListUsers(const MainMenu::Context& ctx);

    /** 处理管理员快递查询流程。 */
    void handleQueryAdminParcels(const MainMenu::Context& ctx);

    /** 提示用户输入个人快递查询条件。 */
    static ParcelQuery promptUserParcelQuery();

    /** 提示管理员输入快递查询条件。 */
    static ParcelQuery promptAdminParcelQuery();

    /** 提示输入可选快递状态。 */
    static std::optional<ParcelStatus> promptOptionalParcelStatus(
        std::string_view label);

    /** 解析批量选择的序号。 */
    static std::optional<std::vector<size_t>> parseSelectionIndices(
        std::string_view input,
        size_t maxCount);

    /** 返回快递状态显示文本。 */
    static std::string parcelStatusText(ParcelStatus status);

    /** 格式化时间戳用于控制台显示。 */
    static std::string formatTimestampForDisplay(std::string_view timestamp);

    /** 打印用户列表。 */
    static void printUserList(const std::vector<User>& users);

    /** 打印快递列表。 */
    static void printParcelList(const std::vector<Parcel>& parcels);
};

}  // namespace exf
