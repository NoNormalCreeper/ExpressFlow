#include "exf/app/stage3/Stage3ClientApp.hpp"

#include <exception>
#include <iostream>
#include <utility>

#include "exf/app/console/ConsoleDisplay.hpp"
#include "exf/app/console/ConsoleInput.hpp"
#include "exf/app/console/ConsoleMenu.hpp"
#include "exf/app/console/ConsoleSelection.hpp"

namespace exf {

// 创建客户端应用并初始化客户端服务。
Stage3ClientApp::Stage3ClientApp(std::string host, uint16_t port)
    : host_(std::move(host)), port_(port), service_(host_, port_) {}

// 运行客户端主循环。
int Stage3ClientApp::run() {
    if (!service_.connect()) {
        std::cerr << service_.lastError() << '\n';
        return 1;
    }

    std::cout << "ExpressFlow Stage 3 client connected to " << host_ << ':'
              << port_ << '\n';
    while (true) {
        if (!service_.isConnected()) {
            return 1;
        }

        if (!isLoggedIn()) {
            const int choice =
                ConsoleMenu::show("主菜单", {"登录", "退出程序"});
            switch (choice) {
                case 1:
                    login();
                    break;
                default:
                    return 0;
            }
            continue;
        }

        const auto role = service_.currentRole();
        if (!role.has_value()) {
            logout();
            continue;
        }

        switch (*role) {
            case Stage3Role::User:
                runUserMenu();
                break;
            case Stage3Role::Admin:
                runAdminMenu();
                break;
            case Stage3Role::Courier:
                runCourierMenu();
                break;
        }
    }
}

// 返回当前是否已登录。
bool Stage3ClientApp::isLoggedIn() const {
    return service_.isLoggedIn();
}

// 执行登录交互流程。
void Stage3ClientApp::login() {
    const Stage3Role role = promptLoginRole();
    const std::string username = ConsoleInput::promptNonEmpty("请输入用户名");
    const std::string password = ConsoleInput::promptNonEmpty("请输入密码");
    auto response = service_.login(role, username, password);
    if (!response.has_value()) {
        std::cout << service_.lastError() << '\n';
        return;
    }
    std::cout << response->message() << '\n';
}

// 执行退出登录流程。
void Stage3ClientApp::logout() {
    if (!isLoggedIn()) {
        return;
    }
    auto response = service_.logout();
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    }
}

// 显示并处理用户菜单。
void Stage3ClientApp::runUserMenu() {
    const int choice = ConsoleMenu::show(
        "用户菜单 (" + service_.username() + ")",
        {"发送快递", "查看待签收快递", "签收快递", "查询我的快递", "退出登录"});
    switch (choice) {
        case 1:
            sendParcel();
            return;
        case 2:
            listWaitingSign();
            return;
        case 3:
            signParcel();
            return;
        case 4:
            listMyParcels();
            return;
        default:
            logout();
            return;
    }
}

// 显示并处理管理员菜单。
void Stage3ClientApp::runAdminMenu() {
    const int choice =
        ConsoleMenu::show("管理员菜单 (" + service_.username() + ")",
                          {"查看所有用户", "查看所有快递员", "添加快递员",
                           "删除快递员", "查看未分配快递", "分配快递员",
                           "查询全部快递", "查询快递员任务", "退出登录"});
    switch (choice) {
        case 1:
            listUsers();
            return;
        case 2:
            listCouriers();
            return;
        case 3:
            addCourier();
            return;
        case 4:
            deleteCourier();
            return;
        case 5:
            listUnassigned();
            return;
        case 6:
            assignCourier();
            return;
        case 7:
            listAllParcels();
            return;
        case 8:
            listCourierParcelsAsAdmin();
            return;
        default:
            logout();
            return;
    }
}

// 显示并处理快递员菜单。
void Stage3ClientApp::runCourierMenu() {
    const int choice = ConsoleMenu::show(
        "快递员菜单 (" + service_.username() + ")",
        {"查看待揽收任务", "揽收快递", "查询我的快递任务", "退出登录"});
    switch (choice) {
        case 1:
            listPickupTasks();
            return;
        case 2:
            pickupParcel();
            return;
        case 3:
            listCourierParcels();
            return;
        default:
            logout();
            return;
    }
}

// 执行用户寄件交互流程。
void Stage3ClientApp::sendParcel() {
    const std::string receiver =
        ConsoleInput::promptNonEmpty("请输入收件人用户名");
    const std::string description =
        ConsoleInput::promptNonEmpty("请输入快递描述");
    const std::string itemType = promptItemType();
    const std::string amount = promptPositiveAmount();
    auto response =
        service_.sendParcel(receiver, itemType, amount, description);
    if (!response.has_value()) {
        printServiceError();
        return;
    }
    std::cout << response->message();
    if (response->isOk() && !response->fields().empty()) {
        std::cout << "，快递单号: " << response->fields()[0];
    }
    std::cout << '\n';
}

// 查询并显示待签收快递。
void Stage3ClientApp::listWaitingSign() {
    const auto parcels = service_.listWaitingSign();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 选择并签收待签收快递。
void Stage3ClientApp::signParcel() {
    const auto parcels = service_.listWaitingSign();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
    if (parcels.empty()) {
        return;
    }
    const auto index = ConsoleSelection::promptSingleIndex(
        "请选择要签收的快递序号", parcels.size());
    if (!index.has_value()) {
        return;
    }
    auto response = service_.signParcel(parcels[*index].id());
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    } else {
        printServiceError();
    }
}

// 查询并显示当前用户相关快递。
void Stage3ClientApp::listMyParcels() {
    const auto parcels = service_.listMyParcels();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 查询并显示所有用户。
void Stage3ClientApp::listUsers() {
    const auto users = service_.listUsers();
    if (printServiceError()) {
        return;
    }
    printUsers(users);
}

// 查询并显示未分配快递。
void Stage3ClientApp::listUnassigned() {
    const auto parcels = service_.listUnassigned();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 查询并显示快递员列表。
void Stage3ClientApp::listCouriers() {
    const auto couriers = service_.listCouriers();
    if (printServiceError()) {
        return;
    }
    printCouriers(couriers);
}

// 执行新增快递员交互流程。
void Stage3ClientApp::addCourier() {
    const std::string username =
        ConsoleInput::promptNonEmpty("请输入快递员用户名");
    const std::string name = ConsoleInput::promptNonEmpty("请输入快递员姓名");
    const std::string phone =
        ConsoleInput::promptNonEmpty("请输入快递员手机号");
    const std::string password = ConsoleInput::promptNonEmpty("请输入初始密码");
    auto response = service_.addCourier(username, name, phone, password);
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    } else {
        printServiceError();
    }
}

// 选择并删除快递员。
void Stage3ClientApp::deleteCourier() {
    const auto couriers = service_.listCouriers();
    if (printServiceError()) {
        return;
    }
    printCouriers(couriers);
    if (couriers.empty()) {
        return;
    }
    const auto index = ConsoleSelection::promptSingleIndex(
        "请选择要删除的快递员序号", couriers.size());
    if (!index.has_value()) {
        return;
    }
    auto response = service_.deleteCourier(couriers[*index].username());
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    } else {
        printServiceError();
    }
}

// 选择快递和快递员并执行分配。
void Stage3ClientApp::assignCourier() {
    const auto parcels = service_.listUnassigned();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
    if (parcels.empty()) {
        return;
    }
    const auto couriers = service_.listCouriers();
    if (printServiceError()) {
        return;
    }
    printCouriers(couriers);
    if (couriers.empty()) {
        return;
    }
    const auto parcelIndex = ConsoleSelection::promptSingleIndex(
        "请选择要分配的快递序号", parcels.size());
    if (!parcelIndex.has_value()) {
        return;
    }
    const auto courierIndex = ConsoleSelection::promptSingleIndex(
        "请选择快递员序号", couriers.size());
    if (!courierIndex.has_value()) {
        return;
    }
    auto response = service_.assignCourier(parcels[*parcelIndex].id(),
                                           couriers[*courierIndex].username());
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    } else {
        printServiceError();
    }
}

// 查询并显示全部快递。
void Stage3ClientApp::listAllParcels() {
    const auto parcels = service_.listAllParcels();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 以管理员身份选择快递员并查看其任务。
void Stage3ClientApp::listCourierParcelsAsAdmin() {
    const auto couriers = service_.listCouriers();
    if (printServiceError()) {
        return;
    }
    printCouriers(couriers);
    if (couriers.empty()) {
        return;
    }
    const auto index = ConsoleSelection::promptSingleIndex("请选择快递员序号",
                                                           couriers.size());
    if (!index.has_value()) {
        return;
    }
    const auto parcels =
        service_.listCourierParcels(couriers[*index].username());
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 查询并显示当前快递员待揽收任务。
void Stage3ClientApp::listPickupTasks() {
    const auto parcels = service_.listPickupTasks();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 选择并揽收快递。
void Stage3ClientApp::pickupParcel() {
    const auto parcels = service_.listPickupTasks();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
    if (parcels.empty()) {
        return;
    }
    const auto index = ConsoleSelection::promptSingleIndex(
        "请选择要揽收的快递序号", parcels.size());
    if (!index.has_value()) {
        return;
    }
    auto response = service_.pickupParcel(parcels[*index].id());
    if (response.has_value()) {
        std::cout << response->message() << '\n';
    } else {
        printServiceError();
    }
}

// 查询并显示当前快递员相关快递。
void Stage3ClientApp::listCourierParcels() {
    const auto parcels = service_.listCourierParcels();
    if (printServiceError()) {
        return;
    }
    printParcels(parcels);
}

// 打印服务错误并返回是否存在错误。
bool Stage3ClientApp::printServiceError() const {
    if (service_.lastError().empty()) {
        return false;
    }
    std::cout << service_.lastError() << '\n';
    return true;
}

// 提示用户选择登录身份。
Stage3Role Stage3ClientApp::promptLoginRole() {
    const int roleChoice = ConsoleMenu::show(
        "登录身份", {"用户", "快递员", "管理员"}, "", "请选择登录身份");
    switch (roleChoice) {
        case 2:
            return Stage3Role::Courier;
        case 3:
            return Stage3Role::Admin;
        default:
            return Stage3Role::User;
    }
}

// 提示用户选择快递物品类型。
std::string Stage3ClientApp::promptItemType() {
    const int choice = ConsoleMenu::show(
        "快递类型",
        {"普通快递（5元/kg）", "易碎品（8元/kg）", "图书（2元/本）"});
    switch (choice) {
        case 1:
            return "standard";
        case 2:
            return "fragile";
        default:
            return "book";
    }
}

// 提示用户输入大于零的计费数量。
std::string Stage3ClientApp::promptPositiveAmount() {
    while (true) {
        const std::string input =
            ConsoleInput::promptNonEmpty("请输入重量 kg 或图书本数");
        try {
            if (std::stod(input) > 0.0) {
                return input;
            }
        } catch (const std::exception&) {
        }
        std::cout << "请输入大于 0 的数字。\n";
    }
}

// 打印用户列表。
void Stage3ClientApp::printUsers(const std::vector<User>& users) {
    ConsoleDisplay::printUsers(users);
}

// 打印快递列表。
void Stage3ClientApp::printParcels(const std::vector<Parcel>& parcels) {
    ConsoleDisplay::printParcels(parcels);
}

// 打印快递员列表。
void Stage3ClientApp::printCouriers(const std::vector<Courier>& couriers) {
    ConsoleDisplay::printCouriers(couriers);
}

}  // namespace exf
