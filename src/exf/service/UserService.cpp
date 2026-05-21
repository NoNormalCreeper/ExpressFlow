#include "exf/service/UserService.hpp"

namespace exf {

// 创建用户服务占位对象。
UserService::UserService() = default;

// 表示占位对象可构造。
bool UserService::isReady() const
{
    return true;
}

} // namespace exf
