#include "exf/util/Validator.hpp"

#include "exf/util/StringUtil.hpp"

namespace exf {

// 检查去掉空白后是否仍有内容。
bool Validator::isNonEmpty(std::string_view value) {
    return !StringUtil::trim(value).empty();
}

// 检查金额类数值是否为正数。
bool Validator::isPositiveAmount(double amount) {
    return amount > 0.0;
}

}  // namespace exf
