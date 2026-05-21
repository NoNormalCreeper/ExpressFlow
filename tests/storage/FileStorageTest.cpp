#include "exf/storage/FileStorage.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

namespace {

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-file-storage-test-" +
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

TEST(FileStorageTest, WritesAndReadsLinesFromConfiguredBasePath) {
    const TempDirectory tempDir;
    const exf::FileStorage storage(tempDir.path());
    const std::vector<std::string> lines = {
        "first",
        "",
        "third|with|delimiters",
        R"(fourth\with\backslashes)",
    };

    storage.writeLines("users.txt", lines);

    EXPECT_EQ(storage.basePath(), tempDir.path());
    EXPECT_EQ(storage.readLines("users.txt"), lines);
    EXPECT_TRUE(std::filesystem::exists(tempDir.path() / "users.txt"));
}

}  // namespace
