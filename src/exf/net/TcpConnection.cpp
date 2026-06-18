#include "exf/net/TcpConnection.hpp"

#include <cerrno>
#include <cstddef>
#include <sys/socket.h>
#include <unistd.h>

namespace exf {
namespace {

constexpr size_t kMaxLineLength = 8192;

}  // namespace

TcpConnection::TcpConnection() : fd_(-1) {}

TcpConnection::TcpConnection(int fd) : fd_(fd) {}

TcpConnection::TcpConnection(TcpConnection&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

TcpConnection& TcpConnection::operator=(TcpConnection&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

TcpConnection::~TcpConnection() {
    close();
}

bool TcpConnection::isOpen() const {
    return fd_ >= 0;
}

bool TcpConnection::sendLine(std::string_view line) {
    std::string payload(line);
    if (payload.empty() || payload.back() != '\n') {
        payload.push_back('\n');
    }
    return sendAll(payload.data(), payload.size());
}

std::optional<std::string> TcpConnection::receiveLine() {
    if (!isOpen()) {
        return std::nullopt;
    }

    std::string line;
    char ch = '\0';
    while (true) {
        const ssize_t n = ::recv(fd_, &ch, 1, 0);
        if (n == 0) {
            close();
            if (line.empty()) {
                return std::nullopt;
            }
            return line;
        }
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            close();
            return std::nullopt;
        }
        if (ch == '\n') {
            return line;
        }
        if (ch != '\r') {
            line.push_back(ch);
            if (line.size() > kMaxLineLength) {
                close();
                return std::nullopt;
            }
        }
    }
}

void TcpConnection::close() {
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
}

bool TcpConnection::sendAll(const char* data, size_t size) {
    if (!isOpen()) {
        return false;
    }

    size_t sent = 0;
    while (sent < size) {
        const ssize_t n = ::send(fd_, data + sent, size - sent, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            close();
            return false;
        }
        if (n == 0) {
            close();
            return false;
        }
        sent += static_cast<size_t>(n);
    }
    return true;
}

}  // namespace exf
