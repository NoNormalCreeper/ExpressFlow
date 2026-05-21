#include "exf/domain/User.hpp"

#include <utility>

namespace exf {

// 创建空用户占位对象。
User::User() : account_(0.0) {}

// 保存用户身份信息和余额。
User::User(std::string username,
           std::string name,
           std::string phone,
           std::string password,
           std::string address,
           double initialBalance)
    : username_(std::move(username)),
      name_(std::move(name)),
      phone_(std::move(phone)),
      password_(std::move(password)),
      address_(std::move(address)),
      account_(initialBalance) {}

// 返回唯一用户名。
const std::string& User::username() const {
    return username_;
}

// 返回用户真实姓名。
const std::string& User::name() const {
    return name_;
}

// 返回手机号。
const std::string& User::phone() const {
    return phone_;
}

// 返回密码占位值。
const std::string& User::password() const {
    return password_;
}

// 返回收件地址。
const std::string& User::address() const {
    return address_;
}

// 返回用户账户。
const Account& User::account() const {
    return account_;
}

// 替换已保存的密码。
void User::setPassword(std::string password) {
    password_ = std::move(password);
}

}  // namespace exf
