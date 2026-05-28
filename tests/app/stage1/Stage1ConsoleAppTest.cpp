#include "exf/app/stage1/Stage1ConsoleApp.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/User.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

using exf::FileStorage;
using exf::Parcel;
using exf::ParcelRecordCodec;
using exf::Stage1ConsoleApp;
using exf::User;
using exf::UserRecordCodec;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-stage1-app-test-" +
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

class ConsoleStreamRedirect {
   public:
    explicit ConsoleStreamRedirect(std::string input)
        : input_(std::move(input)),
          originalCin_(std::cin.rdbuf(input_.rdbuf())),
          originalCout_(std::cout.rdbuf(output_.rdbuf())) {}

    ~ConsoleStreamRedirect() {
        std::cin.rdbuf(originalCin_);
        std::cout.rdbuf(originalCout_);
    }

    std::string output() const {
        return output_.str();
    }

   private:
    std::istringstream input_;
    std::ostringstream output_;
    std::streambuf* originalCin_;
    std::streambuf* originalCout_;
};

User makeUser(std::string username,
              std::string name,
              std::string phone,
              std::string address) {
    return User(std::move(username),
                std::move(name),
                std::move(phone),
                "secret",
                std::move(address),
                Money::from_double(30.0));
}

void writeUsers(const FileStorage& storage) {
    storage.writeLines("users.txt",
                       {
                           UserRecordCodec::encode(makeUser(
                               "alice", "Alice", "13800000000", "Shanghai")),
                           UserRecordCodec::encode(makeUser(
                               "bob", "Bob", "13900000000", "Beijing")),
                           UserRecordCodec::encode(makeUser(
                               "carol", "Carol", "13700000000", "Shenzhen")),
                       });
}

TEST(Stage1ConsoleAppTest, UserParcelQueryFiltersBySenderFromConsoleInput) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    writeUsers(storage);
    storage.writeLines(
        "parcels.txt",
        {
            ParcelRecordCodec::encode(Parcel::createNew(
                "P-ALICE-TO-BOB", "alice", "bob", "documents",
                "2026-05-23 10:00:00", Money::from_double(15.0))),
            ParcelRecordCodec::encode(Parcel::createNew(
                "P-BOB-TO-ALICE", "bob", "alice", "books",
                "2026-05-23 11:00:00", Money::from_double(15.0))),
        });

    ConsoleStreamRedirect console(
        "1\n"
        "2\n"
        "alice\n"
        "secret\n"
        "3\n"
        "3\n"
        "3\n"
        "\n"
        "bob\n"
        "\n"
        "3\n"
        "\n"
        "\n"
        "5\n");
    Stage1ConsoleApp app(tempDir.path());

    EXPECT_EQ(app.run(), 0);

    const auto output = console.output();
    EXPECT_NE(output.find("P-BOB-TO-ALICE"), std::string::npos);
    EXPECT_EQ(output.find("P-ALICE-TO-BOB"), std::string::npos);
}

TEST(Stage1ConsoleAppTest, AdminCanListAllUsersFromConsoleMenu) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    writeUsers(storage);

    ConsoleStreamRedirect console(
        "1\n"
        "3\n"
        "admin\n"
        "admin\n"
        "4\n"
        "1\n"
        "\n"
        "\n"
        "\n"
        "3\n"
        "\n"
        "\n"
        "5\n");
    Stage1ConsoleApp app(tempDir.path());

    EXPECT_EQ(app.run(), 0);

    const auto output = console.output();
    EXPECT_NE(output.find("alice | Alice | 13800000000"), std::string::npos);
    EXPECT_NE(output.find("bob | Bob | 13900000000"), std::string::npos);
    EXPECT_NE(output.find("carol | Carol | 13700000000"), std::string::npos);
}

}  // namespace
