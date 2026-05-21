#include "exf/service/AdminService.hpp"

namespace exf {

// Creates the administrator service placeholder.
AdminService::AdminService() = default;

// Reports that the placeholder is constructible.
bool AdminService::isReady() const
{
    return true;
}

} // namespace exf
