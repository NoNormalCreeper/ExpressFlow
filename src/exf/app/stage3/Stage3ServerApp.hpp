#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>

#include "exf/app/stage3/Stage3RequestDispatcher.hpp"
#include "exf/app/stage3/Stage3SessionManager.hpp"
#include "exf/net/TcpConnection.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/service/CourierService.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/service/UserService.hpp"
#include "exf/storage/FileStorage.hpp"

namespace exf {

/**
 * Stage3ServerApp 是题目 3 网络版服务端入口。
 */
class Stage3ServerApp {
   public:
    /**
     * 使用独立数据目录创建服务端。
     */
    explicit Stage3ServerApp(std::filesystem::path dataDir);

    /**
     * 启动监听并处理客户端请求。
     */
    int run(const std::string& host, uint16_t port);

   private:
    static constexpr unsigned int kMaxClients = 32;

    /** 网络版独立文件存储。 */
    FileStorage storage_;

    /** 用户仓储。 */
    UserRepository userRepository_;

    /** 管理员仓储。 */
    AdminRepository adminRepository_;

    /** 快递员仓储。 */
    CourierRepository courierRepository_;

    /** 快递仓储。 */
    ParcelRepository parcelRepository_;

    /** 用户服务。 */
    UserService userService_;

    /** 管理员服务。 */
    AdminService adminService_;

    /** 快递员服务。 */
    CourierService courierService_;

    /** 快递服务。 */
    ParcelService parcelService_;

    /** 会话管理器。 */
    Stage3SessionManager sessions_;

    /** 请求分发器。 */
    Stage3RequestDispatcher dispatcher_;

    /** 当前在线连接数。 */
    std::atomic<unsigned int> activeClients_{0};

    void handleClient(TcpConnection connection);
};

}  // namespace exf
