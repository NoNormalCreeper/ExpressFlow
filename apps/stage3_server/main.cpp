#include "exf/app/stage3/Stage3ServerApp.hpp"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    const auto parsePort = [](const std::string& text, uint16_t& port) {
        try {
            size_t parsed = 0;
            const long value = std::stol(text, &parsed);
            if (parsed != text.size() || value < 1 || value > 65535) {
                return false;
            }
            port = static_cast<uint16_t>(value);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    };

    std::string host = "0.0.0.0";
    uint16_t port = 18080;
    std::filesystem::path dataDir = "data_stage3";

    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3 && !parsePort(argv[2], port)) {
        std::cerr << "端口参数不合法。\n";
        return 1;
    }
    if (argc >= 4) {
        dataDir = argv[3];
    }

    try {
        exf::Stage3ServerApp app(dataDir);
        return app.run(host, port);
    } catch (const std::exception& e) {
        std::cerr << "服务端启动失败: " << e.what() << '\n';
        return 1;
    }
}
