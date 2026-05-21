#pragma once

#include "exf/domain/Account.hpp"

#include <string>

namespace exf {

/**
 * Admin represents the logistics company administrator for stage 1.
 */
class Admin {
public:
    /**
     * Creates an empty administrator object for repository placeholders.
     */
    Admin();

    /**
     * Creates an administrator with identity data and an initial balance.
     */
    Admin(std::string username, std::string name, std::string password, double initialBalance);

    /**
     * Returns the unique administrator username.
     */
    const std::string& username() const;

    /**
     * Returns the administrator display name.
     */
    const std::string& name() const;

    /**
     * Returns the administrator password placeholder.
     */
    const std::string& password() const;

    /**
     * Returns the administrator account.
     */
    const Account& account() const;

private:
    /** Unique administrator username. */
    std::string username_;

    /** Administrator display name. */
    std::string name_;

    /** Password placeholder for later authentication work. */
    std::string password_;

    /** Account that receives freight charges in stage 1. */
    Account account_;
};

} // namespace exf
