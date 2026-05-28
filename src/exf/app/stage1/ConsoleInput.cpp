#include "ConsoleInput.hpp"

#include <exception>
#include <iostream>

#include "exf/util/TimeUtil.hpp"

namespace exf {
std::string ConsoleInput::promptLine(std::string_view label) {
    std::cout << label << "> ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

std::string ConsoleInput::promptNonEmpty(std::string_view label) {
    return promptIf(
        label, [](const std::string& input) { return !input.empty(); },
        "输入不能为空，请重新输入。");
}

std::optional<std::string> ConsoleInput::promptOptionalText(
    std::string_view label) {
    auto input = promptLine(label);
    if (input.empty()) {
        return std::nullopt;
    }
    return input;
}

std::optional<std::string> ConsoleInput::promptOptionalTimestamp(
    std::string_view label) {
    while (true) {
        auto input = promptLine(label);
        if (input.empty()) {
            return std::nullopt;
        }

        try {
            (void)TimeUtil::formatTimestamp(input);
            return input;
        } catch (const std::exception&) {
            std::cout << "请输入有效时间，格式为 YYYY-MM-DD "
                         "HH:MM:SS；留空表示不限制。\n";
        }
    }
}

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

const std::regex exf::ConsoleInput::passwordRegex{R"(.{6,})"};

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

int ConsoleInput::promptChoice(std::string_view label, int min, int max) {
    while (true) {
        std::string input = promptLine(label);
        try {
            int choice = std::stoi(input);
            if (choice >= min && choice <= max) {
                return choice;
            }
            std::cout << "请输入一个介于 " << min << " 和 " << max
                      << " 之间的数字。\n";
        } catch (const std::invalid_argument&) {
            std::cout << "输入格式错误，请输入一个有效的数字。\n";
        } catch (const std::out_of_range&) {
            std::cout << "输入的数字过大或过小，请重新输入。\n";
        }
    }
}

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
