#include "exf/app/stage3/Stage3ClientApp.hpp"

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>

namespace {

bool parsePort(const std::string& text, uint16_t& port) {
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
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string host = "127.0.0.1";
    uint16_t port = 18080;

    if (argc >= 2) {
        host = argv[1];
    }
    if (argc >= 3 && !parsePort(argv[2], port)) {
        std::cerr << "端口参数不合法。\n";
        return 1;
    }

    exf::Stage3ClientApp app(host, port);
    return app.run();
}
