#include "exf/repository/UserRepository.hpp"
#include "exf/service/UserService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {

using exf::FileStorage;
using exf::UserRepository;
using exf::UserAccountError;
using exf::UserService;
using exf::UserServiceError;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-user-service-test-" +
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

TEST(UserServiceTest, RegistersUsersAndRejectsDuplicateUsernames) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);

    EXPECT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai",
                                   25.0),
              UserServiceError::Nil);
    EXPECT_EQ(service.registerUser("alice",
                                   "Alice 2",
                                   "13900000000",
                                   "secret2",
                                   "Beijing",
                                   50.0),
              UserServiceError::UserAlreadyExists);

    const UserRepository reloaded(storage);
    const auto* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->name(), "Alice");
    EXPECT_EQ(alice->password(), "secret");
}

TEST(UserServiceTest, RejectsNegativeInitialBalance) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);

    EXPECT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai",
                                   -0.01),
              UserServiceError::InvalidInitialBalance);
    EXPECT_EQ(repository.findUser("alice"), nullptr);
}

TEST(UserServiceTest, LogsInOnlyExistingUsersWithCorrectPassword) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);
    ASSERT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai"),
              UserServiceError::Nil);

    EXPECT_EQ(service.loginUser("alice", "secret"), UserServiceError::Nil);
    EXPECT_EQ(service.loginUser("alice", "wrong"),
              UserServiceError::IncorrectPassword);
    EXPECT_EQ(service.loginUser("missing", "secret"),
              UserServiceError::UserNotFound);
}

TEST(UserServiceTest, UpdatesPasswordAfterVerifyingOldPassword) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);
    ASSERT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "old",
                                   "Shanghai"),
              UserServiceError::Nil);

    EXPECT_EQ(service.updatePassword("alice", "wrong", "new"),
              UserServiceError::IncorrectPassword);
    EXPECT_EQ(service.loginUser("alice", "old"), UserServiceError::Nil);

    EXPECT_EQ(service.updatePassword("alice", "old", "new"),
              UserServiceError::Nil);
    EXPECT_EQ(service.loginUser("alice", "old"),
              UserServiceError::IncorrectPassword);
    EXPECT_EQ(service.loginUser("alice", "new"), UserServiceError::Nil);

    const UserRepository reloaded(storage);
    const auto* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->password(), "new");
}

TEST(UserServiceTest, ReportsMissingUserWhenUpdatingPassword) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);

    EXPECT_EQ(service.updatePassword("missing", "old", "new"),
              UserServiceError::UserNotFound);
}

TEST(UserServiceTest, RetrievesAndPersistsTopUpBalance) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);
    ASSERT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai",
                                   25.0),
              UserServiceError::Nil);

    EXPECT_EQ(service.getBalance("alice").raw_value(),
              Money::from_double(25.0).raw_value());

    EXPECT_EQ(service.topUpBalance("alice", Money::from_double(10.50)),
              UserAccountError::Nil);
    EXPECT_EQ(service.getBalance("alice").raw_value(),
              Money::from_double(35.50).raw_value());

    const UserRepository reloaded(storage);
    const auto* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->account().balance().raw_value(),
              Money::from_double(35.50).raw_value());
}

TEST(UserServiceTest, PaysFromBalanceAndLeavesBalanceUnchangedWhenInsufficient) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);
    ASSERT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai",
                                   25.0),
              UserServiceError::Nil);

    EXPECT_EQ(service.payFromBalance("alice", Money::from_double(7.25)),
              UserAccountError::Nil);
    EXPECT_EQ(service.getBalance("alice").raw_value(),
              Money::from_double(17.75).raw_value());

    EXPECT_EQ(service.payFromBalance("alice", Money::from_double(18.0)),
              UserAccountError::InsufficientBalance);
    EXPECT_EQ(service.getBalance("alice").raw_value(),
              Money::from_double(17.75).raw_value());

    const UserRepository reloaded(storage);
    const auto* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->account().balance().raw_value(),
              Money::from_double(17.75).raw_value());
}

TEST(UserServiceTest, ReportsMissingUserWhenUpdatingBalance) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);

    EXPECT_EQ(service.topUpBalance("missing", Money::from_double(10.0)),
              UserAccountError::UserNotFound);
    EXPECT_EQ(service.payFromBalance("missing", Money::from_double(10.0)),
              UserAccountError::UserNotFound);
}

TEST(UserServiceTest, ThrowsWhenGettingBalanceForMissingUser) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);

    EXPECT_THROW(service.getBalance("missing"), std::runtime_error);
}

TEST(UserServiceTest, RejectsNegativeBalanceUpdatesWithoutChangingBalance) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    UserService service(repository);
    ASSERT_EQ(service.registerUser("alice",
                                   "Alice",
                                   "13800000000",
                                   "secret",
                                   "Shanghai",
                                   25.0),
              UserServiceError::Nil);

    EXPECT_EQ(service.topUpBalance("alice", Money::from_double(-10.0)),
              UserAccountError::InvalidAmount);
    EXPECT_EQ(service.payFromBalance("alice", Money::from_double(-5.0)),
              UserAccountError::InvalidAmount);
    EXPECT_EQ(service.getBalance("alice").raw_value(),
              Money::from_double(25.0).raw_value());

    const UserRepository reloaded(storage);
    const auto* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->account().balance().raw_value(),
              Money::from_double(25.0).raw_value());
}

}  // namespace
