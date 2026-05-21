#include "exf/util/StringUtil.hpp"

#include <cctype>

namespace exf {

// 去掉 string_view 的首尾空白。
std::string StringUtil::trim(std::string_view value) {
    std::size_t begin = 0;
    std::size_t end = value.size();

    while (begin < end &&
           std::isspace(static_cast<unsigned char>(value[begin])) != 0) {
        ++begin;
    }

    while (end > begin &&
           std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return std::string(value.substr(begin, end - begin));
}

}  // namespace exf
