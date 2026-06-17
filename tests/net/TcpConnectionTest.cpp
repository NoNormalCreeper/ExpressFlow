#include "exf/net/TcpConnection.hpp"

#include <gtest/gtest.h>
#include <sys/socket.h>
#include <unistd.h>

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

}  // namespace
