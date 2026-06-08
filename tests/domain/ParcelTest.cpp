#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelItemType.hpp"
#include "exf/domain/ParcelStatus.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

namespace {

using exf::Parcel;
using exf::ParcelItemType;
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

TEST(ParcelTest, Stage2ParcelStartsWaitingForPickupWithItemDetails) {
    const Parcel parcel = Parcel::createWaitingForPickup(
        "P-2001",
        "alice",
        "bob",
        "glass cups",
        "2026-05-23 10:00:00",
        Money::from_double(24.0),
        ParcelItemType::Fragile,
        3.0);

    EXPECT_EQ(parcel.status(), ParcelStatus::WaitingForPickup);
    EXPECT_EQ(parcel.itemType(), ParcelItemType::Fragile);
    EXPECT_DOUBLE_EQ(parcel.itemAmount(), 3.0);
    EXPECT_EQ(parcel.courierUsername(), "");
    EXPECT_EQ(parcel.pickedAt(), "");
    EXPECT_EQ(parcel.receivedAt(), "");
}

TEST(ParcelTest, Stage2ParcelRecordsCourierAssignmentAndPickup) {
    Parcel parcel = Parcel::createWaitingForPickup(
        "P-2002",
        "alice",
        "bob",
        "books",
        "2026-05-23 11:00:00",
        Money::from_double(10.0),
        ParcelItemType::Book,
        5.0);

    parcel.assignCourier("courier01");
    parcel.markPickedUp("2026-05-23 11:30:00");

    EXPECT_EQ(parcel.courierUsername(), "courier01");
    EXPECT_EQ(parcel.pickedAt(), "2026-05-23 11:30:00");
    EXPECT_EQ(parcel.status(), ParcelStatus::WaitingForSign);
}

}  // namespace
