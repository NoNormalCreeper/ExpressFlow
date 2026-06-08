#include "exf/domain/Courier.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

namespace {

using exf::Courier;
using exf::CourierRecordCodec;
using exf::util::Money;

TEST(CourierRecordCodecTest, EncodesAndDecodesCourierWithEscapedFields) {
    const Courier courier("courier|01",
                          R"(Li \ Ming)",
                          "138|0000",
                          R"(pa|ss\word)",
                          Money::from_double(18.5));

    const std::string encoded = CourierRecordCodec::encode(courier);
    const Courier decoded = CourierRecordCodec::decode(encoded);

    EXPECT_EQ(decoded.username(), courier.username());
    EXPECT_EQ(decoded.name(), courier.name());
    EXPECT_EQ(decoded.phone(), courier.phone());
    EXPECT_EQ(decoded.password(), courier.password());
    EXPECT_EQ(decoded.account().balance().raw_value(),
              courier.account().balance().raw_value());
}

TEST(CourierRecordCodecTest, RejectsRecordsWithWrongFieldCount) {
    EXPECT_THROW(CourierRecordCodec::decode("only|four|fields|here"),
                 std::runtime_error);
}

}  // namespace
