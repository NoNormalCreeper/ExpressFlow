#include "exf/repository/ParcelRepository.hpp"

namespace exf {

// Creates the parcel repository placeholder.
ParcelRepository::ParcelRepository() = default;

// Reports that the placeholder is constructible.
bool ParcelRepository::isReady() const
{
    return true;
}

} // namespace exf
