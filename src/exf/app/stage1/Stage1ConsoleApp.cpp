#include "exf/app/stage1/Stage1ConsoleApp.hpp"

#include <iostream>

namespace exf {

Stage1ConsoleApp::Stage1ConsoleApp(std::filesystem::path dataDir)
    : storage_(std::move(dataDir)),
      userRepository_(storage_),
      adminRepository_(storage_),
      authService_(userRepository_, adminRepository_),
      userService_(userRepository_) {}

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
    util::Money initialBalance;

    User newUser(username, name, phone, password, address, initialBalance);
    UserServiceError err =
        userService_.registerUser(username, name, phone, password, address);
    switch (err) {
        case UserServiceError::Nil:  // 理论上只应该走到这里
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
    }
}

void Stage1ConsoleApp::handleChangePassword(MainMenu::Context& ctx) {
    if (ctx.loggedIn == 0) {
        std::cout << "尚未登录。\n";
        return;
    }

    std::string username, originalPassword, newPassword;

    // while (true) {
    //     username = ConsoleInput::promptNonEmpty("请输入用户名");
    //     if (userRepository_.userExists(username)) {
    //         break;
    //     }
    //     std::cout << "用户名不存在，请重新输入。\n";
    // }

    username = ctx.username;

    originalPassword =
        ConsoleInput::promptRegex("请输入密码", ConsoleInput::passwordRegex,
                                  "密码至少需要6位，请重新输入。");

    auto err = userService_.loginUser(username, originalPassword);
    switch (err) {
        case UserServiceError::Nil: {
            ctx.loggedIn = 1;
            ctx.username = username;
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

            auto err = userService_.updatePassword(username, originalPassword,
                                                   newPassword);

            if (err == UserServiceError::Nil) {
                std::cout << "密码修改成功。\n";
            } else {
                std::cout << "密码修改失败。\n";  // 不应该走到此分支
            }

            break;
        }

        case UserServiceError::UserNotFound:
            std::cout << "用户不存在。\n";
            break;
        case UserServiceError::IncorrectPassword:
            std::cout << "原密码错误。\n";
            break;
    }
}

void Stage1ConsoleApp::handleGetBalance(MainMenu::Context& ctx) {
    std::cout << "当前余额: " << userService_.getBalance(ctx.username)
              << " 元。\n";
}

void Stage1ConsoleApp::handleTopUpBalance(MainMenu::Context& ctx) {
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
        case UserAccountError::InvalidAmount:
            std::cout << "金额有误。\n";
    }
}

// 打印阶段 1 的占位菜单。
int Stage1ConsoleApp::run() {
    std::cout << "ExpressFlow Stage 1" << '\n';
    // ConsoleMenu::printSeparator();

    MainMenu::Context ctx;
    while (true) {
        int choice = ConsoleMenu::showMainMenu(ctx);
        switch (choice) {
            case 1: {
                // 用户菜单
                int userChoice = ConsoleMenu::showUserMenu(ctx);
                switch (userChoice) {
                    case 1:
                        handleUserRegister();
                        break;
                    case 2:
                        handleUserLogin(ctx);
                        break;
                    case 3:
                        handleChangePassword(ctx);
                        break;
                    default:
                        // 返回主菜单
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
                    default:
                        break;
                }
                break;
            }
        }
    }

    return 0;
}

}  // namespace exf
