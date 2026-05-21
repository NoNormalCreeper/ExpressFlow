#include "exf/util/IdGenerator.hpp"

#include <atomic>
#include <string>

namespace exf {

// 创建进程内唯一的包裹标识。
std::string IdGenerator::nextParcelId() {
    static std::atomic<unsigned long> counter{0};
    const unsigned long value = ++counter;
    return "P" + std::to_string(value);
}

}  // namespace exf
