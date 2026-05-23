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

    // 毫秒时间戳+进程内自增 ID
    static std::string newParcelId();
};

}  // namespace exf
