#pragma once

#include "exf/domain/Account.hpp"

#include <string>

namespace exf {

/**
 * User 保存阶段 1 所需的用户账户信息。
 */
class User {
   public:
    /**
     * 为仓储占位创建空用户对象。
     */
    User();

    /**
     * 用身份信息和初始余额创建用户。
     */
    User(std::string username,
         std::string name,
         std::string phone,
         std::string password,
         std::string address,
         double initialBalance);

    /**
     * 返回唯一用户名。
     */
    const std::string& username() const;

    /**
     * 返回用户真实姓名。
     */
    const std::string& name() const;

    /**
     * 返回用户手机号。
     */
    const std::string& phone() const;

    /**
     * 返回后续认证逻辑使用的密码占位值。
     */
    const std::string& password() const;

    /**
     * 返回用户地址。
     */
    const std::string& address() const;

    /**
     * 返回用户账户。
     */
    const Account& account() const;

    /**
     * 替换用户密码。
     */
    void setPassword(std::string password);

   private:
    /** 平台唯一用户名。 */
    std::string username_;

    /** 用户真实姓名。 */
    std::string name_;

    /** 用户手机号。 */
    std::string phone_;

    /** 阶段 1 认证用密码占位值。 */
    std::string password_;

    /** 用户收件地址。 */
    std::string address_;

    /** 用户余额账户。 */
    Account account_;
};

}  // namespace exf
