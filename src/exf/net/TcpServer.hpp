#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "exf/net/TcpConnection.hpp"

namespace exf {

/**
 * TcpServer 封装服务端监听 socket。
 */
class TcpServer {
   public:
    /**
     * 创建未监听的服务端。
     */
    TcpServer();

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    /**
     * 关闭监听 socket。
     */
    ~TcpServer();

    /**
     * 监听指定地址和端口。
     */
    bool listenOn(const std::string& host, uint16_t port);

    /**
     * 接受一个客户端连接。
     */
    std::optional<TcpConnection> acceptConnection();

    /**
     * 返回最近一次错误。
     */
    const std::string& lastError() const;

    /**
     * 主动关闭监听 socket。
     */
    void close();

   private:
    /** 监听 socket 文件描述符。 */
    int fd_;

    /** 最近一次错误。 */
    std::string lastError_;
};

}  // namespace exf
