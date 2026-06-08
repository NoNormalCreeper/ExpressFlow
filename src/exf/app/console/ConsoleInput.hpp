#pragma once

#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

#include "exf/util/Money.hpp"

namespace exf {

/**
 * ConsoleInput 提供控制台输入读取和校验工具。
 */
class ConsoleInput {
   public:
    static std::string promptLine(std::string_view label);
    static std::string promptNonEmpty(std::string_view label);
    static std::optional<std::string> promptOptionalText(
        std::string_view label);
    static std::optional<std::string> promptOptionalTimestamp(
        std::string_view label);
    static util::Money promptNonNegativeMoney(std::string_view label);
    static std::string promptRegex(
        std::string_view label,
        const std::regex& pattern,
        std::string_view errorMessage = "输入格式错误");
    static std::string promptIf(
        std::string_view label,
        const std::function<bool(const std::string&)>& validator,
        std::string_view errorMessage = "输入不合法");
    static int promptChoice(std::string_view label, int min, int max);

    static const std::regex passwordRegex;
};

}  // namespace exf
