#include "exf/net/TcpConnection.hpp"

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

#include <string>

namespace {

TEST(TcpConnectionTest, ReceiveLineReturnsNulloptAfterPeerClose) {
    int fds[2]{};
    ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

    exf::TcpConnection connection(fds[0]);
    ::close(fds[1]);

    EXPECT_FALSE(connection.receiveLine().has_value());
    EXPECT_FALSE(connection.isOpen());
}

TEST(TcpConnectionTest, SendLineReturnsFalseAfterPeerClose) {
    int fds[2]{};
    ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

    exf::TcpConnection connection(fds[0]);
    ::close(fds[1]);

    EXPECT_FALSE(connection.sendLine("PING"));
    EXPECT_FALSE(connection.isOpen());
}

TEST(TcpConnectionTest, ReceiveLineClosesOverlongLine) {
    int fds[2]{};
    ASSERT_EQ(::socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);

    exf::TcpConnection connection(fds[0]);
    std::string payload(8193, 'x');
    ASSERT_EQ(::send(fds[1], payload.data(), payload.size(), 0),
              static_cast<ssize_t>(payload.size()));

    EXPECT_FALSE(connection.receiveLine().has_value());
    EXPECT_FALSE(connection.isOpen());
    ::close(fds[1]);
}

}  // namespace
