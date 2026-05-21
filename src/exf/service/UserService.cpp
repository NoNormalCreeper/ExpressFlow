#include "exf/service/UserService.hpp"

namespace exf {

// Creates the user service placeholder.
UserService::UserService() = default;

// Reports that the placeholder is constructible.
bool UserService::isReady() const
{
    return true;
}

} // namespace exf
