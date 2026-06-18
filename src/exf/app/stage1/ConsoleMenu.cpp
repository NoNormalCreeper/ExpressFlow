#include "ConsoleMenu.hpp"

#include <cstddef>
#include <iostream>

#include "ConsoleInput.hpp"

namespace exf {

void ConsoleMenu::printMenu(std::string_view title,
                            const std::vector<std::string>& options,
                            int startIndex) {
    std::cout << title << '\n';
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (startIndex + i) << ". " << options[i] << '\n';
    }
}

void ConsoleMenu::printSeparator(std::string_view separator, int length) {
    std::cout << std::string(length, separator.empty() ? '-' : separator[0])
              << '\n';
}

void ConsoleMenu::printLoginStatus(const MainMenu::Context& ctx) {
    std::cout << "登录状态：" << (ctx.loggedIn ? "已登录" : "未登录");
    if (ctx.loggedIn == 1) {
        std::cout << " (用户: " << ctx.username << ")";
    } else if (ctx.loggedIn == 2) {
        std::cout << " (管理员: " << ctx.username << ")";
    }
    std::cout << '\n';
}

int ConsoleMenu::showMainMenu(const MainMenu::Context& ctx) {
    const std::vector<std::string> options = {
        "账号菜单", "余额管理菜单", "快递菜单", "管理员菜单", "退出程序"};

    printSeparator();
    printMenu("主菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showUserMenu(const MainMenu::Context& ctx) {
    const std::vector<std::string> options = {
        "注册", "用户登录", "管理员登录", "修改密码", "退出登录", "返回主菜单"};
    printSeparator();
    printMenu("账号菜单", options);
    printLoginStatus(ctx);

    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showAccountMenu(const MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"查看余额", "充值账户",
                                              "返回主菜单"};
    printSeparator();
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return 3;
    }

    printMenu("余额管理菜单", options);
    printLoginStatus(ctx);

    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showParcelMenu(const MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"发送快递", "待签收并签收",
                                              "查询快递", "返回主菜单"};
    printSeparator();
    if (ctx.loggedIn != 1) {
        std::cout << "请先使用用户身份登录。\n";
        return 4;
    }

    printMenu("快递菜单", options);
    printLoginStatus(ctx);

    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int ConsoleMenu::showAdminMenu(const MainMenu::Context& ctx) {
    const std::vector<std::string> options = {"查看所有用户信息",
                                              "查询全部快递",
                                              "返回主菜单"};
    printSeparator();
    if (ctx.loggedIn != 2) {
        std::cout << "请先使用管理员身份登录。\n";
        return 3;
    }

    printMenu("管理员菜单", options);
    printLoginStatus(ctx);

    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

}  // namespace exf
