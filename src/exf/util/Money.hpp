#pragma once

#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>

namespace exf::util {

class Money {
   public:
    constexpr Money() noexcept : value_(0) {}
    constexpr explicit Money(int64_t raw_value) noexcept : value_(raw_value) {}

    static constexpr Money from_double(double value) noexcept {
        const double scaled = value * 10000.0;
        return Money(
            static_cast<int64_t>(scaled >= 0.0 ? scaled + 0.5 : scaled - 0.5));
    }

    constexpr double to_double() const noexcept {
        return static_cast<double>(value_) / 10000.0;
    }

    constexpr int64_t raw_value() const noexcept { return value_; }

    static Money from_string(const std::string& value) {
        return from_double(std::stod(value));
    }

    constexpr Money operator+(const Money& rhs) const noexcept {
        return Money(value_ + rhs.value_);
    }

    constexpr Money operator-(const Money& rhs) const noexcept {
        return Money(value_ - rhs.value_);
    }

    constexpr Money operator*(double rhs) const noexcept {
        return Money(static_cast<int64_t>(value_ * rhs));
    }

    constexpr Money operator/(double rhs) const noexcept {
        return Money(static_cast<int64_t>(value_ / rhs));
    }

    constexpr Money& operator+=(const Money& rhs) noexcept {
        value_ += rhs.value_;
        return *this;
    }

    constexpr Money& operator-=(const Money& rhs) noexcept {
        value_ -= rhs.value_;
        return *this;
    }

    constexpr Money& operator*=(double rhs) noexcept {
        value_ = static_cast<int64_t>(value_ * rhs);
        return *this;
    }

    constexpr Money& operator/=(double rhs) noexcept {
        value_ = static_cast<int64_t>(value_ / rhs);
        return *this;
    }

    constexpr bool operator==(const Money& rhs) const noexcept {
        return value_ == rhs.value_;
    }

    constexpr bool operator!=(const Money& rhs) const noexcept {
        return value_ != rhs.value_;
    }

    constexpr bool operator<(const Money& rhs) const noexcept {
        return value_ < rhs.value_;
    }

    constexpr bool operator>(const Money& rhs) const noexcept {
        return value_ > rhs.value_;
    }

    constexpr bool operator<=(const Money& rhs) const noexcept {
        return value_ <= rhs.value_;
    }

    constexpr bool operator>=(const Money& rhs) const noexcept {
        return value_ >= rhs.value_;
    }

   private:
    int64_t value_;
};

inline std::string to_string(const Money& money) {
    std::ostringstream oss;
    oss.setf(std::ios::fixed, std::ios::floatfield);
    oss.precision(2);
    oss << money.to_double();
    return oss.str();
}

inline std::ostream& operator<<(std::ostream& os, const Money& money) {
    return os << to_string(money);
}

}  // namespace exf::util
