#include "exf/util/IdGenerator.hpp"
#include "TimeUtil.hpp"

#include <atomic>
#include <string>
#include <format>

namespace exf {

// 创建进程内唯一的包裹标识。
std::string IdGenerator::nextParcelId() {
    static std::atomic<unsigned long> counter{0};
    const unsigned long value = ++counter;
    return std::to_string(value);
}

std::string IdGenerator::newParcelId() {
    return std::format("P{}-{}", TimeUtil::nowMillisecondString(), nextParcelId());
}

}  // namespace exf
