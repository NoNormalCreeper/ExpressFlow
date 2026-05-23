#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelStatus.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

namespace {

using exf::Parcel;
using exf::ParcelRecordCodec;
using exf::ParcelStatus;
using exf::util::Money;

TEST(ParcelRecordCodecTest, EncodesAndDecodesWaitingParcelWithEscapedFields) {
    const Parcel parcel = Parcel::createNew(
        "P|1001", R"(alice\sender)", "bob|receiver", R"(fragile | glass\ware)",
        "2026-05-23 10:00:00", Money::from_double(15.75));

    const std::string encoded = ParcelRecordCodec::encode(parcel);
    const Parcel decoded = ParcelRecordCodec::decode(encoded);

    EXPECT_EQ(decoded.id(), parcel.id());
    EXPECT_EQ(decoded.senderUsername(), parcel.senderUsername());
    EXPECT_EQ(decoded.receiverUsername(), parcel.receiverUsername());
    EXPECT_EQ(decoded.description(), parcel.description());
    EXPECT_EQ(decoded.sentAt(), parcel.sentAt());
    EXPECT_EQ(decoded.receivedAt(), "");
    EXPECT_EQ(decoded.fee().raw_value(), parcel.fee().raw_value());
    EXPECT_EQ(decoded.status(), ParcelStatus::WaitingForSign);
}

TEST(ParcelRecordCodecTest, EncodesAndDecodesSignedParcelWithReceivedTime) {
    const Parcel parcel("P-1002", "alice", "bob", "books",
                        "2026-05-23 10:00:00", "2026-05-23 12:30:00",
                        Money::from_double(20.0), ParcelStatus::Signed);

    const Parcel decoded =
        ParcelRecordCodec::decode(ParcelRecordCodec::encode(parcel));

    EXPECT_EQ(decoded.receivedAt(), "2026-05-23 12:30:00");
    EXPECT_EQ(decoded.fee().raw_value(), Money::from_double(20.0).raw_value());
    EXPECT_EQ(decoded.status(), ParcelStatus::Signed);
}

TEST(ParcelRecordCodecTest, RejectsRecordsWithWrongFieldCount) {
    EXPECT_THROW(ParcelRecordCodec::decode("P-1|alice|bob|description"),
                 std::runtime_error);
}

}  // namespace
