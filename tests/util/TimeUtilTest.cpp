#include "exf/util/TimeUtil.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace {

using exf::TimeUtil;

TEST(TimeUtilTest, FormatsUnixSecondsTimestamp) {
    EXPECT_EQ(TimeUtil::formatTimestamp("1704067200"), "2024-01-01 00:00:00");
}

TEST(TimeUtilTest, FormatsUnixMillisecondsTimestamp) {
    EXPECT_EQ(TimeUtil::formatTimestamp("1704067200123"),
              "2024-01-01 00:00:00");
}

TEST(TimeUtilTest, RejectsInvalidTimestamp) {
    EXPECT_THROW(TimeUtil::formatTimestamp("1704067200Z"), std::invalid_argument);
}

}  // namespace
