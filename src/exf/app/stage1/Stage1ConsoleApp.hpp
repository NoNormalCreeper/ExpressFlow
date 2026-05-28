#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "ConsoleInput.hpp"
#include "ConsoleMenu.hpp"
#include "exf/repository/UserRepository.hpp"
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
    Stage1ConsoleApp(std::filesystem::path dataDir);

    /**
     * 运行阶段 1 的控制台占位程序。
     */
    int run();

   private:
    // 注入依赖
    FileStorage storage_;
    UserRepository userRepository_;
    AdminRepository adminRepository_;
    ParcelRepository parcelRepository_;

    AuthService authService_;
    UserService userService_;
    ParcelService parcelService_;

    void handleUserRegister();
    void handleUserLogin(MainMenu::Context& ctx);
    void handleAdminLogin(MainMenu::Context& ctx);
    void handleLogout(MainMenu::Context& ctx);
    void handleChangePassword(MainMenu::Context& ctx);

    // 约定登录后调用
    void handleGetBalance(MainMenu::Context& ctx);
    void handleTopUpBalance(MainMenu::Context& ctx);

    void handleSendParcel(MainMenu::Context& ctx);
    void handleSignParcels(MainMenu::Context& ctx);
    void handleQueryUserParcels(MainMenu::Context& ctx);
    void handleQueryAdminParcels(MainMenu::Context& ctx);

    static ParcelQuery promptUserParcelQuery();
    static ParcelQuery promptAdminParcelQuery();
    static std::optional<ParcelStatus> promptOptionalParcelStatus(
        std::string_view label);
    static std::optional<std::vector<size_t>> parseSelectionIndices(
        std::string_view input,
        size_t maxCount);
    static std::string parcelStatusText(ParcelStatus status);
    static std::string formatTimestampForDisplay(std::string_view timestamp);
    static void printParcelList(const std::vector<Parcel>& parcels);
};

}  // namespace exf
