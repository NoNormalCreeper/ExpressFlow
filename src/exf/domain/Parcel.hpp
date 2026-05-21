#pragma once

#include "exf/domain/ParcelStatus.hpp"

#include <string>

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
           ParcelStatus status);

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

    /**
     * 返回当前包裹状态。
     */
    ParcelStatus status() const;

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

    /** 用户填写的包裹描述。 */
    std::string description_;

    /** 包裹提交时间。 */
    std::string sentAt_;

    /** 收件人签收时间。 */
    std::string receivedAt_;

    /** 阶段 1 包裹状态。 */
    ParcelStatus status_;
};

}  // namespace exf
