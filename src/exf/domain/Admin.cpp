#include "exf/domain/Admin.hpp"

#include <utility>

namespace exf {

// Creates an empty administrator placeholder.
Admin::Admin() : account_(0.0) {}

// Stores administrator identity data and balance.
Admin::Admin(std::string username, std::string name, std::string password, double initialBalance)
    : username_(std::move(username)),
      name_(std::move(name)),
      password_(std::move(password)),
      account_(initialBalance)
{
}

// Returns the unique administrator username.
const std::string& Admin::username() const
{
    return username_;
}

// Returns the administrator display name.
const std::string& Admin::name() const
{
    return name_;
}

// Returns the administrator password placeholder.
const std::string& Admin::password() const
{
    return password_;
}

// Returns the administrator account.
const Account& Admin::account() const
{
    return account_;
}

} // namespace exf
