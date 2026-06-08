#include "exf/service/CourierService.hpp"

#include <stdexcept>

namespace exf {

// 保存快递员仓储依赖。
CourierService::CourierService(CourierRepository& couriers)
    : couriers_(couriers) {}

// 表示服务对象可构造。
bool CourierService::isReady() const {
    return true;
}

CourierServiceError CourierService::addCourier(
    const std::string& username,
    const std::string& name,
    const std::string& phone,
    const std::string& password,
    double initialBalance) {
    if (couriers_.courierExists(username)) {
        return CourierServiceError::CourierAlreadyExists;
    }
    if (name.empty()) {
        return CourierServiceError::InvalidName;
    }
    if (phone.empty()) {
        return CourierServiceError::InvalidPhone;
    }
    if (password.empty()) {
        return CourierServiceError::InvalidPassword;
    }
    if (initialBalance < 0.0) {
        return CourierServiceError::InvalidInitialBalance;
    }

    couriers_.createCourier(Courier(username,
                                    name,
                                    phone,
                                    password,
                                    util::Money::from_double(initialBalance)));
    return CourierServiceError::Nil;
}

CourierServiceError CourierService::deleteCourier(std::string_view username) {
    if (!couriers_.courierExists(username)) {
        return CourierServiceError::CourierNotFound;
    }

    couriers_.deleteCourier(username);
    return CourierServiceError::Nil;
}

CourierServiceError CourierService::loginCourier(
    const std::string& username,
    const std::string& password) const {
    const Courier* courier = couriers_.findCourier(username);
    if (courier == nullptr) {
        return CourierServiceError::CourierNotFound;
    }
    if (courier->password() != password) {
        return CourierServiceError::IncorrectPassword;
    }
    return CourierServiceError::Nil;
}

CourierServiceError CourierService::updatePassword(
    const std::string& username,
    const std::string& oldPassword,
    const std::string& newPassword) {
    const Courier* courier = couriers_.findCourier(username);
    if (courier == nullptr) {
        return CourierServiceError::CourierNotFound;
    }
    if (courier->password() != oldPassword) {
        return CourierServiceError::IncorrectPassword;
    }
    if (newPassword.empty()) {
        return CourierServiceError::InvalidPassword;
    }

    Courier updatedCourier = *courier;
    updatedCourier.setPassword(newPassword);
    couriers_.updateCourier(username, updatedCourier);
    return CourierServiceError::Nil;
}

std::vector<Courier> CourierService::listCouriers() const {
    return couriers_.listAll();
}

util::Money CourierService::getBalance(std::string_view username) const {
    const Courier* courier = couriers_.findCourier(username);
    if (courier == nullptr) {
        throw std::runtime_error("快递员未找到，不符合约定，请检查调用");
    }
    return courier->account().balance();
}

}  // namespace exf
