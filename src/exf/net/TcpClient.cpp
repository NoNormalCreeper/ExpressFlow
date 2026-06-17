#include "exf/net/TcpClient.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

namespace exf {

bool TcpClient::connectTo(const std::string& host, uint16_t port) {
    lastError_.clear();

    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        lastError_ = "创建 socket 失败";
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        lastError_ = "服务端地址不合法";
        ::close(fd);
        return false;
    }

    if (::connect(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        lastError_ = "连接服务端失败: ";
        lastError_ += std::strerror(errno);
        ::close(fd);
        return false;
    }

    connection_ = TcpConnection(fd);
    return true;
}

TcpConnection& TcpClient::connection() {
    return connection_;
}

const TcpConnection& TcpClient::connection() const {
    return connection_;
}

const std::string& TcpClient::lastError() const {
    return lastError_;
}

}  // namespace exf
