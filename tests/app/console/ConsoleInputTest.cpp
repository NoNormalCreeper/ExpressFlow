#include "exf/app/console/ConsoleInput.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

namespace {

using exf::ConsoleInput;

class ScopedCinRedirect {
   public:
    explicit ScopedCinRedirect(std::istream& input)
        : original_(std::cin.rdbuf(input.rdbuf())) {}

    ~ScopedCinRedirect() {
        std::cin.rdbuf(original_);
    }

   private:
    std::streambuf* original_;
};

TEST(ConsoleInputTest, OptionalTextReturnsEmptyOptionalForBlankInput) {
    std::istringstream input("\n");
    ScopedCinRedirect redirect(input);

    const auto value = ConsoleInput::promptOptionalText("optional text");

    EXPECT_FALSE(value.has_value());
}

TEST(ConsoleInputTest, OptionalTextReturnsEnteredText) {
    std::istringstream input("hello\n");
    ScopedCinRedirect redirect(input);

    const auto value = ConsoleInput::promptOptionalText("optional text");

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "hello");
}

TEST(ConsoleInputTest, OptionalTimestampRejectsInvalidInputUntilValid) {
    std::istringstream input("bad\n2026-05-23 10:00:00\n");
    ScopedCinRedirect redirect(input);

    const auto value = ConsoleInput::promptOptionalTimestamp("optional time");

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "2026-05-23 10:00:00");
}

TEST(ConsoleInputTest, OptionalTimestampRejectsUnixTimestampInput) {
    std::istringstream input("4\n\n");
    ScopedCinRedirect redirect(input);

    const auto value = ConsoleInput::promptOptionalTimestamp("optional time");

    EXPECT_FALSE(value.has_value());
}

}  // namespace
