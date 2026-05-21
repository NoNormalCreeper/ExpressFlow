#pragma once

#include <string>

namespace exf {

/**
 * IdGenerator 为后续包裹记录创建简单标识。
 */
class IdGenerator {
   public:
    /**
     * 返回新的包裹标识。
     */
    static std::string nextParcelId();
};

}  // namespace exf
