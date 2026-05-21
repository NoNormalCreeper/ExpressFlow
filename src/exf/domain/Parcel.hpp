#pragma once

#include "exf/domain/ParcelStatus.hpp"

#include <string>

namespace exf {

/**
 * Parcel stores the stage 1 delivery record owned by sender and receiver users.
 */
class Parcel {
public:
    /**
     * Creates an empty parcel for repository placeholders.
     */
    Parcel();

    /**
     * Creates a parcel record in the given status.
     */
    Parcel(std::string id,
           std::string senderUsername,
           std::string receiverUsername,
           std::string description,
           std::string sentAt,
           ParcelStatus status);

    /**
     * Returns the parcel tracking number.
     */
    const std::string& id() const;

    /**
     * Returns the sending user's username.
     */
    const std::string& senderUsername() const;

    /**
     * Returns the receiving user's username.
     */
    const std::string& receiverUsername() const;

    /**
     * Returns the parcel description.
     */
    const std::string& description() const;

    /**
     * Returns the sending time string.
     */
    const std::string& sentAt() const;

    /**
     * Returns the receiving time string.
     */
    const std::string& receivedAt() const;

    /**
     * Returns the current parcel status.
     */
    ParcelStatus status() const;

    /**
     * Marks the parcel as signed at the given receiving time.
     */
    void markSigned(std::string receivedAt);

private:
    /** Parcel tracking number. */
    std::string id_;

    /** Username of the sender. */
    std::string senderUsername_;

    /** Username of the receiver. */
    std::string receiverUsername_;

    /** User-provided parcel description. */
    std::string description_;

    /** Time when the parcel was submitted. */
    std::string sentAt_;

    /** Time when the receiver signed the parcel. */
    std::string receivedAt_;

    /** Stage 1 parcel status. */
    ParcelStatus status_;
};

} // namespace exf
