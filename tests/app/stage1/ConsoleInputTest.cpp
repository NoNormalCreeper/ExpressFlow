#include "exf/app/stage1/ConsoleInput.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

namespace {

using exf::ConsoleInput;

class ConsoleStreamRedirect {
   public:
    explicit ConsoleStreamRedirect(std::string input)
        : input_(std::move(input)),
          originalCin_(std::cin.rdbuf(input_.rdbuf())),
          originalCout_(std::cout.rdbuf(output_.rdbuf())) {}

    ~ConsoleStreamRedirect() {
        std::cin.rdbuf(originalCin_);
        std::cout.rdbuf(originalCout_);
    }

    std::string output() const {
        return output_.str();
    }

   private:
    std::istringstream input_;
    std::ostringstream output_;
    std::streambuf* originalCin_;
    std::streambuf* originalCout_;
};

TEST(ConsoleInputTest, OptionalTextReturnsEmptyOptionalForBlankInput) {
    ConsoleStreamRedirect console("\n");

    const auto value = ConsoleInput::promptOptionalText("optional text");

    EXPECT_FALSE(value.has_value());
}

TEST(ConsoleInputTest, OptionalTextReturnsEnteredText) {
    ConsoleStreamRedirect console("alice\n");

    const auto value = ConsoleInput::promptOptionalText("optional text");

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "alice");
}

TEST(ConsoleInputTest, OptionalTimestampReturnsEmptyOptionalForBlankInput) {
    ConsoleStreamRedirect console("\n");

    const auto value = ConsoleInput::promptOptionalTimestamp("optional time");

    EXPECT_FALSE(value.has_value());
}

TEST(ConsoleInputTest, OptionalTimestampRejectsInvalidInputUntilValid) {
    ConsoleStreamRedirect console("bad-time\n2024-01-01 08:00:00\n");

    const auto value = ConsoleInput::promptOptionalTimestamp("optional time");

    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(*value, "2024-01-01 08:00:00");
    EXPECT_NE(console.output().find("请输入有效时间"), std::string::npos);
}

}  // namespace
