#include "exf/domain/Account.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::Account;
using exf::util::Money;

TEST(MoneyTest, ConvertsBetweenDecimalAndRawUnits) {
    const Money money = Money::from_double(12.3456);

    EXPECT_EQ(money.raw_value(), 123456);
    EXPECT_DOUBLE_EQ(money.to_double(), 12.3456);
    EXPECT_EQ(Money::from_string("7.5000").raw_value(), 75000);
}

TEST(MoneyTest, ComparesByRawValue) {
    const Money lower = Money::from_double(1.25);
    const Money equal = Money(12500);
    const Money higher = Money::from_double(2.00);

    EXPECT_EQ(lower, equal);
    EXPECT_NE(lower, higher);
    EXPECT_LT(lower, higher);
    EXPECT_LE(lower, equal);
    EXPECT_GT(higher, lower);
    EXPECT_GE(equal, lower);
}

TEST(AccountTest, ReportsInitialBalanceAndAffordability) {
    const Account account(Money::from_double(50.0));

    EXPECT_EQ(account.balance().raw_value(), 500000);
    EXPECT_TRUE(account.canAfford(Money::from_double(50.0)));
    EXPECT_TRUE(account.canAfford(Money::from_double(12.25)));
    EXPECT_FALSE(account.canAfford(Money::from_double(50.01)));
    EXPECT_FALSE(account.canAfford(Money::from_double(-1.0)));
}

TEST(AccountTest, CreditsOnlyPositiveAmounts) {
    Account account(Money::from_double(10.0));

    account.credit(Money::from_double(5.25));
    account.credit(Money::from_double(0.0));
    account.credit(Money::from_double(-2.0));

    EXPECT_EQ(account.balance().raw_value(), 152500);
}

TEST(AccountTest, DebitsOnlyAffordablePositiveAmounts) {
    Account account(Money::from_double(20.0));

    EXPECT_TRUE(account.debit(Money::from_double(7.5)));
    EXPECT_EQ(account.balance().raw_value(), 125000);

    EXPECT_FALSE(account.debit(Money::from_double(12.51)));
    EXPECT_EQ(account.balance().raw_value(), 125000);

    EXPECT_FALSE(account.debit(Money::from_double(-1.0)));
    EXPECT_EQ(account.balance().raw_value(), 125000);
}

}  // namespace
