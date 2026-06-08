#include "exf/domain/Courier.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::Courier;
using exf::util::Money;

TEST(CourierTest, StoresIdentityLoginAndAccountInformation) {
    Courier courier("courier01",
                    "Li Ming",
                    "13800000000",
                    "secret",
                    Money::from_double(12.5));

    EXPECT_EQ(courier.username(), "courier01");
    EXPECT_EQ(courier.name(), "Li Ming");
    EXPECT_EQ(courier.phone(), "13800000000");
    EXPECT_EQ(courier.password(), "secret");
    EXPECT_EQ(courier.account().balance().raw_value(),
              Money::from_double(12.5).raw_value());
}

TEST(CourierTest, AllowsPasswordAndBalanceChangesThroughObjectBehavior) {
    Courier courier("courier01",
                    "Li Ming",
                    "13800000000",
                    "secret",
                    Money::from_double(0.0));

    courier.setPassword("new-secret");
    courier.account().credit(Money::from_double(7.5));

    EXPECT_EQ(courier.password(), "new-secret");
    EXPECT_EQ(courier.account().balance().raw_value(),
              Money::from_double(7.5).raw_value());
}

}  // namespace
