#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "exf/util/Money.hpp"

namespace exf {

namespace MainMenu {

/**
 * Context 保存当前控制台登录状态。
 */
using Context = struct Context {
    /** 登录角色，0 表示未登录，1 表示用户，2 表示管理员。 */
    int loggedIn = 0;  // 0: 未登录, 1: 用户, 2: 管理员

    /** 当前登录用户名。 */
    std::string username;
};

}  // namespace MainMenu


/**
 * ConsoleMenu 负责阶段 1 控制台菜单显示和选项读取。
 */
class ConsoleMenu {
   private:
    /** 打印带序号的菜单项。 */
    static void printMenu(std::string_view title,
                          const std::vector<std::string>& options,
                          int startIndex = 1);

    /** 打印分隔线。 */
    static void printSeparator(std::string_view separator = "-",
                               int length = 20);

    /** 打印当前登录状态。 */
    static void printLoginStatus(const MainMenu::Context& ctx);

   public:
    /** 显示主菜单并返回选择。 */
    static int showMainMenu(const MainMenu::Context& ctx);

    /** 显示账号菜单并返回选择。 */
    static int showUserMenu(const MainMenu::Context& ctx);

    /** 显示余额菜单并返回选择。 */
    static int showAccountMenu(const MainMenu::Context& ctx);

    /** 显示快递菜单并返回选择。 */
    static int showParcelMenu(const MainMenu::Context& ctx);

    /** 显示管理员菜单并返回选择。 */
    static int showAdminMenu(const MainMenu::Context& ctx);
};

}  // namespace exf
