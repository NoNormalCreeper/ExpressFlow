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

int ConsoleMenu::showMainMenu(MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"用户菜单"};

    printSeparator();
    printMenu("主菜单", options);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showUserMenu(MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"注册", "登录", "修改密码",
                                              "返回主菜单"};
    printSeparator();
    printMenu("用户菜单", options);
    std::cout << "登录状态：" << (ctx.loggedIn ? "已登录" : "未登录");
    if (ctx.loggedIn) {
        std::cout << " (" << ctx.username << ")";
    }
    if (ctx.loggedIn == 2) {
        std::cout << " (管理员)";
    }
    std::cout << '\n';

    int choice =
        ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
    return choice;
}

}  // namespace exf