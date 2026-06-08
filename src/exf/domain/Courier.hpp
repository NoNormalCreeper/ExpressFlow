#pragma once

#include "exf/domain/Account.hpp"
#include "exf/util/Money.hpp"

#include <string>

namespace exf {

/**
 * Courier 表示阶段 2 中物流公司的快递员。
 */
class Courier {
   public:
    /**
     * 为仓储占位创建空快递员对象。
     */
    Courier();

    /**
     * 用身份信息和初始余额创建快递员。
     */
    Courier(std::string username,
            std::string name,
            std::string phone,
            std::string password,
            util::Money initialBalance);

    /**
     * 返回唯一快递员用户名。
     */
    const std::string& username() const;

    /**
     * 返回快递员姓名。
     */
    const std::string& name() const;

    /**
     * 返回快递员手机号。
     */
    const std::string& phone() const;

    /**
     * 返回快递员登录密码。
     */
    const std::string& password() const;

    /**
     * 返回快递员账户。
     */
    Account& account();
    const Account& account() const;

    /**
     * 替换快递员登录密码。
     */
    void setPassword(std::string password);

   private:
    /** 快递员唯一用户名。 */
    std::string username_;

    /** 快递员姓名。 */
    std::string name_;

    /** 快递员手机号。 */
    std::string phone_;

    /** 快递员登录密码。 */
    std::string password_;

    /** 快递员收益账户。 */
    Account account_;
};

}  // namespace exf
