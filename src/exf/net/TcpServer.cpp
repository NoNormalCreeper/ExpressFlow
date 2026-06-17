#include "exf/net/TcpServer.hpp"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

namespace exf {

TcpServer::TcpServer() : fd_(-1) {}

TcpServer::~TcpServer() {
    close();
}

bool TcpServer::listenOn(const std::string& host, uint16_t port) {
    lastError_.clear();

    const int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        lastError_ = "创建 socket 失败";
        return false;
    }

    int opt = 1;
    ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (host.empty() || host == "0.0.0.0") {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (::inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        lastError_ = "监听地址不合法";
        ::close(fd);
        return false;
    }

    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        lastError_ = "绑定端口失败: ";
        lastError_ += std::strerror(errno);
        ::close(fd);
        return false;
    }

    if (::listen(fd, 16) < 0) {
        lastError_ = "监听端口失败: ";
        lastError_ += std::strerror(errno);
        ::close(fd);
        return false;
    }

    fd_ = fd;
    return true;
}

std::optional<TcpConnection> TcpServer::acceptConnection() {
    if (fd_ < 0) {
        return std::nullopt;
    }

    while (true) {
        const int clientFd = ::accept(fd_, nullptr, nullptr);
        if (clientFd >= 0) {
            return TcpConnection(clientFd);
        }
        if (errno == EINTR) {
            continue;
        }
        lastError_ = "接受客户端连接失败: ";
        lastError_ += std::strerror(errno);
        return std::nullopt;
    }
}

const std::string& TcpServer::lastError() const {
    return lastError_;
}

void TcpServer::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

}  // namespace exf
