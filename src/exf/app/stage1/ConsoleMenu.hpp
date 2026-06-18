#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "exf/util/Money.hpp"

namespace exf {

namespace MainMenu {

using Context = struct Context {
    int loggedIn = 0;  // 0: 未登录, 1: 用户, 2: 管理员
    std::string username;
};

}  // namespace MainMenu


class ConsoleMenu {
   private:
    static void printMenu(std::string_view title,
                          const std::vector<std::string>& options,
                          int startIndex = 1);
    static void printSeparator(std::string_view separator = "-",
                               int length = 20);

    static void printLoginStatus(const MainMenu::Context& ctx);

   public:
    static int showMainMenu(const MainMenu::Context& ctx);
    static int showUserMenu(const MainMenu::Context& ctx);
    static int showAccountMenu(const MainMenu::Context& ctx);
    static int showParcelMenu(const MainMenu::Context& ctx);
    static int showAdminMenu(const MainMenu::Context& ctx);
};

}  // namespace exf
