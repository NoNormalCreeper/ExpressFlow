#pragma once

#include <string_view>

namespace exf {

/**
 * Validator contains simple input validation helpers for service code.
 */
class Validator {
public:
    /**
     * Returns whether the value has non-whitespace content.
     */
    static bool isNonEmpty(std::string_view value);

    /**
     * Returns whether an amount is positive.
     */
    static bool isPositiveAmount(double amount);
};

} // namespace exf
