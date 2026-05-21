#pragma once

#include <string>

namespace exf {

/**
 * TimeUtil centralizes simple time formatting for domain records.
 */
class TimeUtil {
public:
    /**
     * Returns the current time as a simple timestamp string.
     */
    static std::string nowString();
};

} // namespace exf
