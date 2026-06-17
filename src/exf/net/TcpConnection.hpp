#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace exf {

/**
 * TcpConnection 封装一个已建立的 TCP 连接。
 */
class TcpConnection {
   public:
    /**
     * 创建空连接。
     */
    TcpConnection();

    /**
     * 接管 socket 文件描述符。
     */
    explicit TcpConnection(int fd);

    TcpConnection(const TcpConnection&) = delete;
    TcpConnection& operator=(const TcpConnection&) = delete;

    TcpConnection(TcpConnection&& other) noexcept;
    TcpConnection& operator=(TcpConnection&& other) noexcept;

    /**
     * 关闭连接。
     */
    ~TcpConnection();

    /**
     * 返回连接是否可用。
     */
    bool isOpen() const;

    /**
     * 发送一行文本，自动补齐行尾换行。
     */
    bool sendLine(std::string_view line);

    /**
     * 接收一行文本，不包含行尾换行；断开时返回空。
     */
    std::optional<std::string> receiveLine();

    /**
     * 主动关闭连接。
     */
    void close();

   private:
    /** socket 文件描述符。 */
    int fd_;

    bool sendAll(const char* data, size_t size);
};

}  // namespace exf
