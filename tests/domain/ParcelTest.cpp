#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelStatus.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::Parcel;
using exf::ParcelStatus;
using exf::util::Money;

TEST(ParcelTest, NewParcelStartsWaitingForReceiverWithoutReceivedTime) {
    const Parcel parcel =
        Parcel::createNew("P-1001", "alice", "bob", "documents",
                          "2026-05-23 10:00:00", Money::from_double(15.0));

    EXPECT_EQ(parcel.id(), "P-1001");
    EXPECT_EQ(parcel.senderUsername(), "alice");
    EXPECT_EQ(parcel.receiverUsername(), "bob");
    EXPECT_EQ(parcel.description(), "documents");
    EXPECT_EQ(parcel.sentAt(), "2026-05-23 10:00:00");
    EXPECT_EQ(parcel.receivedAt(), "");
    EXPECT_EQ(parcel.fee().raw_value(), Money::from_double(15.0).raw_value());
    EXPECT_EQ(parcel.status(), ParcelStatus::WaitingForSign);
}

TEST(ParcelTest, MarkSignedRecordsReceiveTimeAndSignedStatus) {
    Parcel parcel =
        Parcel::createNew("P-1002", "alice", "bob", "books",
                          "2026-05-23 11:00:00", Money::from_double(15.0));

    parcel.markSigned("2026-05-23 12:30:00");

    EXPECT_EQ(parcel.receivedAt(), "2026-05-23 12:30:00");
    EXPECT_EQ(parcel.status(), ParcelStatus::Signed);
}

}  // namespace
