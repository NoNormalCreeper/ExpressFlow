#include "exf/domain/Courier.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

namespace {

using exf::Courier;
using exf::CourierRecordCodec;
using exf::CourierRepository;
using exf::FileStorage;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-courier-repository-test-" +
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

Courier makeCourier(std::string username, std::string password = "secret") {
    return Courier(std::move(username),
                   "Li Ming",
                   "13800000000",
                   std::move(password),
                   Money::from_double(25.0));
}

TEST(CourierRepositoryTest, LoadsExistingCouriersFromStorageOnConstruction) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    storage.writeLines("couriers.txt",
                       {
                           CourierRecordCodec::encode(makeCourier("c01")),
                           CourierRecordCodec::encode(makeCourier("c02")),
                       });

    CourierRepository repository(storage);

    const Courier* courier = repository.findCourier("c01");
    ASSERT_NE(courier, nullptr);
    EXPECT_EQ(courier->username(), "c01");
    EXPECT_EQ(courier->password(), "secret");
    EXPECT_TRUE(repository.courierExists("c02"));
    EXPECT_FALSE(repository.courierExists("missing"));
}

TEST(CourierRepositoryTest, CreatesCouriersAndPersistsThemToStorage) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository repository(storage);

    repository.createCourier(makeCourier("c01", "first-password"));

    const Courier* courier = repository.findCourier("c01");
    ASSERT_NE(courier, nullptr);
    EXPECT_EQ(courier->password(), "first-password");

    const CourierRepository reloaded(storage);
    const Courier* reloadedCourier = reloaded.findCourier("c01");
    ASSERT_NE(reloadedCourier, nullptr);
    EXPECT_EQ(reloadedCourier->password(), "first-password");
}

TEST(CourierRepositoryTest, UpdatesCouriersAndPersistsTheReplacement) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository repository(storage);
    repository.createCourier(makeCourier("c01", "old-password"));

    repository.updateCourier("c01", makeCourier("c01", "new-password"));

    const CourierRepository reloaded(storage);
    const Courier* courier = reloaded.findCourier("c01");
    ASSERT_NE(courier, nullptr);
    EXPECT_EQ(courier->password(), "new-password");
}

TEST(CourierRepositoryTest, DeletesCouriersAndPersistsRemoval) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository repository(storage);
    repository.createCourier(makeCourier("c01"));
    repository.createCourier(makeCourier("c02"));

    repository.deleteCourier("c01");

    EXPECT_EQ(repository.findCourier("c01"), nullptr);
    EXPECT_NE(repository.findCourier("c02"), nullptr);

    const CourierRepository reloaded(storage);
    EXPECT_EQ(reloaded.findCourier("c01"), nullptr);
    EXPECT_NE(reloaded.findCourier("c02"), nullptr);
}

}  // namespace
