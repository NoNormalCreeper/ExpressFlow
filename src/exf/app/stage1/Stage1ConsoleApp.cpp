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
                                         std::regex(R"(.{6,})"),
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

void Stage1ConsoleApp::handleUserLogin() {
    // 实现用户登录逻辑
}

void Stage1ConsoleApp::handleChangePassword() {
    // 实现修改密码逻辑
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
                        handleUserLogin();
                        break;
                    case 3:
                        handleChangePassword();
                        break;
                    case 4:
                        // 返回主菜单
                        break;
                }
            }
        }
    }

    return 0;
}

}  // namespace exf
