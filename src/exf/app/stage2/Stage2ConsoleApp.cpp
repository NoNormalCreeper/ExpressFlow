#include "exf/app/stage2/Stage2ConsoleApp.hpp"

#include <exception>
#include <iostream>
#include <regex>
#include <utility>

#include "exf/app/console/ConsoleDisplay.hpp"
#include "exf/app/console/ConsoleInput.hpp"
#include "exf/app/console/ConsoleSelection.hpp"

namespace exf {

Stage2ConsoleApp::Stage2ConsoleApp(std::filesystem::path dataDir)
    : storage_(std::move(dataDir)),
      userRepository_(storage_),
      adminRepository_(storage_),
      courierRepository_(storage_),
      parcelRepository_(storage_),
      userService_(userRepository_),
      adminService_(userRepository_, adminRepository_),
      courierService_(courierRepository_),
      parcelService_(userRepository_,
                     adminRepository_,
                     parcelRepository_,
                     courierRepository_) {}

void Stage2ConsoleApp::handleUserRegister() {
    std::string username, name, phone, password, address;

    while (true) {
        username = ConsoleInput::promptNonEmpty("请输入用户名");
        if (!userRepository_.userExists(username)) {
            break;
        }
        std::cout << "用户名已存在，请重新输入。\n";
    }

    name = ConsoleInput::promptNonEmpty("请输入姓名");
    phone =
        ConsoleInput::promptRegex("请输入手机号", std::regex(R"(1\d{10})"),
                                  "请输入有效的手机号（11位数字，以1开头）。");
    password = ConsoleInput::promptRegex("请输入密码（至少6位）",
                                         ConsoleInput::passwordRegex,
                                         "密码至少需要6位，请重新输入。");
    address = ConsoleInput::promptNonEmpty("请输入地址");

    const auto err =
        userService_.registerUser(username, name, phone, password, address);
    if (err == UserServiceError::Nil) {
        std::cout << "注册成功！\n";
    } else if (err == UserServiceError::UserAlreadyExists) {
        std::cout << "用户已存在。\n";
    } else {
        std::cout << "注册失败。\n";
    }
}

void Stage2ConsoleApp::handleUserLogin(Stage2Context& ctx) {
    const std::string username = ConsoleInput::promptNonEmpty("请输入用户名");
    const std::string password =
        ConsoleInput::promptRegex("请输入密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    const auto err = userService_.loginUser(username, password);
    if (err == UserServiceError::Nil) {
        ctx.role = Stage2Role::User;
        ctx.username = username;
        std::cout << "用户登录成功！\n";
    } else if (err == UserServiceError::UserNotFound) {
        std::cout << "用户不存在。\n";
    } else {
        std::cout << "密码错误。\n";
    }
}

void Stage2ConsoleApp::handleCourierLogin(Stage2Context& ctx) {
    const std::string username =
        ConsoleInput::promptNonEmpty("请输入快递员用户名");
    const std::string password =
        ConsoleInput::promptRegex("请输入密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    const auto err = courierService_.loginCourier(username, password);
    if (err == CourierServiceError::Nil) {
        ctx.role = Stage2Role::Courier;
        ctx.username = username;
        std::cout << "快递员登录成功！\n";
    } else if (err == CourierServiceError::CourierNotFound) {
        std::cout << "快递员不存在。\n";
    } else {
        std::cout << "密码错误。\n";
    }
}

void Stage2ConsoleApp::handleAdminLogin(Stage2Context& ctx) {
    const auto& admin = adminRepository_.getAdmin();
    const std::string username =
        ConsoleInput::promptNonEmpty("请输入管理员用户名");
    if (username != admin.username()) {
        std::cout << "管理员用户名错误。\n";
        return;
    }

    const std::string password = ConsoleInput::promptNonEmpty("请输入管理员密码");
    if (password != admin.password()) {
        std::cout << "密码错误。\n";
        return;
    }

    ctx.role = Stage2Role::Admin;
    ctx.username = admin.username();
    std::cout << "管理员登录成功！\n";
}

void Stage2ConsoleApp::handleLogout(Stage2Context& ctx) {
    if (ctx.role == Stage2Role::Guest) {
        std::cout << "当前尚未登录。\n";
        return;
    }

    ctx.role = Stage2Role::Guest;
    ctx.username.clear();
    std::cout << "已退出登录。\n";
}

void Stage2ConsoleApp::handleChangePassword(Stage2Context& ctx) {
    if (ctx.role != Stage2Role::User && ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用用户或快递员身份登录。\n";
        return;
    }

    const std::string oldPassword =
        ConsoleInput::promptRegex("请输入原密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");
    const std::string newPassword =
        ConsoleInput::promptRegex("请输入新密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    if (ctx.role == Stage2Role::User) {
        const auto err =
            userService_.updatePassword(ctx.username, oldPassword, newPassword);
        std::cout << (err == UserServiceError::Nil ? "密码修改成功。\n"
                                                   : "密码修改失败。\n");
        return;
    }

    const auto err =
        courierService_.updatePassword(ctx.username, oldPassword, newPassword);
    std::cout << (err == CourierServiceError::Nil ? "密码修改成功。\n"
                                                  : "密码修改失败。\n");
}

void Stage2ConsoleApp::handleGetBalance(const Stage2Context& ctx) {
    if (ctx.role == Stage2Role::User) {
        std::cout << "当前余额: " << userService_.getBalance(ctx.username)
                  << " 元。\n";
    } else if (ctx.role == Stage2Role::Courier) {
        std::cout << "当前余额: " << courierService_.getBalance(ctx.username)
                  << " 元。\n";
    } else if (ctx.role == Stage2Role::Admin) {
        std::cout << "当前余额: " << adminService_.getBalance() << " 元。\n";
    } else {
        std::cout << "请先登录。\n";
    }
}

void Stage2ConsoleApp::handleTopUpBalance(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::User) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::cout << "当前余额: " << userService_.getBalance(ctx.username)
              << " 元。\n";
    const auto amount = ConsoleInput::promptNonNegativeMoney("输入需要充值的金额");
    const auto err = userService_.topUpBalance(ctx.username, amount);
    if (err == UserAccountError::Nil) {
        std::cout << "充值成功！当前余额: "
                  << userService_.getBalance(ctx.username) << " 元。\n";
    } else {
        std::cout << "充值失败。\n";
    }
}

void Stage2ConsoleApp::handleSendParcel(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::User) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    const std::string receiverUsername =
        ConsoleInput::promptNonEmpty("请输入收件人用户名");
    const std::string description =
        ConsoleInput::promptNonEmpty("请输入快递描述");
    const auto item = promptItem();

    const auto [parcelId, err] =
        parcelService_.sendParcel(ctx.username, receiverUsername, description,
                                  *item);
    if (err == ParcelServiceError::Nil) {
        std::cout << "寄件成功，快递单号: " << parcelId << '\n';
        std::cout << "当前余额: " << userService_.getBalance(ctx.username)
                  << " 元。\n";
    } else {
        printParcelServiceError(err);
    }
}

void Stage2ConsoleApp::handleSignParcels(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::User) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    const auto parcels = parcelService_.queryUserParcels(
        ctx.username, UserParcelView::WaitingForSign, ParcelQuery{});
    if (parcels.empty()) {
        std::cout << "暂无待签收快递。\n";
        return;
    }

    printParcelList(parcels);
    const auto selected = parseSelectionIndices(
        ConsoleInput::promptLine(
            "请输入要签收的序号（多个用空格或逗号分隔，输入 all 签收全部，直接回车返回）"),
        parcels.size());
    if (!selected.has_value()) {
        std::cout << "选择不合法。\n";
        return;
    }
    if (selected->empty()) {
        std::cout << "未选择快递。\n";
        return;
    }

    size_t successCount = 0;
    for (const size_t index : *selected) {
        const auto err =
            parcelService_.signParcel(ctx.username, parcels[index].id());
        if (err == ParcelServiceError::Nil) {
            ++successCount;
        } else {
            std::cout << "签收失败: " << parcels[index].id() << "，";
            printParcelServiceError(err);
        }
    }
    std::cout << "签收完成，共成功签收 " << successCount << " 件。\n";
}

void Stage2ConsoleApp::handleQueryUserParcels(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::User) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::cout << "查询范围\n";
    std::cout << "  1. 我发出的快递\n";
    std::cout << "  2. 我收到的快递\n";
    std::cout << "  3. 和我相关的全部快递\n";
    std::cout << "  4. 返回快递菜单\n";
    const int choice = ConsoleInput::promptChoice("请选择一个选项", 1, 4);
    if (choice == 4) {
        return;
    }

    UserParcelView view = UserParcelView::Related;
    if (choice == 1) {
        view = UserParcelView::Sent;
    } else if (choice == 2) {
        view = UserParcelView::Received;
    }

    printParcelList(parcelService_.queryUserParcels(ctx.username, view,
                                                    promptParcelQuery()));
}

void Stage2ConsoleApp::handleListWaitingPickupTasks(
    const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用快递员身份登录。\n";
        return;
    }

    printParcelList(parcelService_.queryCourierParcels(
        ctx.username, CourierParcelView::AssignedWaitingForPickup,
        ParcelQuery{}));
}

void Stage2ConsoleApp::handlePickupParcels(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用快递员身份登录。\n";
        return;
    }

    const auto parcels = parcelService_.queryCourierParcels(
        ctx.username, CourierParcelView::AssignedWaitingForPickup,
        ParcelQuery{});
    if (parcels.empty()) {
        std::cout << "暂无待揽收任务。\n";
        return;
    }

    printParcelList(parcels);
    const auto selected = parseSelectionIndices(
        ConsoleInput::promptLine(
            "请输入要揽收的序号（多个用空格或逗号分隔，输入 all 揽收全部，直接回车返回）"),
        parcels.size());
    if (!selected.has_value()) {
        std::cout << "选择不合法。\n";
        return;
    }

    size_t successCount = 0;
    for (const size_t index : *selected) {
        const auto err = parcelService_.pickupParcel(ctx.username,
                                                     parcels[index].id());
        if (err == ParcelServiceError::Nil) {
            ++successCount;
        } else {
            std::cout << "揽收失败: " << parcels[index].id() << "，";
            printParcelServiceError(err);
        }
    }
    std::cout << "揽收完成，共成功揽收 " << successCount << " 件。\n";
}

void Stage2ConsoleApp::handleQueryCourierParcels(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用快递员身份登录。\n";
        return;
    }

    printParcelList(parcelService_.queryCourierParcels(
        ctx.username, CourierParcelView::Related, promptCourierParcelQuery()));
}

void Stage2ConsoleApp::handleCourierBalance(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用快递员身份登录。\n";
        return;
    }

    std::cout << "当前余额: " << courierService_.getBalance(ctx.username)
              << " 元。\n";
}

void Stage2ConsoleApp::handleListUsers(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }
    printUserList(adminService_.listUsers());
}

void Stage2ConsoleApp::handleListCouriers(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }
    printCourierList(courierService_.listCouriers());
}

void Stage2ConsoleApp::handleAddCourier(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    const std::string username =
        ConsoleInput::promptNonEmpty("请输入快递员用户名");
    const std::string name = ConsoleInput::promptNonEmpty("请输入快递员姓名");
    const std::string phone =
        ConsoleInput::promptRegex("请输入快递员手机号", std::regex(R"(1\d{10})"),
                                  "请输入有效的手机号（11位数字，以1开头）。");
    const std::string password =
        ConsoleInput::promptRegex("请输入初始密码（至少6位）",
                                  ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    const auto err =
        courierService_.addCourier(username, name, phone, password, 0.0);
    if (err == CourierServiceError::Nil) {
        std::cout << "快递员添加成功。\n";
    } else if (err == CourierServiceError::CourierAlreadyExists) {
        std::cout << "快递员用户名已存在。\n";
    } else {
        std::cout << "快递员添加失败。\n";
    }
}

void Stage2ConsoleApp::handleDeleteCourier(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    printCourierList(courierService_.listCouriers());
    const std::string username =
        ConsoleInput::promptNonEmpty("请输入要删除的快递员用户名");
    const auto err = courierService_.deleteCourier(username);
    std::cout << (err == CourierServiceError::Nil ? "快递员删除成功。\n"
                                                  : "快递员不存在。\n");
}

void Stage2ConsoleApp::handleAssignCourier(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    ParcelQuery query;
    query.status = ParcelStatus::WaitingForPickup;
    query.courierUsername = "";
    const auto parcels = parcelService_.queryAdminParcels(query);
    if (parcels.empty()) {
        std::cout << "暂无未分配的待揽收快递。\n";
        return;
    }

    printParcelList(parcels);
    printCourierList(courierService_.listCouriers());
    const int choice =
        ConsoleInput::promptChoice("请选择要分配的快递序号", 1, parcels.size());
    const std::string courierUsername =
        ConsoleInput::promptNonEmpty("请输入快递员用户名");

    const auto err =
        parcelService_.assignCourier(parcels[choice - 1].id(), courierUsername);
    if (err == ParcelServiceError::Nil) {
        std::cout << "快递员分配成功。\n";
    } else {
        printParcelServiceError(err);
    }
}

void Stage2ConsoleApp::handleQueryAdminParcels(const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }
    printParcelList(parcelService_.queryAdminParcels(promptAdminParcelQuery()));
}

void Stage2ConsoleApp::handleQueryCourierTasksAsAdmin(
    const Stage2Context& ctx) {
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    const std::string courierUsername =
        ConsoleInput::promptNonEmpty("请输入快递员用户名");
    printParcelList(parcelService_.queryCourierParcels(
        courierUsername, CourierParcelView::Related, promptCourierParcelQuery()));
}

ParcelQuery Stage2ConsoleApp::promptParcelQuery() {
    ParcelQuery query;
    query.id = ConsoleInput::promptOptionalText("请输入快递单号（可留空）");
    query.senderUsername =
        ConsoleInput::promptOptionalText("请输入发件人用户名（可留空）");
    query.receiverUsername =
        ConsoleInput::promptOptionalText("请输入收件人用户名（可留空）");
    query.status = promptOptionalParcelStatus("请选择状态筛选");
    query.itemType = promptOptionalItemType("请选择物品类型筛选");
    query.sentFrom = ConsoleInput::promptOptionalTimestamp(
        "请输入起始寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    query.sentTo = ConsoleInput::promptOptionalTimestamp(
        "请输入截止寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    return query;
}

ParcelQuery Stage2ConsoleApp::promptAdminParcelQuery() {
    ParcelQuery query = promptParcelQuery();
    query.courierUsername =
        ConsoleInput::promptOptionalText("请输入快递员用户名（可留空）");
    return query;
}

ParcelQuery Stage2ConsoleApp::promptCourierParcelQuery() {
    return promptParcelQuery();
}

std::optional<ParcelStatus> Stage2ConsoleApp::promptOptionalParcelStatus(
    std::string_view label) {
    std::cout << label << '\n';
    std::cout << "  1. 待揽收\n";
    std::cout << "  2. 待签收\n";
    std::cout << "  3. 已签收\n";
    std::cout << "  4. 不限制\n";

    const int choice = ConsoleInput::promptChoice("请选择一个选项", 1, 4);
    switch (choice) {
        case 1:
            return ParcelStatus::WaitingForPickup;
        case 2:
            return ParcelStatus::WaitingForSign;
        case 3:
            return ParcelStatus::Signed;
        default:
            return std::nullopt;
    }
}

std::optional<ParcelItemType> Stage2ConsoleApp::promptOptionalItemType(
    std::string_view label) {
    std::cout << label << '\n';
    std::cout << "  1. 普通快递\n";
    std::cout << "  2. 易碎品\n";
    std::cout << "  3. 图书\n";
    std::cout << "  4. 不限制\n";

    const int choice = ConsoleInput::promptChoice("请选择一个选项", 1, 4);
    switch (choice) {
        case 1:
            return ParcelItemType::Standard;
        case 2:
            return ParcelItemType::Fragile;
        case 3:
            return ParcelItemType::Book;
        default:
            return std::nullopt;
    }
}

std::unique_ptr<Item> Stage2ConsoleApp::promptItem() {
    std::cout << "快递类型\n";
    std::cout << "  1. 普通快递（5元/kg）\n";
    std::cout << "  2. 易碎品（8元/kg）\n";
    std::cout << "  3. 图书（2元/本）\n";
    const int choice = ConsoleInput::promptChoice("请选择一个选项", 1, 3);

    if (choice == 1) {
        return std::make_unique<StandardItem>(
            promptPositiveDouble("请输入重量（kg）"));
    }
    if (choice == 2) {
        return std::make_unique<FragileItem>(
            promptPositiveDouble("请输入重量（kg）"));
    }
    return std::make_unique<BookItem>(promptPositiveInt("请输入图书本数"));
}

double Stage2ConsoleApp::promptPositiveDouble(std::string_view label) {
    while (true) {
        const std::string input = ConsoleInput::promptLine(label);
        try {
            const double value = std::stod(input);
            if (value > 0.0) {
                return value;
            }
        } catch (const std::exception&) {
        }
        std::cout << "请输入大于 0 的数字。\n";
    }
}

int Stage2ConsoleApp::promptPositiveInt(std::string_view label) {
    while (true) {
        const std::string input = ConsoleInput::promptLine(label);
        try {
            size_t parsedLength = 0;
            const int value = std::stoi(input, &parsedLength);
            if (parsedLength == input.size() && value > 0) {
                return value;
            }
        } catch (const std::exception&) {
        }
        std::cout << "请输入大于 0 的整数。\n";
    }
}

std::optional<std::vector<size_t>> Stage2ConsoleApp::parseSelectionIndices(
    std::string_view input,
    size_t maxCount) {
    return ConsoleSelection::parseSelectionIndices(input, maxCount);
}

void Stage2ConsoleApp::printUserList(const std::vector<User>& users) {
    ConsoleDisplay::printUsers(users);
}

void Stage2ConsoleApp::printCourierList(
    const std::vector<Courier>& couriers) {
    ConsoleDisplay::printCouriers(couriers);
}

void Stage2ConsoleApp::printParcelList(const std::vector<Parcel>& parcels) {
    ConsoleDisplay::printParcels(parcels);
}

void Stage2ConsoleApp::printParcelServiceError(ParcelServiceError error) {
    switch (error) {
        case ParcelServiceError::Nil:
            std::cout << "操作成功。\n";
            break;
        case ParcelServiceError::SenderNotFound:
            std::cout << "发件用户不存在。\n";
            break;
        case ParcelServiceError::ReceiverNotFound:
            std::cout << "收件用户不存在。\n";
            break;
        case ParcelServiceError::InsufficientBalance:
            std::cout << "余额不足。\n";
            break;
        case ParcelServiceError::ParcelNotFound:
            std::cout << "快递不存在。\n";
            break;
        case ParcelServiceError::CourierNotFound:
            std::cout << "快递员不存在。\n";
            break;
        case ParcelServiceError::NotWaitingForPickup:
            std::cout << "快递不是待揽收状态。\n";
            break;
        case ParcelServiceError::CourierAlreadyAssigned:
            std::cout << "快递已分配快递员。\n";
            break;
        case ParcelServiceError::NotAssignedCourier:
            std::cout << "不能揽收不属于自己的快递。\n";
            break;
        case ParcelServiceError::NotWaitingForSign:
            std::cout << "快递不是待签收状态。\n";
            break;
        case ParcelServiceError::AlreadySigned:
            std::cout << "快递已签收。\n";
            break;
        case ParcelServiceError::InvalidDescription:
            std::cout << "快递描述不能为空。\n";
            break;
        case ParcelServiceError::InvalidItem:
            std::cout << "物品信息不合法。\n";
            break;
        case ParcelServiceError::AdminInsufficientBalance:
            std::cout << "管理员账户余额不足，无法支付快递员分成。\n";
            break;
        default:
            std::cout << "操作失败。\n";
            break;
    }
}

int Stage2ConsoleApp::run() {
    std::cout << "ExpressFlow Stage 2" << '\n';

    Stage2Context ctx;
    while (true) {
        const int choice = Stage2ConsoleMenu::showMainMenu(ctx);
        switch (choice) {
            case 1: {
                const int accountChoice =
                    Stage2ConsoleMenu::showAccountMenu(ctx);
                switch (accountChoice) {
                    case 1:
                        handleUserRegister();
                        break;
                    case 2:
                        handleUserLogin(ctx);
                        break;
                    case 3:
                        handleCourierLogin(ctx);
                        break;
                    case 4:
                        handleAdminLogin(ctx);
                        break;
                    case 5:
                        handleChangePassword(ctx);
                        break;
                    case 6:
                        handleLogout(ctx);
                        break;
                    default:
                        break;
                }
                break;
            }
            case 2: {
                const int balanceChoice =
                    Stage2ConsoleMenu::showBalanceMenu(ctx);
                if (balanceChoice == 1) {
                    handleGetBalance(ctx);
                } else if (balanceChoice == 2 && ctx.role == Stage2Role::User) {
                    handleTopUpBalance(ctx);
                }
                break;
            }
            case 3: {
                const int parcelChoice =
                    Stage2ConsoleMenu::showUserParcelMenu(ctx);
                if (parcelChoice == 1) {
                    handleSendParcel(ctx);
                } else if (parcelChoice == 2) {
                    handleSignParcels(ctx);
                } else if (parcelChoice == 3) {
                    handleQueryUserParcels(ctx);
                }
                break;
            }
            case 4: {
                const int courierChoice =
                    Stage2ConsoleMenu::showCourierTaskMenu(ctx);
                if (courierChoice == 1) {
                    handleListWaitingPickupTasks(ctx);
                } else if (courierChoice == 2) {
                    handlePickupParcels(ctx);
                } else if (courierChoice == 3) {
                    handleQueryCourierParcels(ctx);
                } else if (courierChoice == 4) {
                    handleCourierBalance(ctx);
                }
                break;
            }
            case 5: {
                const int adminChoice = Stage2ConsoleMenu::showAdminMenu(ctx);
                if (adminChoice == 1) {
                    handleListUsers(ctx);
                } else if (adminChoice == 2) {
                    handleListCouriers(ctx);
                } else if (adminChoice == 3) {
                    handleAddCourier(ctx);
                } else if (adminChoice == 4) {
                    handleDeleteCourier(ctx);
                } else if (adminChoice == 5) {
                    handleAssignCourier(ctx);
                } else if (adminChoice == 6) {
                    handleQueryAdminParcels(ctx);
                } else if (adminChoice == 7) {
                    handleQueryCourierTasksAsAdmin(ctx);
                }
                break;
            }
            case 6:
                std::cout << "再见。\n";
                return 0;
            default:
                break;
        }
    }
}

}  // namespace exf
