#pragma once
#include "exf/util/Money.hpp"

namespace exf {

/**
 * Account 保存用户或管理员的简单余额。
 */
class Account {
   public:
    /**
     * 用指定初始余额创建账户。
     */
    explicit Account(exf::util::Money initialBalance = exf::util::Money(0)) : balance_(initialBalance) {};

    /**
     * 返回当前余额，不修改账户。
     */
    exf::util::Money balance() const;

    /**
     * 判断账户能否支付指定金额。
     */
    bool canAfford(exf::util::Money amount) const;

    /**
     * 向余额增加正数金额。
     */
    void credit(exf::util::Money amount);

    /**
     * 尝试从余额扣除正数金额。
     */
    bool debit(exf::util::Money amount);

   private:
    /** 当前账户余额。 */
    exf::util::Money balance_;
};

}  // namespace exf
