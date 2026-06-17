#pragma once

#include <cstdint>
#include <string>

#include "exf/net/TcpConnection.hpp"

namespace exf {

/**
 * TcpClient 封装客户端主动连接服务端的 socket 操作。
 */
class TcpClient {
   public:
    /**
     * 连接服务端。
     */
    bool connectTo(const std::string& host, uint16_t port);

    /**
     * 返回连接对象。
     */
    TcpConnection& connection();
    const TcpConnection& connection() const;

    /**
     * 返回最近一次错误。
     */
    const std::string& lastError() const;

   private:
    /** TCP 连接。 */
    TcpConnection connection_;

    /** 最近一次错误。 */
    std::string lastError_;
};

}  // namespace exf
