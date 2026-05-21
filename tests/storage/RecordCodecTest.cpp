#include "exf/storage/RecordCodec.hpp"

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace {

using exf::RecordCodec;

TEST(RecordCodecTest, JoinsAndSplitsOrdinaryFields) {
    const std::vector<std::string> fields = {"alpha", "beta", "gamma"};

    EXPECT_EQ(RecordCodec::delimiter(), '|');
    EXPECT_EQ(RecordCodec::join(fields), "alpha|beta|gamma");
    EXPECT_EQ(RecordCodec::split("alpha|beta|gamma"), fields);
}

TEST(RecordCodecTest, PreservesEmptyFields) {
    const std::vector<std::string> fields = {"", "alpha", "", ""};

    EXPECT_EQ(RecordCodec::join(fields), "|alpha||");
    EXPECT_EQ(RecordCodec::split("|alpha||"), fields);
}

TEST(RecordCodecTest, EscapesDelimitersAndBackslashesWhenJoining) {
    const std::vector<std::string> fields = {
        "alpha|beta",
        R"(C:\tmp\file)",
        R"(pipe|and\slash)",
    };

    const std::string encoded = RecordCodec::join(fields);

    EXPECT_EQ(encoded, R"(alpha\|beta|C:\\tmp\\file|pipe\|and\\slash)");
    EXPECT_EQ(RecordCodec::split(encoded), fields);
}

TEST(RecordCodecTest, SplitsEscapedDelimitersAndBackslashesLiterally) {
    const std::vector<std::string> expected = {
        "alpha|beta",
        R"(C:\tmp)",
        "plain",
    };

    EXPECT_EQ(RecordCodec::split(R"(alpha\|beta|C:\\tmp|plain)"), expected);
}

TEST(RecordCodecTest, TreatsTrailingEscapeAsLiteralBackslash) {
    const std::vector<std::string> expected = {"unfinished\\"};

    EXPECT_EQ(RecordCodec::split("unfinished\\"), expected);
}

}  // namespace
