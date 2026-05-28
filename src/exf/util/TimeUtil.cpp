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
    using namespace std::chrono;
    const auto tp_utc8 = tp + displayUtcOffset();
    return std::format("{:%Y-%m-%d %H:%M:%S}", floor<seconds>(tp_utc8));
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
        if (timestamp.size() != 19 || timestamp[4] != '-' ||
            timestamp[7] != '-' || timestamp[10] != ' ' ||
            timestamp[13] != ':' || timestamp[16] != ':') {
            throw std::invalid_argument("timestamp is not valid");
        }

        auto parsePart = [timestamp](size_t pos, size_t len) {
            int part = 0;
            const auto* partBegin = timestamp.data() + pos;
            const auto* partEnd = partBegin + len;
            const auto [partPtr, partEc] =
                std::from_chars(partBegin, partEnd, part);
            if (partEc != std::errc{} || partPtr != partEnd) {
                throw std::invalid_argument("timestamp is not valid");
            }
            return part;
        };

        const int yearValue = parsePart(0, 4);
        const int monthValue = parsePart(5, 2);
        const int dayValue = parsePart(8, 2);
        const int hourValue = parsePart(11, 2);
        const int minuteValue = parsePart(14, 2);
        const int secondValue = parsePart(17, 2);

        using namespace std::chrono;
        if (hourValue < 0 || hourValue > 23 || minuteValue < 0 ||
            minuteValue > 59 || secondValue < 0 || secondValue > 60) {
            throw std::invalid_argument("timestamp is not valid");
        }

        const year_month_day ymd{year{yearValue},
                                 month{static_cast<unsigned>(monthValue)},
                                 day{static_cast<unsigned>(dayValue)}};
        if (!ymd.ok()) {
            throw std::invalid_argument("timestamp is not valid");
        }

        const sys_days days{ymd};
        return days + hours{hourValue} + minutes{minuteValue} +
               seconds{secondValue} - displayUtcOffset();
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
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                            .count();
    return std::to_string(millis);
}

std::string TimeUtil::nowReadableString() {
    return formatTimePoint(std::chrono::system_clock::now());
}

std::string TimeUtil::formatTimestamp(std::string_view timestamp) {
    return formatTimePoint(parseTimestamp(timestamp));
}

bool TimeUtil::isTimestampWithinRange(std::string_view timestamp,
                                      const std::optional<std::string>& from,
                                      const std::optional<std::string>& to) {
    if (!from.has_value() && !to.has_value()) {
        return true;
    }

    try {
        const auto current = parseTimestamp(timestamp);
        if (from.has_value() && current < parseTimestamp(*from)) {
            return false;
        }
        if (to.has_value() && current > parseTimestamp(*to)) {
            return false;
        }
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    }
}

}  // namespace exf
