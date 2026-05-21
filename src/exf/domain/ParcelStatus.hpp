#pragma once

namespace exf {

/**
 * ParcelStatus lists the two parcel states required by stage 1.
 */
enum class ParcelStatus {
    /** The parcel is waiting for the receiver to sign. */
    WaitingForSign,

    /** The parcel has been signed by the receiver. */
    Signed
};

} // namespace exf
