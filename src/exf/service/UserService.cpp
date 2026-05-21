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

}  // namespace exf
