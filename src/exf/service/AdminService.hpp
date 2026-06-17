#pragma once

#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/UserRepository.hpp"
#include "exf/util/Money.hpp"

#include <string>
#include <vector>

namespace exf {

/**
 * AdminService 负责阶段 1 的管理员业务操作。
 */
class AdminService {
   public:
    /**
     * 创建管理员服务。
     */
    explicit AdminService(UserRepository& users);
    AdminService(UserRepository& users, AdminRepository& admins);

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;

    /**
     * 返回当前所有注册用户，供管理员查看。
     */
    std::vector<User> listUsers() const;

    /**
     * 校验管理员登录。
     */
    bool loginAdmin(const std::string& username,
                    const std::string& password) const;

    /**
     * 返回管理员账户余额。
     */
    util::Money getBalance() const;

   private:
    UserRepository& users_;
    AdminRepository* admins_ = nullptr;
};

}  // namespace exf
