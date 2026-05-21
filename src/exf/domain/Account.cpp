#include "exf/domain/Account.hpp"

namespace exf {

// 保存新账户的初始余额。
// Account::Account(exf::util::Money initialBalance) : balance_(initialBalance) {}

// 返回账户余额。
exf::util::Money Account::balance() const {
    return balance_;
}

// 检查余额是否足够支付非负金额。
bool Account::canAfford(exf::util::Money amount) const {
    return amount >= exf::util::Money(0) && balance_ >= amount;
}

// 向余额增加正数金额。
void Account::credit(exf::util::Money amount) {
    if (amount > exf::util::Money(0)) {
        balance_ += amount;
    }
}

// 余额足够时扣除正数金额。
bool Account::debit(exf::util::Money amount) {
    if (!canAfford(amount)) {
        return false;
    }

    balance_ -= amount;
    return true;
}

}  // namespace exf
