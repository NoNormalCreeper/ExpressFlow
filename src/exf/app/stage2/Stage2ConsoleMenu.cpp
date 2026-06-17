#include "exf/app/stage2/Stage2ConsoleMenu.hpp"

#include <cstddef>
#include <iostream>

#include "exf/app/console/ConsoleInput.hpp"
#include "exf/app/console/ConsoleMenu.hpp"

namespace exf {

void Stage2ConsoleMenu::printMenu(
    std::string_view title,
    const std::vector<std::string>& options,
    int startIndex) {
    ConsoleMenu::print(title, options, startIndex);
}

void Stage2ConsoleMenu::printSeparator(std::string_view separator,
                                       int length) {
    ConsoleMenu::printSeparator(separator, length);
}

const char* Stage2ConsoleMenu::roleText(Stage2Role role) {
    switch (role) {
        case Stage2Role::Guest:
            return "未登录";
        case Stage2Role::User:
            return "用户";
        case Stage2Role::Courier:
            return "快递员";
        case Stage2Role::Admin:
            return "管理员";
    }
    return "未知";
}

void Stage2ConsoleMenu::printLoginStatus(const Stage2Context& ctx) {
    std::cout << "登录状态：";
    if (ctx.role == Stage2Role::Guest) {
        std::cout << "未登录";
    } else {
        std::cout << "已登录 (" << roleText(ctx.role) << ": " << ctx.username
                  << ")";
    }
    std::cout << '\n';
}

int Stage2ConsoleMenu::showMainMenu(const Stage2Context& ctx) {
    const std::vector<std::string> options = {
        "账号菜单",      "账户余额菜单", "用户快递菜单",
        "快递员任务菜单", "管理员菜单",   "退出程序"};

    printSeparator();
    printMenu("主菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int Stage2ConsoleMenu::showAccountMenu(const Stage2Context& ctx) {
    const std::vector<std::string> options = {
        "用户注册", "用户登录", "快递员登录", "管理员登录",
        "修改密码", "退出登录", "返回主菜单"};

    printSeparator();
    printMenu("账号菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int Stage2ConsoleMenu::showBalanceMenu(const Stage2Context& ctx) {
    printSeparator();
    if (ctx.role == Stage2Role::Guest) {
        std::cout << "请先登录。\n";
        return 3;
    }

    const std::vector<std::string> options =
        ctx.role == Stage2Role::User
            ? std::vector<std::string>{"查看余额", "充值账户", "返回主菜单"}
            : std::vector<std::string>{"查看余额", "返回主菜单"};

    printMenu("账户余额菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int Stage2ConsoleMenu::showUserParcelMenu(const Stage2Context& ctx) {
    const std::vector<std::string> options = {"发送快递", "待签收并签收",
                                              "查询快递", "返回主菜单"};
    printSeparator();
    if (ctx.role != Stage2Role::User) {
        std::cout << "请先使用用户身份登录。\n";
        return 4;
    }

    printMenu("用户快递菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int Stage2ConsoleMenu::showCourierTaskMenu(const Stage2Context& ctx) {
    const std::vector<std::string> options = {
        "查看待揽收任务", "揽收快递", "查询我的快递任务",
        "查看账户余额",   "返回主菜单"};
    printSeparator();
    if (ctx.role != Stage2Role::Courier) {
        std::cout << "请先使用快递员身份登录。\n";
        return 5;
    }

    printMenu("快递员任务菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

int Stage2ConsoleMenu::showAdminMenu(const Stage2Context& ctx) {
    const std::vector<std::string> options = {
        "查看所有用户", "查看所有快递员", "添加快递员",
        "删除快递员", "分配快递员",   "查询全部快递",
        "查询快递员任务", "返回主菜单"};
    printSeparator();
    if (ctx.role != Stage2Role::Admin) {
        std::cout << "请先使用管理员身份登录。\n";
        return 8;
    }

    printMenu("管理员菜单", options);
    printLoginStatus(ctx);
    return ConsoleInput::promptChoice("请选择一个选项", 1, options.size());
}

}  // namespace exf
