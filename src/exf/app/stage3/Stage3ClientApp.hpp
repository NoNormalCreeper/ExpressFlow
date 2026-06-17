#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "exf/app/stage3/Stage3ClientService.hpp"
#include "exf/domain/Courier.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/User.hpp"

namespace exf {

/**
 * Stage3ClientApp 是题目 3 网络版客户端入口。
 */
class Stage3ClientApp {
   public:
    /**
     * 用服务端地址创建客户端。
     */
    Stage3ClientApp(std::string host, uint16_t port);

    /**
     * 启动客户端菜单。
     */
    int run();

   private:
    /** 服务端地址。 */
    std::string host_;

    /** 服务端端口。 */
    uint16_t port_;

    /** 网络版客户端服务。 */
    Stage3ClientService service_;

    bool isLoggedIn() const;
    void login();
    void logout();
    void runUserMenu();
    void runAdminMenu();
    void runCourierMenu();
    void sendParcel();
    void listWaitingSign();
    void signParcel();
    void listMyParcels();
    void listUsers();
    void listUnassigned();
    void listCouriers();
    void addCourier();
    void deleteCourier();
    void assignCourier();
    void listAllParcels();
    void listCourierParcelsAsAdmin();
    void listPickupTasks();
    void pickupParcel();
    void listCourierParcels();
    bool printServiceError() const;

    static Stage3Role promptLoginRole();
    static std::string promptItemType();
    static std::string promptPositiveAmount();
    static void printUsers(const std::vector<User>& users);
    static void printParcels(const std::vector<Parcel>& parcels);
    static void printCouriers(const std::vector<Courier>& couriers);
};

}  // namespace exf
