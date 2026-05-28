#include "exf/service/AdminService.hpp"

namespace exf {

// 保存管理员服务需要访问的用户仓储。
AdminService::AdminService(UserRepository& users) : users_(users) {}

// 表示占位对象可构造。
bool AdminService::isReady() const {
    return true;
}

std::vector<User> AdminService::listUsers() const {
    return users_.listAll();
}

}  // namespace exf
