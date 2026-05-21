#include "exf/repository/AdminRepository.hpp"

namespace exf {

// Creates the administrator repository placeholder.
AdminRepository::AdminRepository() = default;

// Reports that the placeholder is constructible.
bool AdminRepository::isReady() const
{
    return true;
}

} // namespace exf
