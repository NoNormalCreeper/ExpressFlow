#include "exf/domain/Parcel.hpp"

#include <utility>

namespace exf {

// Creates an empty parcel placeholder.
Parcel::Parcel() : status_(ParcelStatus::WaitingForSign) {}

// Stores the stage 1 parcel data.
Parcel::Parcel(std::string id,
               std::string senderUsername,
               std::string receiverUsername,
               std::string description,
               std::string sentAt,
               ParcelStatus status)
    : id_(std::move(id)),
      senderUsername_(std::move(senderUsername)),
      receiverUsername_(std::move(receiverUsername)),
      description_(std::move(description)),
      sentAt_(std::move(sentAt)),
      status_(status)
{
}

// Returns the parcel tracking number.
const std::string& Parcel::id() const
{
    return id_;
}

// Returns the sender username.
const std::string& Parcel::senderUsername() const
{
    return senderUsername_;
}

// Returns the receiver username.
const std::string& Parcel::receiverUsername() const
{
    return receiverUsername_;
}

// Returns the parcel description.
const std::string& Parcel::description() const
{
    return description_;
}

// Returns the sending time.
const std::string& Parcel::sentAt() const
{
    return sentAt_;
}

// Returns the receiving time.
const std::string& Parcel::receivedAt() const
{
    return receivedAt_;
}

// Returns the parcel status.
ParcelStatus Parcel::status() const
{
    return status_;
}

// Marks the parcel as signed and stores the receiving time.
void Parcel::markSigned(std::string receivedAt)
{
    receivedAt_ = std::move(receivedAt);
    status_ = ParcelStatus::Signed;
}

} // namespace exf
