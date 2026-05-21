#include "exf/domain/User.hpp"

#include <utility>

namespace exf {

// Creates an empty user placeholder.
User::User() : account_(0.0) {}

// Stores user identity data and balance.
User::User(std::string username,
           std::string name,
           std::string phone,
           std::string password,
           std::string address,
           double initialBalance)
    : username_(std::move(username)),
      name_(std::move(name)),
      phone_(std::move(phone)),
      password_(std::move(password)),
      address_(std::move(address)),
      account_(initialBalance)
{
}

// Returns the unique username.
const std::string& User::username() const
{
    return username_;
}

// Returns the user's real name.
const std::string& User::name() const
{
    return name_;
}

// Returns the phone number.
const std::string& User::phone() const
{
    return phone_;
}

// Returns the password placeholder.
const std::string& User::password() const
{
    return password_;
}

// Returns the delivery address.
const std::string& User::address() const
{
    return address_;
}

// Returns the user account.
const Account& User::account() const
{
    return account_;
}

// Replaces the stored password.
void User::setPassword(std::string password)
{
    password_ = std::move(password);
}

} // namespace exf
