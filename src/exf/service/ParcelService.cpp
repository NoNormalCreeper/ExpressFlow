#include "exf/service/ParcelService.hpp"

namespace exf {

// Creates the parcel service placeholder.
ParcelService::ParcelService() = default;

// Returns the fixed price required by stage 1.
double ParcelService::stage1ParcelPrice() const
{
    return 15.0;
}

} // namespace exf
