#pragma once

#include "exf/repository/UserRepository.hpp"

#include <string>

namespace exf {

enum class UserServiceError {
    Nil,
    UserAlreadyExists,
    UserNotFound,
    IncorrectPassword,
    InvalidInitialBalance
};

/**
 * UserService 负责后续用户资料和余额操作。
 */
class UserService {
   public:
    /**
     * 创建服务占位对象。
     */
    UserService(UserRepository& users) : users_(users) {};

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;

    UserServiceError registerUser(const std::string& username,
                                  const std::string& name,
                                  const std::string& phone,
                                  const std::string& password,
                                  const std::string& address,
                                  double initialBalance = 0.0);

    UserServiceError loginUser(const std::string& username,
                               const std::string& password) const;

    UserServiceError updatePassword(const std::string& username,
                                    const std::string& oldPassword,
                                    const std::string& newPassword);

   private:
    UserRepository& users_;
};

}  // namespace exf
