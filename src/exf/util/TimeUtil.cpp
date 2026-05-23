#include "exf/util/TimeUtil.hpp"

#include <charconv>
#include <chrono>
#include <format>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace exf {

std::string TimeUtil::formatTimePoint(
    const std::chrono::system_clock::time_point& tp) {
    return std::format("{:%Y-%m-%d %H:%M:%S}",
                       std::chrono::floor<std::chrono::seconds>(tp));
}

std::chrono::system_clock::time_point TimeUtil::parseTimestamp(
    std::string_view timestamp) {
    if (timestamp.empty()) {
        throw std::invalid_argument("timestamp is empty");
    }

    long long value = 0;
    const auto* begin = timestamp.data();
    const auto* end = begin + timestamp.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);
    if (ec != std::errc{} || ptr != end) {
        throw std::invalid_argument("timestamp is not a valid integer");
    }

    using namespace std::chrono;
    if (value >= 100000000000LL || value <= -100000000000LL) {
        return system_clock::time_point{milliseconds{value}};
    }

    return system_clock::time_point{seconds{value}};
}

// 返回纪元以来的秒数，作为稳定时间戳字符串。
std::string TimeUtil::nowString() {
    const auto now = std::chrono::system_clock::now();
    const auto seconds =
        std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch())
            .count();
    return std::to_string(seconds);
}

std::string TimeUtil::nowMillisecondString() {
    const auto now = std::chrono::system_clock::now();
    const auto millis =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch())
            .count();
    return std::to_string(millis);
}

std::string TimeUtil::nowReadableString() {
    return formatTimePoint(std::chrono::system_clock::now());
}

std::string TimeUtil::formatTimestamp(std::string_view timestamp) {
    return formatTimePoint(parseTimestamp(timestamp));
}

}  // namespace exf
