#include "exf/app/stage3/Stage3SessionManager.hpp"

#include <gtest/gtest.h>

namespace {

TEST(Stage3SessionManagerTest, CreatesFindsAndRemovesSessions) {
    exf::Stage3SessionManager sessions;

    const auto token = sessions.createSession(exf::Stage3Role::User, "u1");
    const auto session = sessions.findSession(token);

    ASSERT_TRUE(session.has_value());
    EXPECT_EQ(session->role(), exf::Stage3Role::User);
    EXPECT_EQ(session->username(), "u1");

    sessions.removeSession(token);
    EXPECT_FALSE(sessions.findSession(token).has_value());
}

TEST(Stage3SessionManagerTest, NewLoginInvalidatesOldSessionForSameAccount) {
    exf::Stage3SessionManager sessions;

    const auto oldToken = sessions.createSession(exf::Stage3Role::User, "u1");
    const auto newToken = sessions.createSession(exf::Stage3Role::User, "u1");

    EXPECT_FALSE(sessions.findSession(oldToken).has_value());
    EXPECT_TRUE(sessions.findSession(newToken).has_value());
}

}  // namespace
