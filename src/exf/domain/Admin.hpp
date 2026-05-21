#pragma once

#include "exf/domain/Account.hpp"

#include <string>

namespace exf {

/**
 * Admin 表示阶段 1 的物流公司管理员。
 */
class Admin {
   public:
    /**
     * 为仓储占位创建空管理员对象。
     */
    Admin();

    /**
     * 用身份信息和初始余额创建管理员。
     */
    Admin(std::string username,
          std::string name,
          std::string password,
          double initialBalance);

    /**
     * 返回唯一管理员用户名。
     */
    const std::string& username() const;

    /**
     * 返回管理员显示名。
     */
    const std::string& name() const;

    /**
     * 返回管理员密码占位值。
     */
    const std::string& password() const;

    /**
     * 返回管理员账户。
     */
    const Account& account() const;

   private:
    /** 唯一管理员用户名。 */
    std::string username_;

    /** 管理员显示名。 */
    std::string name_;

    /** 后续认证用密码占位值。 */
    std::string password_;

    /** 阶段 1 接收运费的账户。 */
    Account account_;
};

}  // namespace exf
