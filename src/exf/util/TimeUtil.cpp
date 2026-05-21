#include "exf/util/TimeUtil.hpp"

#include <chrono>
#include <string>

namespace exf {

// 返回纪元以来的秒数，作为稳定时间戳字符串。
std::string TimeUtil::nowString()
{
    const auto now = std::chrono::system_clock::now();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return std::to_string(seconds);
}

} // namespace exf
