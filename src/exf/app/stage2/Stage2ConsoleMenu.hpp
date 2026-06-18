#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace exf {

/**
 * Stage2Role 表示阶段 2 控制台当前登录角色。
 */
enum class Stage2Role {
    Guest,
    User,
    Courier,
    Admin
};

/**
 * Stage2Context 保存当前登录角色和用户名。
 */
struct Stage2Context {
    /** 当前登录角色。 */
    Stage2Role role = Stage2Role::Guest;

    /** 当前登录用户名。 */
    std::string username;
};

/**
 * Stage2ConsoleMenu 输出阶段 2 控制台菜单。
 */
class Stage2ConsoleMenu {
   public:
    /** 显示主菜单并返回选择。 */
    static int showMainMenu(const Stage2Context& ctx);

    /** 显示账号菜单并返回选择。 */
    static int showAccountMenu(const Stage2Context& ctx);

    /** 显示余额菜单并返回选择。 */
    static int showBalanceMenu(const Stage2Context& ctx);

    /** 显示用户快递菜单并返回选择。 */
    static int showUserParcelMenu(const Stage2Context& ctx);

    /** 显示快递员任务菜单并返回选择。 */
    static int showCourierTaskMenu(const Stage2Context& ctx);

    /** 显示管理员菜单并返回选择。 */
    static int showAdminMenu(const Stage2Context& ctx);

   private:
    /** 打印带序号的菜单项。 */
    static void printMenu(std::string_view title,
                          const std::vector<std::string>& options,
                          int startIndex = 1);

    /** 打印分隔线。 */
    static void printSeparator(std::string_view separator = "-",
                               int length = 20);

    /** 打印当前登录状态。 */
    static void printLoginStatus(const Stage2Context& ctx);

    /** 返回角色显示文本。 */
    static const char* roleText(Stage2Role role);
};

}  // namespace exf
