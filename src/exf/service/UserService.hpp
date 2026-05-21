#pragma once

#include "exf/repository/UserRepository.hpp"
namespace exf {

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

   private:
    UserRepository& users_;
};

}  // namespace exf
