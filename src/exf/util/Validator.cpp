#include "exf/util/Validator.hpp"

#include "exf/util/StringUtil.hpp"

namespace exf {

// Checks that text remains after trimming whitespace.
bool Validator::isNonEmpty(std::string_view value)
{
    return !StringUtil::trim(value).empty();
}

// Checks that money-like values are positive.
bool Validator::isPositiveAmount(double amount)
{
    return amount > 0.0;
}

} // namespace exf
