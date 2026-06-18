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

    /** 返回当前是否已登录。 */
    bool isLoggedIn() const;

    /** 执行登录流程。 */
    void login();

    /** 执行退出登录流程。 */
    void logout();

    /** 运行用户菜单。 */
    void runUserMenu();

    /** 运行管理员菜单。 */
    void runAdminMenu();

    /** 运行快递员菜单。 */
    void runCourierMenu();

    /** 执行用户寄件流程。 */
    void sendParcel();

    /** 显示待签收快递。 */
    void listWaitingSign();

    /** 执行签收流程。 */
    void signParcel();

    /** 显示当前用户相关快递。 */
    void listMyParcels();

    /** 显示用户列表。 */
    void listUsers();

    /** 显示未分配快递。 */
    void listUnassigned();

    /** 显示快递员列表。 */
    void listCouriers();

    /** 执行新增快递员流程。 */
    void addCourier();

    /** 执行删除快递员流程。 */
    void deleteCourier();

    /** 执行快递员分配流程。 */
    void assignCourier();

    /** 显示全部快递。 */
    void listAllParcels();

    /** 按管理员选择的快递员显示快递。 */
    void listCourierParcelsAsAdmin();

    /** 显示当前快递员待揽收任务。 */
    void listPickupTasks();

    /** 执行快递员揽收流程。 */
    void pickupParcel();

    /** 显示当前快递员相关快递。 */
    void listCourierParcels();

    /** 打印最近一次服务错误。 */
    bool printServiceError() const;

    /** 提示用户选择登录角色。 */
    static Stage3Role promptLoginRole();

    /** 提示用户选择物品类型。 */
    static std::string promptItemType();

    /** 提示用户输入正数数量。 */
    static std::string promptPositiveAmount();

    /** 打印用户列表。 */
    static void printUsers(const std::vector<User>& users);

    /** 打印快递列表。 */
    static void printParcels(const std::vector<Parcel>& parcels);

    /** 打印快递员列表。 */
    static void printCouriers(const std::vector<Courier>& couriers);
};

}  // namespace exf
