#pragma once

#include <string>
#include <string_view>

namespace exf {

/**
 * StringUtil contains small string helpers used across the core layer.
 */
class StringUtil {
public:
    /**
     * Returns a copy of the input without leading or trailing whitespace.
     */
    static std::string trim(std::string_view value);
};

} // namespace exf
