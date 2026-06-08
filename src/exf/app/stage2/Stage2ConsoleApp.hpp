#pragma once

#include <filesystem>

#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/storage/FileStorage.hpp"

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

    /** 阶段 2 复用并扩展包裹仓储。 */
    ParcelRepository parcelRepository_;
};

}  // namespace exf
