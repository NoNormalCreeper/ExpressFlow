#pragma once

namespace exf {

/**
 * Account stores a simple money balance for a platform user or administrator.
 */
class Account {
public:
    /**
     * Creates an account with the given initial balance.
     */
    explicit Account(double initialBalance = 0.0);

    /**
     * Returns the current balance without changing the account.
     */
    double balance() const;

    /**
     * Returns whether the account can pay the requested amount.
     */
    bool canAfford(double amount) const;

    /**
     * Adds a positive amount to the account balance.
     */
    void credit(double amount);

    /**
     * Tries to subtract a positive amount from the account balance.
     */
    bool debit(double amount);

private:
    /** Current account balance. */
    double balance_;
};

} // namespace exf
