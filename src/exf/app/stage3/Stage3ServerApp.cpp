#include "exf/app/stage3/Stage3ServerApp.hpp"

#include <iostream>
#include <thread>
#include <utility>

#include "exf/net/TcpServer.hpp"

namespace exf {

// 构造使用独立数据目录的服务端应用。
Stage3ServerApp::Stage3ServerApp(std::filesystem::path dataDir)
    : storage_(std::move(dataDir)),
      userRepository_(storage_),
      adminRepository_(storage_),
      courierRepository_(storage_),
      parcelRepository_(storage_),
      userService_(userRepository_),
      adminService_(userRepository_, adminRepository_),
      courierService_(courierRepository_),
      parcelService_(userRepository_,
                     adminRepository_,
                     parcelRepository_,
                     courierRepository_),
      dispatcher_(userService_,
                  adminService_,
                  courierService_,
                  parcelService_,
                  sessions_) {}

// 启动 TCP 监听并为每个客户端创建处理线程。
int Stage3ServerApp::run(const std::string& host, uint16_t port) {
    TcpServer server;
    if (!server.listenOn(host, port)) {
        std::cerr << server.lastError() << '\n';
        return 1;
    }

    std::cout << "ExpressFlow Stage 3 server listening on " << host << ':'
              << port << '\n';
    while (true) {
        auto connection = server.acceptConnection();
        if (!connection.has_value()) {
            std::cerr << server.lastError() << '\n';
            continue;
        }
        if (activeClients_.load() >= kMaxClients) {
            connection->sendLine(Stage3Protocol::encodeResponse(
                Stage3Response::error("ServerBusy", "服务器繁忙，请稍后再试")));
            continue;
        }
        ++activeClients_;
        std::thread(&Stage3ServerApp::handleClient, this,
                    std::move(*connection))
            .detach();
    }
}

// 循环读取单个客户端请求并写回响应。
void Stage3ServerApp::handleClient(TcpConnection connection) {
    while (connection.isOpen()) {
        auto line = connection.receiveLine();
        if (!line.has_value()) {
            break;
        }
        const auto response = dispatcher_.handleLine(*line);
        if (!connection.sendLine(response)) {
            break;
        }
    }
    --activeClients_;
}

}  // namespace exf
