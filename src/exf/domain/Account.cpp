#include "exf/domain/Account.hpp"

namespace exf {

// Stores the initial balance for a new account.
Account::Account(double initialBalance) : balance_(initialBalance) {}

// Returns the account balance.
double Account::balance() const
{
    return balance_;
}

// Checks whether the balance covers a positive payment amount.
bool Account::canAfford(double amount) const
{
    return amount >= 0.0 && balance_ >= amount;
}

// Adds positive money to the balance.
void Account::credit(double amount)
{
    if (amount > 0.0) {
        balance_ += amount;
    }
}

// Subtracts positive money when the balance is sufficient.
bool Account::debit(double amount)
{
    if (!canAfford(amount)) {
        return false;
    }

    balance_ -= amount;
    return true;
}

} // namespace exf
