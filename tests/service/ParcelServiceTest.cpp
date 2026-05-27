#include "exf/domain/Parcel.hpp"
#include "exf/domain/ParcelStatus.hpp"
#include "exf/domain/User.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace {

using exf::AdminRepository;
using exf::FileStorage;
using exf::Parcel;
using exf::ParcelQuery;
using exf::ParcelRepository;
using exf::ParcelService;
using exf::ParcelServiceError;
using exf::ParcelStatus;
using exf::UserParcelView;
using exf::User;
using exf::UserRepository;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-parcel-service-test-" +
                 std::to_string(std::chrono::steady_clock::now()
                                    .time_since_epoch()
                                    .count()))) {
        std::filesystem::create_directories(path_);
    }

    ~TempDirectory() {
        std::error_code ignored;
        std::filesystem::remove_all(path_, ignored);
    }

    const std::filesystem::path& path() const {
        return path_;
    }

   private:
    std::filesystem::path path_;
};

User makeUser(std::string username, double balance) {
    return User(std::move(username),
                "Test User",
                "13800000000",
                "secret",
                "Shanghai",
                Money::from_double(balance));
}

Parcel makeWaitingParcel(std::string id,
                         std::string sender,
                         std::string receiver,
                         std::string sentAt,
                         std::string description = "documents") {
    return Parcel::createNew(std::move(id), std::move(sender),
                             std::move(receiver), std::move(description),
                             std::move(sentAt), Money::from_double(15.0));
}

Parcel makeSignedParcel(std::string id,
                        std::string sender,
                        std::string receiver,
                        std::string sentAt,
                        std::string receivedAt,
                        std::string description = "books") {
    return Parcel(std::move(id), std::move(sender), std::move(receiver),
                  std::move(description), std::move(sentAt),
                  std::move(receivedAt), Money::from_double(15.0),
                  ParcelStatus::Signed);
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

TEST(ParcelServiceTest,
     SendParcelChargesSenderCreditsAdminAndPersistsParcel) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 30.0));
    users.createUser(makeUser("bob", 0.0));

    const auto [parcelId, error] =
        service.sendParcel("alice", "bob", "documents");

    EXPECT_EQ(error, ParcelServiceError::Nil);
    EXPECT_FALSE(parcelId.empty());

    const auto expectedBalance = Money::from_double(15.0).raw_value();
    ASSERT_NE(users.findUser("alice"), nullptr);
    EXPECT_EQ(users.findUser("alice")->account().balance().raw_value(),
              expectedBalance);
    EXPECT_EQ(admins.getAdmin().account().balance().raw_value(),
              expectedBalance);

    const auto* parcel = parcels.findParcel(parcelId);
    ASSERT_NE(parcel, nullptr);
    EXPECT_EQ(parcel->senderUsername(), "alice");
    EXPECT_EQ(parcel->receiverUsername(), "bob");
    EXPECT_EQ(parcel->description(), "documents");
    EXPECT_EQ(parcel->status(), ParcelStatus::WaitingForSign);

    const UserRepository reloadedUsers(storage);
    AdminRepository reloadedAdmins(storage);
    const ParcelRepository reloadedParcels(storage);

    ASSERT_NE(reloadedUsers.findUser("alice"), nullptr);
    EXPECT_EQ(reloadedUsers.findUser("alice")->account().balance().raw_value(),
              expectedBalance);
    EXPECT_EQ(reloadedAdmins.getAdmin().account().balance().raw_value(),
              expectedBalance);
    EXPECT_NE(reloadedParcels.findParcel(parcelId), nullptr);
}

TEST(ParcelServiceTest,
     SendParcelRejectsInsufficientBalanceWithoutSideEffects) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 10.0));
    users.createUser(makeUser("bob", 0.0));

    const auto [parcelId, error] =
        service.sendParcel("alice", "bob", "documents");

    EXPECT_EQ(error, ParcelServiceError::InsufficientBalance);
    EXPECT_TRUE(parcelId.empty());

    ASSERT_NE(users.findUser("alice"), nullptr);
    EXPECT_EQ(users.findUser("alice")->account().balance().raw_value(),
              Money::from_double(10.0).raw_value());
    EXPECT_EQ(admins.getAdmin().account().balance().raw_value(),
              Money::from_double(0.0).raw_value());
    EXPECT_TRUE(parcels.listAll().empty());
}

TEST(ParcelServiceTest, SignParcelUpdatesStatusForTheReceiver) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 30.0));
    users.createUser(makeUser("bob", 0.0));
    parcels.createParcel(
        makeWaitingParcel("P-1001", "alice", "bob", "1000"));

    EXPECT_EQ(service.signParcel("bob", "P-1001"), ParcelServiceError::Nil);

    const auto* signedParcel = parcels.findParcel("P-1001");
    ASSERT_NE(signedParcel, nullptr);
    EXPECT_EQ(signedParcel->status(), ParcelStatus::Signed);
    EXPECT_FALSE(signedParcel->receivedAt().empty());

    EXPECT_EQ(service.signParcel("bob", "P-1001"),
              ParcelServiceError::AlreadySigned);

    const ParcelRepository reloadedParcels(storage);
    const auto* reloadedParcel = reloadedParcels.findParcel("P-1001");
    ASSERT_NE(reloadedParcel, nullptr);
    EXPECT_EQ(reloadedParcel->status(), ParcelStatus::Signed);
    EXPECT_EQ(reloadedParcel->receivedAt(), signedParcel->receivedAt());
}

TEST(ParcelServiceTest, SignParcelRejectsWrongReceiverAndMissingParcel) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 30.0));
    users.createUser(makeUser("bob", 0.0));
    users.createUser(makeUser("carol", 0.0));
    parcels.createParcel(
        makeWaitingParcel("P-1001", "alice", "bob", "1000"));

    EXPECT_EQ(service.signParcel("carol", "P-1001"),
              ParcelServiceError::NotReceiver);
    EXPECT_EQ(service.signParcel("bob", "missing"),
              ParcelServiceError::ParcelNotFound);

    const auto* parcel = parcels.findParcel("P-1001");
    ASSERT_NE(parcel, nullptr);
    EXPECT_EQ(parcel->status(), ParcelStatus::WaitingForSign);
    EXPECT_EQ(parcel->receivedAt(), "");
}

TEST(ParcelServiceTest, QueryUserParcelsRespectsViewAndQuery) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 30.0));
    users.createUser(makeUser("bob", 0.0));
    users.createUser(makeUser("carol", 0.0));
    users.createUser(makeUser("dave", 0.0));

    parcels.createParcel(
        makeWaitingParcel("P-1001", "alice", "bob", "1000"));
    parcels.createParcel(
        makeSignedParcel("P-1002", "alice", "carol", "2000", "2100"));
    parcels.createParcel(
        makeWaitingParcel("P-1003", "dave", "alice", "3000"));
    parcels.createParcel(
        makeSignedParcel("P-1004", "bob", "alice", "4000", "4100"));

    ParcelQuery query;
    query.sentFrom = "1500";
    query.sentTo = "3500";

    EXPECT_EQ(sortedParcelIds(service.queryUserParcels("alice",
                                                       UserParcelView::Sent,
                                                       query)),
              (std::vector<std::string>{"P-1002"}));

    query = ParcelQuery{};
    query.status = ParcelStatus::WaitingForSign;
    EXPECT_EQ(sortedParcelIds(service.queryUserParcels(
                  "alice", UserParcelView::WaitingForSign, query)),
              (std::vector<std::string>{"P-1003"}));

    query = ParcelQuery{};
    query.status = ParcelStatus::Signed;
    EXPECT_EQ(sortedParcelIds(service.queryUserParcels("alice",
                                                       UserParcelView::Related,
                                                       query)),
              (std::vector<std::string>{"P-1002", "P-1004"}));
}

TEST(ParcelServiceTest, QueryAdminParcelsFiltersAcrossAllParcels) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    ParcelRepository parcels(storage);
    ParcelService service(users, admins, parcels);

    users.createUser(makeUser("alice", 30.0));
    users.createUser(makeUser("bob", 0.0));
    users.createUser(makeUser("carol", 0.0));

    parcels.createParcel(
        makeWaitingParcel("P-1001", "alice", "bob", "1000"));
    parcels.createParcel(
        makeSignedParcel("P-1002", "alice", "carol", "2000", "2100"));
    parcels.createParcel(
        makeWaitingParcel("P-1003", "bob", "alice", "3000"));
    parcels.createParcel(makeWaitingParcel("P-1004", "carol", "bob",
                                           "2026-05-23 10:00:00"));

    EXPECT_EQ(sortedParcelIds(service.queryAdminParcels(ParcelQuery{})),
              (std::vector<std::string>{"P-1001", "P-1002", "P-1003",
                                        "P-1004"}));

    ParcelQuery query;
    query.receiverUsername = "alice";
    query.sentFrom = "1500";
    query.sentTo = "3500";

    EXPECT_EQ(sortedParcelIds(service.queryAdminParcels(query)),
              (std::vector<std::string>{"P-1003"}));
}

}  // namespace
