#include "exf/service/AuthService.hpp"

namespace exf {

// 创建认证服务占位对象。
AuthService::AuthService() = default;

// 表示占位对象可构造。
bool AuthService::isReady() const
{
    return true;
}

} // namespace exf
