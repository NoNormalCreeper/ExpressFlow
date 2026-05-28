#pragma once

#include "exf/repository/UserRepository.hpp"

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

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;

    /**
     * 返回当前所有注册用户，供管理员查看。
     */
    std::vector<User> listUsers() const;

   private:
    UserRepository& users_;
};

}  // namespace exf
