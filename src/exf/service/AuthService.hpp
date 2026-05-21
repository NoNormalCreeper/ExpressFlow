#pragma once

#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/UserRepository.hpp"
namespace exf {

/**
 * AuthService 负责后续注册、登录和改密流程。
 */
class AuthService {
   public:
    /**
     * 创建服务占位对象。
     */
    AuthService(UserRepository& users, AdminRepository& admins): users_(users), admins_(admins) {}

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;

   private:
    UserRepository& users_;
    AdminRepository& admins_;
};

}  // namespace exf
