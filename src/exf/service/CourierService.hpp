#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "exf/repository/CourierRepository.hpp"
#include "exf/util/Money.hpp"

namespace exf {

enum class CourierServiceError {
    Nil,
    CourierAlreadyExists,
    CourierNotFound,
    IncorrectPassword,
    InvalidInitialBalance,
    InvalidName,
    InvalidPhone,
    InvalidPassword
};

/**
 * CourierService 负责阶段 2 快递员账号和列表管理。
 */
class CourierService {
   public:
    /**
     * 使用快递员仓储创建服务。
     */
    explicit CourierService(CourierRepository& couriers);

    /**
     * 返回服务对象是否可用。
     */
    bool isReady() const;

    /**
     * 添加快递员。
     */
    CourierServiceError addCourier(const std::string& username,
                                   const std::string& name,
                                   const std::string& phone,
                                   const std::string& password,
                                   double initialBalance = 0.0);

    /**
     * 删除快递员。
     */
    CourierServiceError deleteCourier(std::string_view username);

    /**
     * 校验快递员登录。
     */
    CourierServiceError loginCourier(const std::string& username,
                                     const std::string& password) const;

    /**
     * 校验旧密码后更新快递员密码。
     */
    CourierServiceError updatePassword(const std::string& username,
                                       const std::string& oldPassword,
                                       const std::string& newPassword);

    /**
     * 返回所有快递员。
     */
    std::vector<Courier> listCouriers() const;

    /**
     * 返回快递员余额。
     */
    util::Money getBalance(std::string_view username) const;

   private:
    /** 快递员仓储。 */
    CourierRepository& couriers_;
};

}  // namespace exf
