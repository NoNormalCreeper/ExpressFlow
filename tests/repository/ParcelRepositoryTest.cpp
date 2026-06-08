#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelItemType.hpp"
#include "exf/domain/ParcelStatus.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace {

using exf::FileStorage;
using exf::Parcel;
using exf::ParcelItemType;
using exf::ParcelRecordCodec;
using exf::ParcelRepository;
using exf::ParcelStatus;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-parcel-repository-test-" +
                 std::to_string(std::chrono::steady_clock::now()
                                    .time_since_epoch()
                                    .count()))) {
        std::filesystem::create_directories(path_);
    }

    ~TempDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }

    const std::filesystem::path& path() const { return path_; }

   private:
    std::filesystem::path path_;
};

Parcel makeWaitingForPickupParcel(std::string id,
                                  std::string sender,
                                  std::string receiver,
                                  std::string description = "documents") {
    return Parcel::createWaitingForPickup(
        std::move(id),
        std::move(sender),
        std::move(receiver),
        std::move(description),
        "2026-05-23 10:00:00",
        Money::from_double(15.0),
        ParcelItemType::Standard,
        3.0);
}

Parcel makeWaitingForSignParcel(std::string id,
                                std::string sender,
                                std::string receiver,
                                std::string description = "documents") {
    Parcel parcel = makeWaitingForPickupParcel(std::move(id),
                                               std::move(sender),
                                               std::move(receiver),
                                               std::move(description));
    parcel.assignCourier("c01");
    parcel.markPickedUp("2026-05-23 11:00:00");
    return parcel;
}

Parcel makeSignedParcel(std::string id,
                        std::string sender,
                        std::string receiver,
                        std::string receivedAt = "2026-05-23 12:30:00") {
    return Parcel(std::move(id), std::move(sender), std::move(receiver),
                  "books", "2026-05-23 09:00:00", std::move(receivedAt),
                  Money::from_double(15.0), ParcelStatus::Signed);
}

std::vector<std::string> sortedParcelIds(const std::vector<Parcel>& parcels) {
    std::vector<std::string> ids;
    ids.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        ids.push_back(parcel.id());
    }
    std::sort(ids.begin(), ids.end());
    return ids;
}

TEST(ParcelRepositoryTest, LoadsExistingParcelsFromStorageOnConstruction) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    storage.writeLines("parcels.txt",
                       {
                           ParcelRecordCodec::encode(
                               makeWaitingForPickupParcel("P-1001", "alice", "bob")),
                           ParcelRecordCodec::encode(
                               makeSignedParcel("P-1002", "carol", "bob")),
                       });

    const ParcelRepository repository(storage);

    const Parcel* parcel = repository.findParcel("P-1001");
    ASSERT_NE(parcel, nullptr);
    EXPECT_EQ(parcel->senderUsername(), "alice");
    EXPECT_EQ(parcel->receiverUsername(), "bob");
    EXPECT_EQ(parcel->status(), ParcelStatus::WaitingForPickup);
    EXPECT_EQ(repository.listAll().size(), 2U);
}

TEST(ParcelRepositoryTest, CreatesParcelsAndPersistsThemToStorage) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    ParcelRepository repository(storage);

    repository.createParcel(makeWaitingForPickupParcel("P-1001", "alice", "bob"));

    const Parcel* parcel = repository.findParcel("P-1001");
    ASSERT_NE(parcel, nullptr);
    EXPECT_EQ(parcel->description(), "documents");

    const ParcelRepository reloaded(storage);
    const Parcel* reloadedParcel = reloaded.findParcel("P-1001");
    ASSERT_NE(reloadedParcel, nullptr);
    EXPECT_EQ(reloadedParcel->senderUsername(), "alice");
    EXPECT_EQ(reloadedParcel->receiverUsername(), "bob");
    EXPECT_EQ(reloadedParcel->fee().raw_value(),
              Money::from_double(15.0).raw_value());
}

TEST(ParcelRepositoryTest, UpdatesParcelsAndPersistsTheReplacement) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    ParcelRepository repository(storage);
    repository.createParcel(makeWaitingForPickupParcel("P-1001", "alice", "bob"));

    Parcel signedParcel =
        makeSignedParcel("P-1001", "alice", "bob", "2026-05-23 13:00:00");
    repository.updateParcel("P-1001", signedParcel);

    const ParcelRepository reloaded(storage);
    const Parcel* parcel = reloaded.findParcel("P-1001");
    ASSERT_NE(parcel, nullptr);
    EXPECT_EQ(parcel->status(), ParcelStatus::Signed);
    EXPECT_EQ(parcel->receivedAt(), "2026-05-23 13:00:00");
}

TEST(ParcelRepositoryTest, ListsParcelsByParticipantsAndWaitingReceiver) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    ParcelRepository repository(storage);
    repository.createParcel(makeWaitingForSignParcel("P-1001", "alice", "bob"));
    repository.createParcel(makeWaitingForPickupParcel("P-1002", "alice", "carol"));
    repository.createParcel(makeSignedParcel("P-1003", "dave", "bob"));

    const auto sentByAlice = repository.listBySender("alice");
    EXPECT_EQ(sortedParcelIds(sentByAlice),
              (std::vector<std::string>{"P-1001", "P-1002"}));

    const auto receivedByBob = repository.listByReceiver("bob");
    EXPECT_EQ(sortedParcelIds(receivedByBob),
              (std::vector<std::string>{"P-1001", "P-1003"}));

    const auto waitingForBob = repository.listWaitingForReceiver("bob");
    ASSERT_EQ(waitingForBob.size(), 1U);
    EXPECT_EQ(waitingForBob[0].id(), "P-1001");
    EXPECT_EQ(waitingForBob[0].status(), ParcelStatus::WaitingForSign);
}

}  // namespace
