#include "exf/service/UserService.hpp"

namespace exf {

// 创建用户服务占位对象。
// UserService::UserService() = default;

// 表示占位对象可构造。
bool UserService::isReady() const {
    return true;
}

UserServiceError UserService::registerUser(const std::string& username,
                                           const std::string& name,
                                           const std::string& phone,
                                           const std::string& password,
                                           const std::string& address,
                                           double initialBalance) {
    if (users_.userExists(username)) {
        return UserServiceError::UserAlreadyExists;
    }

    if (initialBalance < 0.0) {
        return UserServiceError::InvalidInitialBalance;
    }

    User newUser(username, name, phone, password, address,
                 util::Money::from_double(initialBalance));
    users_.createUser(newUser);

    return UserServiceError::Nil;
}

UserServiceError UserService::loginUser(const std::string& username,
                                        const std::string& password) const {
    const User* user = users_.findUser(username);
    if (!user) {
        return UserServiceError::UserNotFound;
    }
    if (user->password() != password) {
        return UserServiceError::IncorrectPassword;
    }
    return UserServiceError::Nil;
}

UserServiceError UserService::updatePassword(const std::string& username,
                                             const std::string& oldPassword,
                                             const std::string& newPassword) {
    const User* user = users_.findUser(username);
    if (!user) {
        return UserServiceError::UserNotFound;
    }
    if (user->password() != oldPassword) {
        return UserServiceError::IncorrectPassword;
    }

    User updatedUser = *user;
    updatedUser.setPassword(newPassword);
    users_.updateUser(username, updatedUser);

    return UserServiceError::Nil;
}
util::Money UserService::getBalance(const std::string_view username) const {
    auto user = users_.findUser(username);
    return user->account().balance();
}

UserAccountError UserService::topUpBalance(const std::string_view username,
                                           const util::Money& amount) {
    // 走 copy-modify-update，不直接修改 repo 内部对象
    auto user = users_.findUser(username);
    if (user == nullptr) {
        return UserAccountError::UserNotFound;
    }

    auto updated = *user;
    updated.account().credit(amount);
    users_.updateUser(username, updated);

    return UserAccountError::Nil;
}

UserAccountError UserService::payFromBalance(const std::string_view username,
                                             const util::Money& amount) {
    auto user = users_.findUser(username);
    if (user == nullptr) {
        return UserAccountError::UserNotFound;
    }

    auto updated = *user;

    if (!updated.account().debit(amount)) {
        return UserAccountError::InsufficientBalance;
    }
    users_.updateUser(username, updated);

    return UserAccountError::Nil;
}

}  // namespace exf
