#include "exf/domain/ParcelStatus.hpp"
#include "exf/domain/User.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

namespace {

using exf::AdminRepository;
using exf::FileStorage;
using exf::ParcelRepository;
using exf::ParcelService;
using exf::ParcelServiceError;
using exf::ParcelStatus;
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

}  // namespace
