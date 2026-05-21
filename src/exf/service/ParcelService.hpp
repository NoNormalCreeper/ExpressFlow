#pragma once

namespace exf {

/**
 * ParcelService will hold parcel sending, signing, and query workflows.
 */
class ParcelService {
public:
    /**
     * Creates a service placeholder.
     */
    ParcelService();

    /**
     * Returns the fixed stage 1 parcel price.
     */
    double stage1ParcelPrice() const;
};

} // namespace exf
