#pragma once

namespace exf {

/**
 * ParcelRepository is the stage 1 placeholder for parcel persistence.
 */
class ParcelRepository {
public:
    /**
     * Creates a repository placeholder.
     */
    ParcelRepository();

    /**
     * Returns whether the repository placeholder is usable.
     */
    bool isReady() const;
};

} // namespace exf
