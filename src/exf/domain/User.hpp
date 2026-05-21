#pragma once

#include "exf/domain/Account.hpp"

#include <string>

namespace exf {

/**
 * User stores the stage 1 account information required by the assignment.
 */
class User {
public:
    /**
     * Creates an empty user object for repository placeholders.
     */
    User();

    /**
     * Creates a user with identity data and an initial balance.
     */
    User(std::string username,
         std::string name,
         std::string phone,
         std::string password,
         std::string address,
         double initialBalance);

    /**
     * Returns the unique username.
     */
    const std::string& username() const;

    /**
     * Returns the user's real name.
     */
    const std::string& name() const;

    /**
     * Returns the user's phone number.
     */
    const std::string& phone() const;

    /**
     * Returns the password placeholder used by later authentication logic.
     */
    const std::string& password() const;

    /**
     * Returns the user's address.
     */
    const std::string& address() const;

    /**
     * Returns the user's account.
     */
    const Account& account() const;

    /**
     * Replaces the user's password.
     */
    void setPassword(std::string password);

private:
    /** Unique platform username. */
    std::string username_;

    /** User's real name. */
    std::string name_;

    /** User's phone number. */
    std::string phone_;

    /** Password placeholder for stage 1 authentication. */
    std::string password_;

    /** User's delivery address. */
    std::string address_;

    /** User balance account. */
    Account account_;
};

} // namespace exf
