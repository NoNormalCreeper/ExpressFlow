#include "ConsoleMenu.hpp"
#include <iostream>
#include "ConsoleInput.hpp"

namespace exf {
void ConsoleMenu::printMenu(const std::string_view title,
                            const std::vector<std::string>& options,
                            int startIndex) {
    std::cout << title << '\n';
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (startIndex + i) << ". " << options[i] << '\n';
    }
}

void ConsoleMenu::printSeparator(const std::string_view separator, int length) {
    std::cout << std::string(length, separator.empty() ? '-' : separator[0])
              << '\n';
}

void ConsoleMenu::printLoginStatus(MainMenu::Context& ctx) {
    std::cout << "登录状态：" << (ctx.loggedIn ? "已登录" : "未登录");
    if (ctx.loggedIn) {
        std::cout << " (" << ctx.username << ")";
    }
    if (ctx.loggedIn == 2) {
        std::cout << " (管理员)";
    }
    std::cout << '\n';
}

int ConsoleMenu::showMainMenu(MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"用户菜单", "余额管理菜单"};

    printSeparator();
    printMenu("主菜单", options);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showUserMenu(MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"注册", "登录", "修改密码",
                                              "返回主菜单"};
    printSeparator();
    printMenu("用户菜单", options);
    printLoginStatus(ctx);

    int choice =
        ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
    return choice;
}

int ConsoleMenu::showAccountMenu(MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"查看余额", "充值账户", "返回主菜单"};
    printSeparator();
    if (ctx.loggedIn == 0) {
        std::cout << "未登录，请登录后再操作。\n";
        return 3;
    }

    printMenu("余额管理菜单", options);
    printLoginStatus(ctx);

    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

}  // namespace exf