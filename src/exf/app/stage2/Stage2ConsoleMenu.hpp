#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace exf {

enum class Stage2Role {
    Guest,
    User,
    Courier,
    Admin
};

struct Stage2Context {
    Stage2Role role = Stage2Role::Guest;
    std::string username;
};

/**
 * Stage2ConsoleMenu 输出阶段 2 控制台菜单。
 */
class Stage2ConsoleMenu {
   public:
    static int showMainMenu(const Stage2Context& ctx);
    static int showAccountMenu(const Stage2Context& ctx);
    static int showBalanceMenu(const Stage2Context& ctx);
    static int showUserParcelMenu(const Stage2Context& ctx);
    static int showCourierTaskMenu(const Stage2Context& ctx);
    static int showAdminMenu(const Stage2Context& ctx);

   private:
    static void printMenu(std::string_view title,
                          const std::vector<std::string>& options,
                          int startIndex = 1);
    static void printSeparator(std::string_view separator = "-",
                               int length = 20);
    static void printLoginStatus(const Stage2Context& ctx);
    static const char* roleText(Stage2Role role);
};

}  // namespace exf
