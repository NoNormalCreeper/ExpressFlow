#pragma once

#include <string>
#include <string_view>

namespace exf {

/**
 * StringUtil 提供核心层通用的小型字符串工具。
 */
class StringUtil {
   public:
    /**
     * 返回去掉首尾空白后的输入副本。
     */
    static std::string trim(std::string_view value);
};

}  // namespace exf
