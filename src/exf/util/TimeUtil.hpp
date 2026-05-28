#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <chrono>

namespace exf {

/**
 * TimeUtil 统一处理领域记录的简单时间格式。
 * 数字时间戳按 Unix 时间戳处理；可读时间字符串默认按 UTC+8 解释，不包含时区后缀。
 */
class TimeUtil {
   public:
    /**
     * 返回当前时间的简单时间戳字符串。
     */
    static std::string nowString();
    static std::string nowMillisecondString();
    static std::string nowReadableString();
    static std::string formatTimestamp(std::string_view timestamp);
    static bool isTimestampWithinRange(
        std::string_view timestamp,
        const std::optional<std::string>& from = std::nullopt,
        const std::optional<std::string>& to = std::nullopt);

private:
    static constexpr std::chrono::hours displayUtcOffset() {
        return std::chrono::hours{8};
    }

    static std::string formatTimePoint(const std::chrono::system_clock::time_point& tp);
    static std::chrono::system_clock::time_point parseTimestamp(std::string_view timestamp);

};

}  // namespace exf
