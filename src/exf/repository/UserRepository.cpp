#include "exf/repository/UserRepository.hpp"

namespace exf {

// 创建用户仓储占位对象。
UserRepository::UserRepository() = default;

// 表示占位对象可构造。
bool UserRepository::isReady() const
{
    return true;
}

} // namespace exf
