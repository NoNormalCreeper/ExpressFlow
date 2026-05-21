#include "exf/domain/User.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::User;
using exf::UserRecordCodec;
using exf::util::Money;

TEST(UserRecordCodecTest, EncodesAndDecodesUserWithEscapedFields) {
    const User user("user|42",
                    R"(Alice \ Admin)",
                    "138|0000",
                    R"(pa|ss\word)",
                    R"(42 Back\Street | Apt)",
                    Money::from_double(42.5));

    const std::string encoded = UserRecordCodec::encode(user);
    const User decoded = UserRecordCodec::decode(encoded);

    EXPECT_EQ(decoded.username(), user.username());
    EXPECT_EQ(decoded.name(), user.name());
    EXPECT_EQ(decoded.phone(), user.phone());
    EXPECT_EQ(decoded.password(), user.password());
    EXPECT_EQ(decoded.address(), user.address());
    EXPECT_EQ(decoded.account().balance().raw_value(),
              user.account().balance().raw_value());
}

TEST(UserRecordCodecTest, RejectsRecordsWithWrongFieldCount) {
    EXPECT_THROW(UserRecordCodec::decode("only|five|fields|are|here"),
                 std::runtime_error);
}

}  // namespace
