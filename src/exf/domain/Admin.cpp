#include "exf/domain/Admin.hpp"

#include <utility>

namespace exf {

// 创建空管理员占位对象。
Admin::Admin() : account_(0.0) {}

// 保存管理员身份信息和余额。
Admin::Admin(std::string username,
             std::string name,
             std::string password,
             double initialBalance)
    : username_(std::move(username)),
      name_(std::move(name)),
      password_(std::move(password)),
      account_(initialBalance) {}

// 返回唯一管理员用户名。
const std::string& Admin::username() const {
    return username_;
}

// 返回管理员显示名。
const std::string& Admin::name() const {
    return name_;
}

// 返回管理员密码占位值。
const std::string& Admin::password() const {
    return password_;
}

// 返回管理员账户。
const Account& Admin::account() const {
    return account_;
}

}  // namespace exf
