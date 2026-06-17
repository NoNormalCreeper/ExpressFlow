#include "exf/app/stage3/Stage3ServerApp.hpp"

#include <iostream>
#include <thread>
#include <utility>

#include "exf/net/TcpServer.hpp"

namespace exf {

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
        std::thread(&Stage3ServerApp::handleClient, this,
                    std::move(*connection))
            .detach();
    }
}

void Stage3ServerApp::handleClient(TcpConnection connection) {
    while (connection.isOpen()) {
        auto line = connection.receiveLine();
        if (!line.has_value()) {
            return;
        }
        const auto response = dispatcher_.handleLine(*line);
        if (!connection.sendLine(response)) {
            return;
        }
    }
}

}  // namespace exf
