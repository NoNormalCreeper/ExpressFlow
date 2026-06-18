
#pragma once

#include <functional>
#include <optional>
#include <regex>
#include <string>
#include <string_view>

#include "exf/util/Money.hpp"

namespace exf {

/**
 * ConsoleInput 封装阶段 1 控制台输入和校验。
 */
class ConsoleInput {
   public:
    /**
     * 提示用户输入一行文本。
     */
    static std::string promptLine(std::string_view label);

    /**
     * 提示用户输入非空文本。
     */
    static std::string promptNonEmpty(std::string_view label);

    /**
     * 提示用户输入可为空文本。
     */
    static std::optional<std::string> promptOptionalText(
        std::string_view label);

    /**
     * 提示用户输入可为空时间戳。
     */
    static std::optional<std::string> promptOptionalTimestamp(
        std::string_view label);

    /**
     * 提示用户输入非负金额。
     */
    static util::Money promptNonNegativeMoney(std::string_view label);

    /**
     * 提示用户输入符合正则表达式的文本。
     */
    static std::string promptRegex(
        std::string_view label,
        const std::regex& pattern,
        std::string_view errorMessage = "输入格式错误");

    /**
     * 提示用户输入符合自定义校验的文本。
     */
    static std::string promptIf(
        std::string_view label,
        const std::function<bool(const std::string&)>& validator,
        std::string_view errorMessage = "输入不合法");

    /**
     * 提示用户输入指定范围内的菜单选项。
     */
    static int promptChoice(std::string_view label, int min, int max);

    /** 密码最小长度校验正则。 */
    static const std::regex passwordRegex;
};

}  // namespace exf
