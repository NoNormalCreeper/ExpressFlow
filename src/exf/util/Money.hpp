#pragma once

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>

namespace exf::util {

/**
 * Money 用整数保存金额，避免直接用 double 累积余额误差。
 */
class Money {
   public:
    /** 创建 0 元金额。 */
    constexpr Money() noexcept : value_(0) {}

    /** 用内部整数值创建金额。 */
    constexpr explicit Money(int64_t raw_value) noexcept : value_(raw_value) {}

    /** 从元为单位的小数金额创建 Money。 */
    static constexpr Money from_double(double value) noexcept {
        const double scaled = value * 10000.0;
        return Money(
            static_cast<int64_t>(scaled >= 0.0 ? scaled + 0.5 : scaled - 0.5));
    }

    /** 转换为元为单位的小数金额。 */
    constexpr double to_double() const noexcept {
        return static_cast<double>(value_) / 10000.0;
    }

    /** 返回内部整数值。 */
    constexpr int64_t raw_value() const noexcept { return value_; }

    /** 从字符串金额创建 Money。 */
    static Money from_string(const std::string& value) {
        return from_double(std::stod(value));
    }

    /** 返回两笔金额相加的结果。 */
    constexpr Money operator+(const Money& rhs) const noexcept {
        return Money(value_ + rhs.value_);
    }

    /** 返回两笔金额相减的结果。 */
    constexpr Money operator-(const Money& rhs) const noexcept {
        return Money(value_ - rhs.value_);
    }

    /** 返回金额乘以倍数的结果。 */
    constexpr Money operator*(double rhs) const noexcept {
        return Money(static_cast<int64_t>(value_ * rhs));
    }

    /** 返回金额除以倍数的结果。 */
    constexpr Money operator/(double rhs) const noexcept {
        return Money(static_cast<int64_t>(value_ / rhs));
    }

    /** 将另一笔金额加到当前对象。 */
    constexpr Money& operator+=(const Money& rhs) noexcept {
        value_ += rhs.value_;
        return *this;
    }

    /** 从当前对象扣减另一笔金额。 */
    constexpr Money& operator-=(const Money& rhs) noexcept {
        value_ -= rhs.value_;
        return *this;
    }

    /** 将当前金额乘以倍数。 */
    constexpr Money& operator*=(double rhs) noexcept {
        value_ = static_cast<int64_t>(value_ * rhs);
        return *this;
    }

    /** 将当前金额除以倍数。 */
    constexpr Money& operator/=(double rhs) noexcept {
        value_ = static_cast<int64_t>(value_ / rhs);
        return *this;
    }

    /** 判断两笔金额是否相等。 */
    constexpr bool operator==(const Money& rhs) const noexcept {
        return value_ == rhs.value_;
    }

    /** 判断两笔金额是否不相等。 */
    constexpr bool operator!=(const Money& rhs) const noexcept {
        return value_ != rhs.value_;
    }

    /** 判断当前金额是否小于另一笔金额。 */
    constexpr bool operator<(const Money& rhs) const noexcept {
        return value_ < rhs.value_;
    }

    /** 判断当前金额是否大于另一笔金额。 */
    constexpr bool operator>(const Money& rhs) const noexcept {
        return value_ > rhs.value_;
    }

    /** 判断当前金额是否小于等于另一笔金额。 */
    constexpr bool operator<=(const Money& rhs) const noexcept {
        return value_ <= rhs.value_;
    }

    /** 判断当前金额是否大于等于另一笔金额。 */
    constexpr bool operator>=(const Money& rhs) const noexcept {
        return value_ >= rhs.value_;
    }

   private:
    /** 内部金额整数值，1 元对应 10000。 */
    int64_t value_;
};

/**
 * 将 Money 格式化为两位小数字符串。
 */
inline std::string to_string(const Money& money) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss.precision(2);
    oss << money.to_double();
    return oss.str();
}

/**
 * 将 Money 输出为两位小数。
 */
inline std::ostream& operator<<(std::ostream& os, const Money& money) {
    return os << to_string(money);
}

}  // namespace exf::util
