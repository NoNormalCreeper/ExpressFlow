#include "exf/domain/Item.hpp"
#include "exf/domain/ParcelItemType.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::BookItem;
using exf::FragileItem;
using exf::Item;
using exf::ParcelItemType;
using exf::StandardItem;
using exf::util::Money;

TEST(ItemTest, StandardItemCalculatesFiveYuanPerKilogram) {
    const StandardItem item(2.5);
    const Item& base = item;

    EXPECT_TRUE(base.isValid());
    EXPECT_EQ(base.type(), ParcelItemType::Standard);
    EXPECT_DOUBLE_EQ(base.amount(), 2.5);
    EXPECT_EQ(base.getPrice().raw_value(), Money::from_double(12.5).raw_value());
}

TEST(ItemTest, FragileItemCalculatesEightYuanPerKilogram) {
    const FragileItem item(3.0);
    const Item& base = item;

    EXPECT_TRUE(base.isValid());
    EXPECT_EQ(base.type(), ParcelItemType::Fragile);
    EXPECT_DOUBLE_EQ(base.amount(), 3.0);
    EXPECT_EQ(base.getPrice().raw_value(), Money::from_double(24.0).raw_value());
}

TEST(ItemTest, BookItemCalculatesTwoYuanPerBook) {
    const BookItem item(5);
    const Item& base = item;

    EXPECT_TRUE(base.isValid());
    EXPECT_EQ(base.type(), ParcelItemType::Book);
    EXPECT_DOUBLE_EQ(base.amount(), 5.0);
    EXPECT_EQ(base.getPrice().raw_value(), Money::from_double(10.0).raw_value());
}

TEST(ItemTest, InvalidAmountsAreRejectedByItemObjects) {
    const StandardItem standard(0.0);
    const FragileItem fragile(-1.0);
    const BookItem book(0);

    EXPECT_FALSE(standard.isValid());
    EXPECT_FALSE(fragile.isValid());
    EXPECT_FALSE(book.isValid());
    EXPECT_EQ(standard.getPrice().raw_value(), Money::from_double(0.0).raw_value());
    EXPECT_EQ(fragile.getPrice().raw_value(), Money::from_double(0.0).raw_value());
    EXPECT_EQ(book.getPrice().raw_value(), Money::from_double(0.0).raw_value());
}

}  // namespace
