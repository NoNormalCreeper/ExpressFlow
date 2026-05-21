
#include <regex>
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
    static void printMenu(const std::string_view title,
                          const std::vector<std::string>& options,
                          int startIndex = 1);
    static void printSeparator(const std::string_view separator = "-",
                               int length = 20);

   public:
    static int showMainMenu(MainMenu::Context& ctx);
    static int showUserMenu(MainMenu::Context& ctx);
};

}  // namespace exf