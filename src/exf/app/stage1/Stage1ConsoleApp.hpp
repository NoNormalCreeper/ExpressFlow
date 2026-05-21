#pragma once

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

    AuthService authService_;
    UserService userService_;

    void handleUserRegister();
    void handleUserLogin();
    void handleChangePassword();
};

}  // namespace exf
