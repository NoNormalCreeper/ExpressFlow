#pragma once

#include <string_view>

namespace exf {

/**
 * Validator 提供服务层使用的简单输入校验工具。
 */
class Validator {
public:
    /**
     * 判断值是否包含非空白内容。
     */
    static bool isNonEmpty(std::string_view value);

    /**
     * 判断金额是否为正数。
     */
    static bool isPositiveAmount(double amount);
};

} // namespace exf
