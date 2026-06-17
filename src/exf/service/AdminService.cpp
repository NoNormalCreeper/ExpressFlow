#include "exf/service/AdminService.hpp"

#include <stdexcept>

namespace exf {

// 保存管理员服务需要访问的用户仓储。
AdminService::AdminService(UserRepository& users) : users_(users) {}

AdminService::AdminService(UserRepository& users, AdminRepository& admins)
    : users_(users), admins_(&admins) {}

// 表示占位对象可构造。
bool AdminService::isReady() const {
    return true;
}

std::vector<User> AdminService::listUsers() const {
    return users_.listAll();
}

bool AdminService::loginAdmin(const std::string& username,
                              const std::string& password) const {
    if (admins_ == nullptr) {
        return false;
    }

    const auto& admin = admins_->getAdmin();
    return admin.username() == username && admin.password() == password;
}

util::Money AdminService::getBalance() const {
    if (admins_ == nullptr) {
        throw std::runtime_error("管理员仓储未配置，不符合约定，请检查调用");
    }
    return admins_->getAdmin().account().balance();
}

}  // namespace exf
