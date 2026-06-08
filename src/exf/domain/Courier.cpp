#include "exf/domain/Courier.hpp"

#include <utility>

namespace exf {

// 创建空快递员占位对象。
Courier::Courier() : account_() {}

// 保存快递员身份信息和余额。
Courier::Courier(std::string username,
                 std::string name,
                 std::string phone,
                 std::string password,
                 util::Money initialBalance)
    : username_(std::move(username)),
      name_(std::move(name)),
      phone_(std::move(phone)),
      password_(std::move(password)),
      account_(initialBalance) {}

// 返回唯一快递员用户名。
const std::string& Courier::username() const {
    return username_;
}

// 返回快递员姓名。
const std::string& Courier::name() const {
    return name_;
}

// 返回快递员手机号。
const std::string& Courier::phone() const {
    return phone_;
}

// 返回快递员登录密码。
const std::string& Courier::password() const {
    return password_;
}

// 返回快递员账户。
Account& Courier::account() {
    return account_;
}

const Account& Courier::account() const {
    return account_;
}

// 替换快递员登录密码。
void Courier::setPassword(std::string password) {
    password_ = std::move(password);
}

}  // namespace exf
