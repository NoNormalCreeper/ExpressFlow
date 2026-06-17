#include "exf/app/stage3/Stage3RequestDispatcher.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>

#include "exf/domain/Courier.hpp"
#include "exf/domain/User.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/service/AdminService.hpp"
#include "exf/service/CourierService.hpp"
#include "exf/service/ParcelService.hpp"
#include "exf/service/UserService.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"
#include "exf/util/Money.hpp"

namespace {

class TempDirectory {
   public:
    TempDirectory()
        : path_(std::filesystem::temp_directory_path() /
                ("expressflow-stage3-dispatcher-test-" +
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

class DispatcherHarness {
   public:
    DispatcherHarness()
        : storage_(tempDir_.path()),
          users_(storage_),
          admins_(storage_),
          couriers_(storage_),
          parcels_(storage_),
          userService_(users_),
          adminService_(users_, admins_),
          courierService_(couriers_),
          parcelService_(users_, admins_, parcels_, couriers_),
          dispatcher_(userService_,
                      adminService_,
                      courierService_,
                      parcelService_,
                      sessions_) {
        users_.createUser(exf::User("u1",
                                    "User One",
                                    "13800000001",
                                    "123456",
                                    "A",
                                    exf::util::Money::from_double(100.0)));
        users_.createUser(exf::User("u2",
                                    "User Two",
                                    "13800000002",
                                    "123456",
                                    "B",
                                    exf::util::Money::from_double(0.0)));
        couriers_.createCourier(exf::Courier(
            "c1", "Courier One", "13900000001", "123456",
            exf::util::Money::from_double(0.0)));
    }

    exf::Stage3RequestDispatcher& dispatcher() {
        return dispatcher_;
    }

   private:
    TempDirectory tempDir_;
    exf::FileStorage storage_;
    exf::UserRepository users_;
    exf::AdminRepository admins_;
    exf::CourierRepository couriers_;
    exf::ParcelRepository parcels_;
    exf::UserService userService_;
    exf::AdminService adminService_;
    exf::CourierService courierService_;
    exf::ParcelService parcelService_;
    exf::Stage3SessionManager sessions_;
    exf::Stage3RequestDispatcher dispatcher_;
};

std::string login(exf::Stage3RequestDispatcher& dispatcher,
                  const std::string& role,
                  const std::string& username,
                  const std::string& password = "123456") {
    const auto response =
        dispatcher.handleRequest(
            exf::Stage3Request("LOGIN", {role, username, password}));
    EXPECT_TRUE(response.isOk());
    EXPECT_GE(response.fields().size(), 1);
    return response.fields()[0];
}

TEST(Stage3RequestDispatcherTest, RejectsBadLogin) {
    DispatcherHarness harness;

    const auto response = harness.dispatcher().handleRequest(
        exf::Stage3Request("LOGIN", {"user", "u1", "bad-password"}));

    EXPECT_FALSE(response.isOk());
    EXPECT_EQ(response.code(), "IncorrectPassword");
}

TEST(Stage3RequestDispatcherTest, CompletesNetworkParcelWorkflow) {
    DispatcherHarness harness;
    auto& dispatcher = harness.dispatcher();

    const auto senderToken = login(dispatcher, "user", "u1");
    const auto sendResponse = dispatcher.handleRequest(exf::Stage3Request(
        "SEND_PARCEL", {senderToken, "u2", "book", "2", "books"}));
    ASSERT_TRUE(sendResponse.isOk());
    ASSERT_EQ(sendResponse.fields().size(), 1);
    const auto parcelId = sendResponse.fields()[0];

    const auto adminToken = login(dispatcher, "admin", "admin", "admin");
    const auto unassignedResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_UNASSIGNED", {adminToken}));
    ASSERT_TRUE(unassignedResponse.isOk());
    ASSERT_EQ(unassignedResponse.fields().size(), 1);
    EXPECT_EQ(exf::ParcelRecordCodec::decode(unassignedResponse.fields()[0]).id(),
              parcelId);

    const auto assignResponse = dispatcher.handleRequest(exf::Stage3Request(
        "ASSIGN_COURIER", {adminToken, parcelId, "c1"}));
    EXPECT_TRUE(assignResponse.isOk());

    const auto courierToken = login(dispatcher, "courier", "c1");
    const auto tasksResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_PICKUP_TASKS", {courierToken}));
    ASSERT_TRUE(tasksResponse.isOk());
    ASSERT_EQ(tasksResponse.fields().size(), 1);

    const auto pickupResponse = dispatcher.handleRequest(
        exf::Stage3Request("PICKUP", {courierToken, parcelId}));
    EXPECT_TRUE(pickupResponse.isOk());

    const auto receiverToken = login(dispatcher, "user", "u2");
    const auto waitingResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_WAITING_SIGN", {receiverToken}));
    ASSERT_TRUE(waitingResponse.isOk());
    ASSERT_EQ(waitingResponse.fields().size(), 1);

    const auto signResponse = dispatcher.handleRequest(
        exf::Stage3Request("SIGN", {receiverToken, parcelId}));
    EXPECT_TRUE(signResponse.isOk());
}

TEST(Stage3RequestDispatcherTest, EnforcesRolePermissions) {
    DispatcherHarness harness;
    const auto userToken = login(harness.dispatcher(), "user", "u1");

    const auto response = harness.dispatcher().handleRequest(
        exf::Stage3Request("LIST_UNASSIGNED", {userToken}));

    EXPECT_FALSE(response.isOk());
    EXPECT_EQ(response.code(), "Unauthorized");
}

TEST(Stage3RequestDispatcherTest, SupportsStage2StyleAdminQueries) {
    DispatcherHarness harness;
    auto& dispatcher = harness.dispatcher();
    const auto adminToken = login(dispatcher, "admin", "admin", "admin");

    const auto usersResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_USERS", {adminToken}));
    ASSERT_TRUE(usersResponse.isOk());
    EXPECT_EQ(usersResponse.fields().size(), 2);

    const auto addCourierResponse =
        dispatcher.handleRequest(exf::Stage3Request(
            "ADD_COURIER",
            {adminToken, "c2", "Courier Two", "13900000002", "123456"}));
    EXPECT_TRUE(addCourierResponse.isOk());

    const auto couriersResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_COURIERS", {adminToken}));
    ASSERT_TRUE(couriersResponse.isOk());
    EXPECT_EQ(couriersResponse.fields().size(), 2);

    const auto senderToken = login(dispatcher, "user", "u1");
    const auto sendResponse = dispatcher.handleRequest(exf::Stage3Request(
        "SEND_PARCEL", {senderToken, "u2", "standard", "1", "box"}));
    ASSERT_TRUE(sendResponse.isOk());

    const auto parcelsResponse = dispatcher.handleRequest(
        exf::Stage3Request("LIST_ALL_PARCELS", {adminToken}));
    ASSERT_TRUE(parcelsResponse.isOk());
    EXPECT_EQ(parcelsResponse.fields().size(), 1);

    const auto deleteCourierResponse = dispatcher.handleRequest(
        exf::Stage3Request("DELETE_COURIER", {adminToken, "c2"}));
    EXPECT_TRUE(deleteCourierResponse.isOk());
}

}  // namespace
