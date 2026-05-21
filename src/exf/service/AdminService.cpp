#include "exf/service/AdminService.hpp"

namespace exf {

// 创建管理员服务占位对象。
AdminService::AdminService() = default;

// 表示占位对象可构造。
bool AdminService::isReady() const {
    return true;
}

}  // namespace exf
