#include "exf/domain/Courier.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/service/CourierService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/util/Money.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

namespace {

using exf::CourierRepository;
using exf::CourierService;
using exf::CourierServiceError;
using exf::FileStorage;
using exf::util::Money;

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-courier-service-test-" +
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

TEST(CourierServiceTest, AddsCouriersAndRejectsDuplicateUsernames) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository couriers(storage);
    CourierService service(couriers);

    EXPECT_EQ(service.addCourier("c01", "Li Ming", "13800000000", "secret",
                                 5.0),
              CourierServiceError::Nil);
    EXPECT_EQ(service.addCourier("c01", "Other", "13800000001", "secret",
                                 0.0),
              CourierServiceError::CourierAlreadyExists);

    const auto list = service.listCouriers();
    ASSERT_EQ(list.size(), 1U);
    EXPECT_EQ(list[0].username(), "c01");
    EXPECT_EQ(list[0].account().balance().raw_value(),
              Money::from_double(5.0).raw_value());
}

TEST(CourierServiceTest, LogsInOnlyExistingCouriersWithCorrectPassword) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository couriers(storage);
    CourierService service(couriers);
    ASSERT_EQ(service.addCourier("c01", "Li Ming", "13800000000", "secret"),
              CourierServiceError::Nil);

    EXPECT_EQ(service.loginCourier("c01", "secret"),
              CourierServiceError::Nil);
    EXPECT_EQ(service.loginCourier("c01", "wrong"),
              CourierServiceError::IncorrectPassword);
    EXPECT_EQ(service.loginCourier("missing", "secret"),
              CourierServiceError::CourierNotFound);
}

TEST(CourierServiceTest, DeletesCouriersAndReportsMissingCourier) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository couriers(storage);
    CourierService service(couriers);
    ASSERT_EQ(service.addCourier("c01", "Li Ming", "13800000000", "secret"),
              CourierServiceError::Nil);

    EXPECT_EQ(service.deleteCourier("missing"),
              CourierServiceError::CourierNotFound);
    EXPECT_EQ(service.deleteCourier("c01"), CourierServiceError::Nil);
    EXPECT_TRUE(service.listCouriers().empty());
}

TEST(CourierServiceTest, UpdatesPasswordAfterVerifyingOldPassword) {
    const TempDirectory tempDir;
    const FileStorage storage(tempDir.path());
    CourierRepository couriers(storage);
    CourierService service(couriers);
    ASSERT_EQ(service.addCourier("c01", "Li Ming", "13800000000", "secret"),
              CourierServiceError::Nil);

    EXPECT_EQ(service.updatePassword("c01", "wrong", "new-secret"),
              CourierServiceError::IncorrectPassword);
    EXPECT_EQ(service.updatePassword("c01", "secret", "new-secret"),
              CourierServiceError::Nil);
    EXPECT_EQ(service.loginCourier("c01", "new-secret"),
              CourierServiceError::Nil);
}

}  // namespace
