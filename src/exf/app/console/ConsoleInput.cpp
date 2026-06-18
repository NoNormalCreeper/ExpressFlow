#include "exf/app/console/ConsoleInput.hpp"

#include <charconv>
#include <exception>
#include <iostream>

#include "exf/util/TimeUtil.hpp"

namespace exf {

// 提示用户输入一行文本。
std::string ConsoleInput::promptLine(std::string_view label) {
    std::cout << label << "> ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

// 提示用户输入非空文本。
std::string ConsoleInput::promptNonEmpty(std::string_view label) {
    return promptIf(
        label, [](const std::string& input) { return !input.empty(); },
        "输入不能为空，请重新输入。");
}

// 提示用户输入可为空文本。
std::optional<std::string> ConsoleInput::promptOptionalText(
    std::string_view label) {
    auto input = promptLine(label);
    if (input.empty()) {
        return std::nullopt;
    }
    return input;
}

// 提示用户输入可为空时间戳。
std::optional<std::string> ConsoleInput::promptOptionalTimestamp(
    std::string_view label) {
    static const std::regex readableTimestampRegex{
        R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})"};

    while (true) {
        auto input = promptLine(label);
        if (input.empty()) {
            return std::nullopt;
        }

        try {
            if (!std::regex_match(input, readableTimestampRegex)) {
                throw std::invalid_argument("timestamp format is not readable");
            }
            (void)TimeUtil::formatTimestamp(input);
            return input;
        } catch (const std::exception&) {
            std::cout << "请输入有效时间，格式为 YYYY-MM-DD "
                         "HH:MM:SS；留空表示不限制。\n";
        }
    }
}

// 提示用户输入非负金额。
util::Money ConsoleInput::promptNonNegativeMoney(std::string_view label) {
    while (true) {
        std::string input = promptLine(label);
        try {
            double value = std::stod(input);
            if (value >= 0.0) {
                return util::Money::from_double(value);
            }
            std::cout << "金额不能为负数，请重新输入。\n";
        } catch (const std::invalid_argument&) {
            std::cout << "输入格式错误，请输入一个有效的数字。\n";
        } catch (const std::out_of_range&) {
            std::cout << "输入的数字过大或过小，请重新输入。\n";
        }
    }
}

// 密码最小长度校验正则。
const std::regex exf::ConsoleInput::passwordRegex{R"(.{6,})"};

// 提示用户输入符合正则表达式的文本。
std::string ConsoleInput::promptRegex(std::string_view label,
                                      const std::regex& pattern,
                                      std::string_view errorMessage) {
    return promptIf(
        label,
        [&pattern](const std::string& input) {
            return std::regex_match(input, pattern);
        },
        errorMessage);
}

// 提示用户输入指定范围内的菜单选项。
int ConsoleInput::promptChoice(std::string_view label, int min, int max) {
    while (true) {
        std::string input = promptLine(label);

        auto start = input.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) {
            std::cout << "输入格式错误，请输入一个有效的数字。\n";
            continue;
        }
        auto end = input.find_last_not_of(" \t\r\n");
        std::string_view trimmedInput(input.data() + start, end - start + 1);

        int choice;
        auto [ptr, ec] =
            std::from_chars(trimmedInput.data(),
                            trimmedInput.data() + trimmedInput.size(), choice);

        if (ec == std::errc()) {
            if (ptr != trimmedInput.data() + trimmedInput.size()) {
                std::cout << "输入包含非法字符，请输入纯数字。\n";
            } else if (choice >= min && choice <= max) {
                return choice;
            } else {
                std::cout << "请输入一个介于 " << min << " 和 " << max
                          << " 之间的数字。\n";
            }
        } else if (ec == std::errc::invalid_argument) {
            std::cout << "输入格式错误，请输入一个有效的数字。\n";
        } else if (ec == std::errc::result_out_of_range) {
            std::cout << "输入的数字过大或过小，请重新输入。\n";
        }
    }
}

// 提示用户输入符合自定义校验的文本。
std::string ConsoleInput::promptIf(
    std::string_view label,
    const std::function<bool(const std::string&)>& validator,
    std::string_view errorMessage) {
    while (true) {
        std::string input = promptLine(label);
        if (validator(input)) {
            return input;
        }
        std::cout << errorMessage << '\n';
    }
}

}  // namespace exf
