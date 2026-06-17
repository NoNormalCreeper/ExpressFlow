#include "exf/app/stage3/Stage3Protocol.hpp"

#include <gtest/gtest.h>

#include <string>

namespace {

TEST(Stage3ProtocolTest, EncodesAndDecodesRequestWithEscapedFields) {
    const exf::Stage3Request request(
        "SEND_PARCEL", {"token", "u2", "book", "2", "C++|Book\\Guide"});

    const auto line = exf::Stage3Protocol::encodeRequest(request);
    const auto decoded = exf::Stage3Protocol::decodeRequest(line);

    ASSERT_TRUE(decoded.has_value());
    EXPECT_EQ(decoded->command(), "SEND_PARCEL");
    ASSERT_EQ(decoded->fields().size(), 5);
    EXPECT_EQ(decoded->fields()[4], "C++|Book\\Guide");
}

TEST(Stage3ProtocolTest, EncodesAndDecodesOkResponse) {
    const auto response =
        exf::Stage3Response::ok("寄件成功", {"P-1", "extra|field"});

    const auto decoded =
        exf::Stage3Protocol::decodeResponse(
            exf::Stage3Protocol::encodeResponse(response));

    ASSERT_TRUE(decoded.has_value());
    EXPECT_TRUE(decoded->isOk());
    EXPECT_EQ(decoded->message(), "寄件成功");
    ASSERT_EQ(decoded->fields().size(), 2);
    EXPECT_EQ(decoded->fields()[1], "extra|field");
}

TEST(Stage3ProtocolTest, EncodesAndDecodesErrorResponse) {
    const auto response =
        exf::Stage3Response::error("Unauthorized", "请先登录");

    const auto decoded =
        exf::Stage3Protocol::decodeResponse(
            exf::Stage3Protocol::encodeResponse(response));

    ASSERT_TRUE(decoded.has_value());
    EXPECT_FALSE(decoded->isOk());
    EXPECT_EQ(decoded->code(), "Unauthorized");
    EXPECT_EQ(decoded->message(), "请先登录");
}

}  // namespace
