#include "exf/domain/User.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

namespace {

using exf::FileStorage;
using exf::User;
using exf::UserRecordCodec;
using exf::UserRepository;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-user-repository-test-" +
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

User makeUser(std::string username, std::string password = "secret") {
    return User(std::move(username),
                "Alice",
                "13800000000",
                std::move(password),
                "Shanghai",
                Money::from_double(25.0));
}

TEST(UserRepositoryTest, LoadsExistingUsersFromStorageOnConstruction) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    storage.writeLines("users.txt", {
                                      UserRecordCodec::encode(makeUser("alice")),
                                      UserRecordCodec::encode(makeUser("bob")),
                                  });

    UserRepository repository(storage);

    const User* alice = repository.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->username(), "alice");
    EXPECT_EQ(alice->password(), "secret");
    EXPECT_TRUE(repository.userExists("bob"));
    EXPECT_FALSE(repository.userExists("missing"));
}

TEST(UserRepositoryTest, CreatesUsersAndPersistsThemToStorage) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);

    repository.createUser(makeUser("alice", "first-password"));

    const User* alice = repository.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->password(), "first-password");

    const UserRepository reloaded(storage);
    const User* reloadedAlice = reloaded.findUser("alice");
    ASSERT_NE(reloadedAlice, nullptr);
    EXPECT_EQ(reloadedAlice->password(), "first-password");
}

TEST(UserRepositoryTest, UpdatesUsersAndPersistsTheReplacement) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    repository.createUser(makeUser("alice", "old-password"));

    repository.updateUser("alice", makeUser("alice", "new-password"));

    const UserRepository reloaded(storage);
    const User* alice = reloaded.findUser("alice");
    ASSERT_NE(alice, nullptr);
    EXPECT_EQ(alice->password(), "new-password");
}

TEST(UserRepositoryTest, DeletesUsersAndPersistsRemoval) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository repository(storage);
    repository.createUser(makeUser("alice"));
    repository.createUser(makeUser("bob"));

    repository.deleteUser("alice");

    EXPECT_EQ(repository.findUser("alice"), nullptr);
    EXPECT_NE(repository.findUser("bob"), nullptr);

    const UserRepository reloaded(storage);
    EXPECT_EQ(reloaded.findUser("alice"), nullptr);
    EXPECT_NE(reloaded.findUser("bob"), nullptr);
}

}  // namespace
