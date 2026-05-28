#include "exf/util/TimeUtil.hpp"

#include <gtest/gtest.h>

#include <optional>
#include <stdexcept>
#include <string>

namespace {

using exf::TimeUtil;

TEST(TimeUtilTest, FormatsUnixSecondsTimestamp) {
    EXPECT_EQ(TimeUtil::formatTimestamp("1704067200"), "2024-01-01 08:00:00");
}

TEST(TimeUtilTest, FormatsUnixMillisecondsTimestamp) {
    EXPECT_EQ(TimeUtil::formatTimestamp("1704067200123"),
              "2024-01-01 08:00:00");
}

TEST(TimeUtilTest, RejectsInvalidTimestamp) {
    EXPECT_THROW(TimeUtil::formatTimestamp("1704067200Z"), std::invalid_argument);
}

TEST(TimeUtilTest, TreatsMissingTimestampRangeAsNoFiltering) {
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("2026-05-23 10:00:00"));
}

TEST(TimeUtilTest, ChecksUnixTimestampRangeInclusively) {
    const std::optional<std::string> from = "1704067200";
    const std::optional<std::string> to = "1704067300";

    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("1704067200", from, to));
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("1704067250", from, to));
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("1704067300", from, to));
    EXPECT_FALSE(TimeUtil::isTimestampWithinRange("1704067199", from, to));
    EXPECT_FALSE(TimeUtil::isTimestampWithinRange("1704067301", from, to));
}

TEST(TimeUtilTest, ChecksReadableTimestampRangeInclusively) {
    const std::optional<std::string> from = "2024-01-01 00:00:00";
    const std::optional<std::string> to = "2024-01-01 00:01:00";

    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("2024-01-01 00:00:00", from,
                                                 to));
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("2024-01-01 00:00:30", from,
                                                 to));
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("2024-01-01 00:01:00", from,
                                                 to));
    EXPECT_FALSE(TimeUtil::isTimestampWithinRange("2024-01-01 00:01:01",
                                                  from, to));
}

TEST(TimeUtilTest, TreatsReadableTimestampAsUtc8WhenMixedWithUnixTimestamp) {
    const std::optional<std::string> from = "2024-01-01 08:00:00";
    const std::optional<std::string> to = "2024-01-01 08:00:00";

    EXPECT_EQ(TimeUtil::formatTimestamp("2024-01-01 08:00:00"),
              "2024-01-01 08:00:00");
    EXPECT_TRUE(TimeUtil::isTimestampWithinRange("1704067200", from, to));
}

TEST(TimeUtilTest, RejectsInvalidTimestampWhenRangeIsProvided) {
    const std::optional<std::string> from = "1704067200";

    EXPECT_FALSE(TimeUtil::isTimestampWithinRange("not-a-timestamp", from));
    EXPECT_FALSE(TimeUtil::isTimestampWithinRange("1704067250",
                                                 std::optional<std::string>{
                                                     "invalid-bound"}));
}

}  // namespace
