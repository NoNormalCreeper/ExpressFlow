#pragma once

#include "exf/domain/ParcelStatus.hpp"

#include <string>

#include "exf/domain/ParcelItemType.hpp"
#include "exf/util/Money.hpp"

namespace exf {

/**
 * Parcel 保存阶段 1 中寄件人和收件人的包裹记录。
 */
class Parcel {
   public:
    /**
     * 为仓储占位创建空包裹。
     */
    Parcel();

    /**
     * 用指定状态创建包裹记录。
     */
    Parcel(std::string id,
           std::string senderUsername,
           std::string receiverUsername,
           std::string description,
           std::string sentAt,
           std::string receivedAt,
           util::Money fee,
           ParcelStatus status,
           ParcelItemType itemType = ParcelItemType::Standard,
           double itemAmount = 1.0,
           std::string courierUsername = "",
           std::string pickedAt = "");

    /** 创建包裹，给业务创建新快递用 */
    static Parcel createNew(std::string id,
                        std::string senderUsername,
                        std::string receiverUsername,
                        std::string description,
                        std::string sentAt,
                        util::Money fee);

    /**
     * 创建阶段 2 待揽收包裹，保存物品分类和计费数量。
     */
    static Parcel createWaitingForPickup(std::string id,
                                         std::string senderUsername,
                                         std::string receiverUsername,
                                         std::string description,
                                         std::string sentAt,
                                         util::Money fee,
                                         ParcelItemType itemType,
                                         double itemAmount);

    /**
     * 返回包裹单号。
     */
    const std::string& id() const;

    /**
     * 返回寄件用户的用户名。
     */
    const std::string& senderUsername() const;

    /**
     * 返回收件用户的用户名。
     */
    const std::string& receiverUsername() const;

    /**
     * 返回包裹描述。
     */
    const std::string& description() const;

    /**
     * 返回寄件时间字符串。
     */
    const std::string& sentAt() const;

    /**
     * 返回签收时间字符串。
     */
    const std::string& receivedAt() const;

    const util::Money& fee() const;

    /**
     * 返回阶段 2 快递分类。
     */
    ParcelItemType itemType() const;

    /**
     * 返回阶段 2 计费用数量，普通/易碎品表示重量，图书表示本数。
     */
    double itemAmount() const;

    /**
     * 返回分配给该包裹的快递员用户名。
     */
    const std::string& courierUsername() const;

    /**
     * 返回快递员揽收时间。
     */
    const std::string& pickedAt() const;

    /**
     * 返回当前包裹状态。
     */
    ParcelStatus status() const;

    /**
     * 记录管理员分配的快递员。
     */
    void assignCourier(std::string courierUsername);

    /**
     * 记录快递员揽收时间，并将包裹转为待签收。
     */
    void markPickedUp(std::string pickedAt);

    /**
     * 按指定签收时间将包裹标记为已签收。
     */
    void markSigned(std::string receivedAt);

   private:
    /** 包裹单号。 */
    std::string id_;

    /** 寄件人用户名。 */
    std::string senderUsername_;

    /** 收件人用户名。 */
    std::string receiverUsername_;

    /** 包裹实际运费 */
    util::Money fee_;

    /** 阶段 2 快递分类。 */
    ParcelItemType itemType_;

    /** 阶段 2 计费用数量，重量或本数。 */
    double itemAmount_;

    /** 用户填写的包裹描述。 */
    std::string description_;

    /** 包裹提交时间。 */
    std::string sentAt_;

    /** 收件人签收时间。 */
    std::string receivedAt_;

    /** 管理员分配的快递员用户名。 */
    std::string courierUsername_;

    /** 快递员揽收时间。 */
    std::string pickedAt_;

    /** 阶段 1 包裹状态。 */
    ParcelStatus status_;
};

}  // namespace exf
