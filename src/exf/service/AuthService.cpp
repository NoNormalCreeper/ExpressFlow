#include "exf/service/AuthService.hpp"

namespace exf {

// Creates the authentication service placeholder.
AuthService::AuthService() = default;

// Reports that the placeholder is constructible.
bool AuthService::isReady() const
{
    return true;
}

} // namespace exf
