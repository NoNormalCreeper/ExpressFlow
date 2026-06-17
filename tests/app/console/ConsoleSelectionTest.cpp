#include "exf/app/console/ConsoleSelection.hpp"

#include <gtest/gtest.h>

namespace {

using exf::ConsoleSelection;

TEST(ConsoleSelectionTest, BlankInputReturnsEmptySelection) {
    const auto result = ConsoleSelection::parseSelectionIndices("   ", 3);

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->empty());
}

TEST(ConsoleSelectionTest, AllInputSelectsEveryIndex) {
    const auto result = ConsoleSelection::parseSelectionIndices("all", 3);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, (std::vector<size_t>{0, 1, 2}));
}

TEST(ConsoleSelectionTest, ParsesCommaAndSpaceSeparatedInput) {
    const auto result =
        ConsoleSelection::parseSelectionIndices("1, 3 2", 3);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, (std::vector<size_t>{0, 2, 1}));
}

TEST(ConsoleSelectionTest, DeduplicatesRepeatedIndices) {
    const auto result = ConsoleSelection::parseSelectionIndices("2 2,1", 3);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, (std::vector<size_t>{1, 0}));
}

TEST(ConsoleSelectionTest, RejectsOutOfRangeInput) {
    EXPECT_FALSE(ConsoleSelection::parseSelectionIndices("4", 3).has_value());
    EXPECT_FALSE(ConsoleSelection::parseSelectionIndices("0", 3).has_value());
}

TEST(ConsoleSelectionTest, RejectsNonNumericInput) {
    EXPECT_FALSE(ConsoleSelection::parseSelectionIndices("1 abc", 3)
                     .has_value());
}

}  // namespace
