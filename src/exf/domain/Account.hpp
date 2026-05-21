#pragma once

namespace exf {

/**
 * Account 保存用户或管理员的简单余额。
 */
class Account {
   public:
    /**
     * 用指定初始余额创建账户。
     */
    explicit Account(double initialBalance = 0.0);

    /**
     * 返回当前余额，不修改账户。
     */
    double balance() const;

    /**
     * 判断账户能否支付指定金额。
     */
    bool canAfford(double amount) const;

    /**
     * 向余额增加正数金额。
     */
    void credit(double amount);

    /**
     * 尝试从余额扣除正数金额。
     */
    bool debit(double amount);

   private:
    /** 当前账户余额。 */
    double balance_;
};

}  // namespace exf
