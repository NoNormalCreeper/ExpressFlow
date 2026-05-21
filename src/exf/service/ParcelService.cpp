#include "exf/service/ParcelService.hpp"

namespace exf {

// 创建包裹服务占位对象。
ParcelService::ParcelService() = default;

// 返回阶段 1 要求的固定价格。
double ParcelService::stage1ParcelPrice() const {
    return 15.0;
}

}  // namespace exf
