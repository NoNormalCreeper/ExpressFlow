#include "exf/repository/UserRepository.hpp"

namespace exf {

// Creates the user repository placeholder.
UserRepository::UserRepository() = default;

// Reports that the placeholder is constructible.
bool UserRepository::isReady() const
{
    return true;
}

} // namespace exf
