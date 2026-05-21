#include "exf/repository/ParcelRepository.hpp"

namespace exf {

// 创建包裹仓储占位对象。
ParcelRepository::ParcelRepository() = default;

// 表示占位对象可构造。
bool ParcelRepository::isReady() const {
    return true;
}

}  // namespace exf
