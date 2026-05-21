#pragma once

#include <string>

namespace exf {

/**
 * TimeUtil 统一处理领域记录的简单时间格式。
 */
class TimeUtil {
public:
    /**
     * 返回当前时间的简单时间戳字符串。
     */
    static std::string nowString();
};

} // namespace exf
