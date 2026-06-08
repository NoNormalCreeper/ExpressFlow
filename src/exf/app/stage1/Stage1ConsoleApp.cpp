#include "exf/app/stage1/Stage1ConsoleApp.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <regex>
#include <sstream>
#include <utility>

#include "exf/util/TimeUtil.hpp"

namespace exf {

Stage1ConsoleApp::Stage1ConsoleApp(std::filesystem::path dataDir)
    : storage_(std::move(dataDir)),
      userRepository_(storage_),
      adminRepository_(storage_),
      parcelRepository_(storage_),
      authService_(userRepository_, adminRepository_),
      userService_(userRepository_),
      adminService_(userRepository_),
      parcelService_(userRepository_, adminRepository_, parcelRepository_) {}

void Stage1ConsoleApp::handleUserRegister() {
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

    UserServiceError err =
        userService_.registerUser(username, name, phone, password, address);
    switch (err) {
        case UserServiceError::Nil:
            std::cout << "注册成功！\n";
            break;
        case UserServiceError::UserAlreadyExists:
            std::cout << "用户已存在。\n";
            break;
        default:
            std::cout << "注册失败。\n";
            break;
    }
}

void Stage1ConsoleApp::handleUserLogin(MainMenu::Context& ctx) {
    std::string username, password;

    while (true) {
        username = ConsoleInput::promptNonEmpty("请输入用户名");
        if (userRepository_.userExists(username)) {
            break;
        }
        std::cout << "用户名不存在，请重新输入。\n";
    }

    password =
        ConsoleInput::promptRegex("请输入密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    auto err = userService_.loginUser(username, password);
    switch (err) {
        case UserServiceError::Nil:
            ctx.loggedIn = 1;
            ctx.username = username;
            std::cout << "登录成功！\n";
            break;

        case UserServiceError::UserNotFound:
            std::cout << "用户不存在。\n";
            break;
        case UserServiceError::IncorrectPassword:
            std::cout << "密码错误。\n";
            break;
        default:
            break;
    }
}

void Stage1ConsoleApp::handleAdminLogin(MainMenu::Context& ctx) {
    const auto& admin = adminRepository_.getAdmin();
    std::string username, password;

    username = ConsoleInput::promptNonEmpty("请输入管理员用户名");
    if (username != admin.username()) {
        std::cout << "管理员用户名错误。\n";
        return;
    }

    password = ConsoleInput::promptNonEmpty("请输入管理员密码");
    if (password != admin.password()) {
        std::cout << "密码错误。\n";
        return;
    }

    ctx.loggedIn = 2;
    ctx.username = admin.username();
    std::cout << "管理员登录成功！\n";
}

void Stage1ConsoleApp::handleLogout(MainMenu::Context& ctx) {
    if (ctx.loggedIn == 0) {
        std::cout << "当前尚未登录。\n";
        return;
    }

    ctx.loggedIn = 0;
    ctx.username.clear();
    std::cout << "已退出登录。\n";
}

void Stage1ConsoleApp::handleChangePassword(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::string username = ctx.username;
    std::string originalPassword, newPassword;

    originalPassword =
        ConsoleInput::promptRegex("请输入密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    auto err = userService_.loginUser(username, originalPassword);
    switch (err) {
        case UserServiceError::Nil: {
            std::cout << "密码验证通过。\n";

            while (true) {
                newPassword = ConsoleInput::promptRegex(
                    "请输入新密码", ConsoleInput::passwordRegex,
                    "密码至少需要6位，请重新输入。");

                auto confirmPassword =
                    ConsoleInput::promptNonEmpty("请再次输入新密码以确认");

                if (confirmPassword == newPassword) {
                    break;
                }

                std::cout << "两次输入的密码不匹配，请重新输入。\n";
            }

            err = userService_.updatePassword(username, originalPassword,
                                              newPassword);

            if (err == UserServiceError::Nil) {
                std::cout << "密码修改成功。\n";
            } else {
                std::cout << "密码修改失败。\n";
            }

            break;
        }

        case UserServiceError::UserNotFound:
            std::cout << "用户不存在。\n";
            break;
        case UserServiceError::IncorrectPassword:
            std::cout << "原密码错误。\n";
            break;
        default:
            break;
    }
}

void Stage1ConsoleApp::handleGetBalance(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::cout << "当前余额: " << userService_.getBalance(ctx.username)
              << " 元。\n";
}

void Stage1ConsoleApp::handleTopUpBalance(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::cout << "当前余额: " << userService_.getBalance(ctx.username)
              << " 元。\n";

    auto amount = ConsoleInput::promptNonNegativeMoney("输入需要充值的金额");
    auto err = userService_.topUpBalance(ctx.username, amount);
    switch (err) {
        case UserAccountError::Nil:
            std::cout << "充值成功！当前余额: "
                      << userService_.getBalance(ctx.username) << " 元。\n";
            break;
        case UserAccountError::UserNotFound:
            std::cout << "用户不存在。\n";
            break;
        case UserAccountError::InvalidAmount:
            std::cout << "金额有误。\n";
            break;
        default:
            break;
    }
}

void Stage1ConsoleApp::handleSendParcel(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return;
    }

    std::cout << "当前固定运费: " << parcelService_.stage1ParcelPrice()
              << " 元。\n";
    std::string receiverUsername =
        ConsoleInput::promptNonEmpty("请输入收件人用户名");
    std::string description = ConsoleInput::promptNonEmpty("请输入快递描述");

    const auto [parcelId, err] =
        parcelService_.sendParcel(ctx.username, receiverUsername, description);
    switch (err) {
        case ParcelServiceError::Nil:
            std::cout << "寄件成功，快递单号: " << parcelId << '\n';
            std::cout << "当前余额: " << userService_.getBalance(ctx.username)
                      << " 元。\n";
            break;
        case ParcelServiceError::SenderNotFound:
            std::cout << "发件用户不存在。\n";
            break;
        case ParcelServiceError::ReceiverNotFound:
            std::cout << "收件用户不存在。\n";
            break;
        case ParcelServiceError::InsufficientBalance:
            std::cout << "余额不足，无法发送快递。\n";
            break;
        case ParcelServiceError::AdminNotFound:
            std::cout << "管理员账户不存在，无法收取运费。\n";
            break;
        case ParcelServiceError::InvalidDescription:
            std::cout << "快递描述不能为空。\n";
            break;
        default:
            std::cout << "寄件失败。\n";
            break;
    }
}

void Stage1ConsoleApp::handleSignParcels(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
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

    while (true) {
        const auto input = ConsoleInput::promptLine(
            "请输入要签收的序号（多个用空格或逗号分隔，输入 all "
            "签收全部，直接回车返回）");
        const auto selected = parseSelectionIndices(input, parcels.size());
        if (!selected.has_value()) {
            std::cout << "选择不合法，请重新输入。\n";
            continue;
        }
        if (selected->empty()) {
            std::cout << "未选择快递。\n";
            return;
        }

        size_t successCount = 0;
        for (const size_t index : *selected) {
            const auto err =
                parcelService_.signParcel(ctx.username, parcels[index].id());
            switch (err) {
                case ParcelServiceError::Nil:
                    ++successCount;
                    break;
                case ParcelServiceError::ParcelNotFound:
                    std::cout << "快递不存在: " << parcels[index].id() << '\n';
                    break;
                case ParcelServiceError::NotReceiver:
                    std::cout
                        << "不能签收不属于自己的快递: " << parcels[index].id()
                        << '\n';
                    break;
                case ParcelServiceError::AlreadySigned:
                    std::cout << "快递已签收: " << parcels[index].id() << '\n';
                    break;
                default:
                    std::cout << "签收失败: " << parcels[index].id() << '\n';
                    break;
            }
        }

        std::cout << "签收完成，共成功签收 " << successCount << " 件。\n";
        return;
    }
}

void Stage1ConsoleApp::handleQueryUserParcels(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 1) {
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
    switch (choice) {
        case 1:
            view = UserParcelView::Sent;
            break;
        case 2:
            view = UserParcelView::Received;
            break;
        case 3:
            view = UserParcelView::Related;
            break;
        default:
            break;
    }

    const auto query = promptUserParcelQuery();
    const auto parcels =
        parcelService_.queryUserParcels(ctx.username, view, query);
    printParcelList(parcels);
}

void Stage1ConsoleApp::handleQueryAdminParcels(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 2) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    const auto query = promptAdminParcelQuery();
    printParcelList(parcelService_.queryAdminParcels(query));
}

void Stage1ConsoleApp::handleListUsers(MainMenu::Context& ctx) {
    if (ctx.loggedIn != 2) {
        std::cout << "请先使用管理员身份登录。\n";
        return;
    }

    printUserList(adminService_.listUsers());
}

ParcelQuery Stage1ConsoleApp::promptUserParcelQuery() {
    ParcelQuery query;
    query.id = ConsoleInput::promptOptionalText("请输入快递单号（可留空）");
    query.senderUsername =
        ConsoleInput::promptOptionalText("请输入发件人用户名（可留空）");
    query.receiverUsername =
        ConsoleInput::promptOptionalText("请输入收件人用户名（可留空）");
    query.status = promptOptionalParcelStatus("请选择状态筛选");
    query.sentFrom = ConsoleInput::promptOptionalTimestamp(
        "请输入起始寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    query.sentTo = ConsoleInput::promptOptionalTimestamp(
        "请输入截止寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    return query;
}

ParcelQuery Stage1ConsoleApp::promptAdminParcelQuery() {
    ParcelQuery query;
    query.id = ConsoleInput::promptOptionalText("请输入快递单号（可留空）");
    query.senderUsername =
        ConsoleInput::promptOptionalText("请输入发件人用户名（可留空）");
    query.receiverUsername =
        ConsoleInput::promptOptionalText("请输入收件人用户名（可留空）");
    query.status = promptOptionalParcelStatus("请选择状态筛选");
    query.sentFrom = ConsoleInput::promptOptionalTimestamp(
        "请输入起始寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    query.sentTo = ConsoleInput::promptOptionalTimestamp(
        "请输入截止寄件时间（YYYY-MM-DD HH:MM:SS，可留空）");
    return query;
}

std::optional<ParcelStatus> Stage1ConsoleApp::promptOptionalParcelStatus(
    std::string_view label) {
    std::cout << label << '\n';
    std::cout << "  1. 待签收\n";
    std::cout << "  2. 已签收\n";
    std::cout << "  3. 不限制\n";

    const int choice = ConsoleInput::promptChoice("请选择一个选项", 1, 3);
    switch (choice) {
        case 1:
            return ParcelStatus::WaitingForSign;
        case 2:
            return ParcelStatus::Signed;
        default:
            return std::nullopt;
    }
}

std::optional<std::vector<size_t>> Stage1ConsoleApp::parseSelectionIndices(
    std::string_view input,
    size_t maxCount) {
    std::string normalized(input);
    std::replace(normalized.begin(), normalized.end(), ',', ' ');

    std::istringstream stream(normalized);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return std::vector<size_t>{};
    }
    if (tokens.size() == 1 && tokens.front() == "all") {
        std::vector<size_t> all;
        all.reserve(maxCount);
        for (size_t i = 0; i < maxCount; ++i) {
            all.push_back(i);
        }
        return all;
    }

    std::vector<size_t> indices;
    std::vector<bool> used(maxCount, false);
    for (const auto& item : tokens) {
        size_t parsedLength = 0;
        int parsedIndex = 0;
        try {
            parsedIndex = std::stoi(item, &parsedLength);
        } catch (const std::exception&) {
            return std::nullopt;
        }

        if (parsedLength != item.size() || parsedIndex <= 0 ||
            static_cast<size_t>(parsedIndex) > maxCount) {
            return std::nullopt;
        }

        const size_t zeroBasedIndex = static_cast<size_t>(parsedIndex - 1);
        if (!used[zeroBasedIndex]) {
            indices.push_back(zeroBasedIndex);
            used[zeroBasedIndex] = true;
        }
    }

    return indices;
}

std::string Stage1ConsoleApp::parcelStatusText(ParcelStatus status) {
    switch (status) {
        case ParcelStatus::WaitingForSign:
            return "待签收";
        case ParcelStatus::Signed:
            return "已签收";
        case ParcelStatus::WaitingForPickup:
            return "待揽收";
    }

    return "未知";
}

std::string Stage1ConsoleApp::formatTimestampForDisplay(
    std::string_view timestamp) {
    if (timestamp.empty()) {
        return "-";
    }

    try {
        return TimeUtil::formatTimestamp(timestamp);
    } catch (const std::exception&) {
        return std::string(timestamp);
    }
}

void Stage1ConsoleApp::printParcelList(const std::vector<Parcel>& parcels) {
    if (parcels.empty()) {
        std::cout << "没有符合条件的快递。\n";
        return;
    }

    std::cout << "共 " << parcels.size() << " 件快递：\n";
    for (size_t i = 0; i < parcels.size(); ++i) {
        const auto& parcel = parcels[i];
        std::cout << "  [" << (i + 1) << "] " << parcel.id() << " | "
                  << parcel.senderUsername() << " -> "
                  << parcel.receiverUsername() << " | "
                  << parcelStatusText(parcel.status())
                  << " | 寄件: " << formatTimestampForDisplay(parcel.sentAt())
                  << " | 签收: "
                  << formatTimestampForDisplay(parcel.receivedAt())
                  << " | 运费: " << parcel.fee() << " 元 | "
                  << parcel.description() << '\n';
    }
}

void Stage1ConsoleApp::printUserList(const std::vector<User>& users) {
    if (users.empty()) {
        std::cout << "暂无注册用户。\n";
        return;
    }

    std::cout << "共 " << users.size() << " 个用户：\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& user = users[i];
        std::cout << "  [" << (i + 1) << "] " << user.username() << " | "
                  << user.name() << " | " << user.phone() << " | "
                  << user.address() << " | 余额: "
                  << user.account().balance() << " 元\n";
    }
}

int Stage1ConsoleApp::run() {
    std::cout << "ExpressFlow Stage 1" << '\n';

    MainMenu::Context ctx;
    while (true) {
        int choice = ConsoleMenu::showMainMenu(ctx);
        switch (choice) {
            case 1: {
                int userChoice = ConsoleMenu::showUserMenu(ctx);
                switch (userChoice) {
                    case 1:
                        handleUserRegister();
                        break;
                    case 2:
                        handleUserLogin(ctx);
                        break;
                    case 3:
                        handleAdminLogin(ctx);
                        break;
                    case 4:
                        handleChangePassword(ctx);
                        break;
                    case 5:
                        handleLogout(ctx);
                        break;
                    default:
                        break;
                }
                break;
            }
            case 2: {
                int userChoice = ConsoleMenu::showAccountMenu(ctx);
                switch (userChoice) {
                    case 1:
                        handleGetBalance(ctx);
                        break;
                    case 2:
                        handleTopUpBalance(ctx);
                        break;
                    default:
                        break;
                }
                break;
            }
            case 3: {
                int parcelChoice = ConsoleMenu::showParcelMenu(ctx);
                switch (parcelChoice) {
                    case 1:
                        handleSendParcel(ctx);
                        break;
                    case 2:
                        handleSignParcels(ctx);
                        break;
                    case 3:
                        handleQueryUserParcels(ctx);
                        break;
                    default:
                        break;
                }
                break;
            }
            case 4: {
                int adminChoice = ConsoleMenu::showAdminMenu(ctx);
                switch (adminChoice) {
                    case 1:
                        handleListUsers(ctx);
                        break;
                    case 2:
                        handleQueryAdminParcels(ctx);
                        break;
                    default:
                        break;
                }
                break;
            }
            case 5:
                std::cout << "再见。\n";
                return 0;
            default:
                break;
        }
    }

    return 0;
}

}  // namespace exf
