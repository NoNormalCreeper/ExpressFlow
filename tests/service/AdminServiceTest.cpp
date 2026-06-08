#include "exf/repository/UserRepository.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

namespace {

using exf::AdminService;
using exf::AdminRepository;
using exf::FileStorage;
using exf::User;
using exf::UserRepository;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-admin-service-test-" +
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

User makeUser(std::string username, std::string name) {
    return User(std::move(username),
                std::move(name),
                "13800000000",
                "secret",
                "Shanghai",
                Money::from_double(10.0));
}

TEST(AdminServiceTest, ListsAllRegisteredUsers) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    users.createUser(makeUser("alice", "Alice"));
    users.createUser(makeUser("bob", "Bob"));
    AdminService service(users);

    const auto listedUsers = service.listUsers();

    ASSERT_EQ(listedUsers.size(), 2U);
    EXPECT_EQ(listedUsers[0].username(), "alice");
    EXPECT_EQ(listedUsers[1].username(), "bob");
}

TEST(AdminServiceTest, GetsAdminAccountBalance) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    UserRepository users(storage);
    AdminRepository admins(storage);
    admins.modifyAdmin(
        [](exf::Admin& admin) { admin.account().credit(Money::from_double(42.0)); });
    AdminService service(users, admins);

    EXPECT_EQ(service.getBalance(), Money::from_double(42.0));
}

}  // namespace
