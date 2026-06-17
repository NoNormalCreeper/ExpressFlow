#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace exf {

/**
 * ConsoleMenu 提供通用控制台菜单渲染和选择能力。
 */
class ConsoleMenu {
   public:
    /**
     * 显示菜单并读取选择。
     */
    static int show(std::string_view title,
                    const std::vector<std::string>& options,
                    std::string_view footer = "",
                    std::string_view prompt = "请选择一个选项");

    /**
     * 仅打印菜单内容，不读取输入。
     */
    static void print(std::string_view title,
                      const std::vector<std::string>& options,
                      int startIndex = 1);

    /**
     * 打印分隔线。
     */
    static void printSeparator(std::string_view separator = "-",
                               int length = 20);
};

}  // namespace exf
