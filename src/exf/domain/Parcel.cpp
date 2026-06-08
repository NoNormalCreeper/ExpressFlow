#include "exf/domain/Parcel.hpp"

#include <utility>

namespace exf {

// 创建空包裹占位对象。
Parcel::Parcel()
    : itemType_(ParcelItemType::Standard),
      itemAmount_(1.0),
      status_(ParcelStatus::WaitingForSign) {}

// 保存阶段 1 的包裹数据。
Parcel::Parcel(std::string id,
               std::string senderUsername,
               std::string receiverUsername,
               std::string description,
               std::string sentAt,
               std::string receivedAt,
               util::Money fee,
               ParcelStatus status,
               ParcelItemType itemType,
               double itemAmount,
               std::string courierUsername,
               std::string pickedAt)
    : id_(std::move(id)),
      senderUsername_(std::move(senderUsername)),
      receiverUsername_(std::move(receiverUsername)),
      fee_(std::move(fee)),
      itemType_(itemType),
      itemAmount_(itemAmount),
      description_(std::move(description)),
      sentAt_(std::move(sentAt)),
      receivedAt_(std::move(receivedAt)),
      courierUsername_(std::move(courierUsername)),
      pickedAt_(std::move(pickedAt)),
      status_(status) {}

Parcel Parcel::createNew(std::string id,
                         std::string senderUsername,
                         std::string receiverUsername,
                         std::string description,
                         std::string sentAt,
                         util::Money fee) {
    return {std::move(id),
            std::move(senderUsername),
            std::move(receiverUsername),
            std::move(description),
            std::move(sentAt),
            "",
            std::move(fee),
            ParcelStatus::WaitingForSign};
}

Parcel Parcel::createWaitingForPickup(std::string id,
                                      std::string senderUsername,
                                      std::string receiverUsername,
                                      std::string description,
                                      std::string sentAt,
                                      util::Money fee,
                                      ParcelItemType itemType,
                                      double itemAmount) {
    return {std::move(id),
            std::move(senderUsername),
            std::move(receiverUsername),
            std::move(description),
            std::move(sentAt),
            "",
            std::move(fee),
            ParcelStatus::WaitingForPickup,
            itemType,
            itemAmount};
}

// 返回包裹单号。
const std::string& Parcel::id() const {
    return id_;
}

// 返回寄件人用户名。
const std::string& Parcel::senderUsername() const {
    return senderUsername_;
}

// 返回收件人用户名。
const std::string& Parcel::receiverUsername() const {
    return receiverUsername_;
}

// 返回包裹描述。
const std::string& Parcel::description() const {
    return description_;
}

// 返回寄件时间。
const std::string& Parcel::sentAt() const {
    return sentAt_;
}

// 返回签收时间。
const std::string& Parcel::receivedAt() const {
    return receivedAt_;
}

const util::Money& Parcel::fee() const {
    return fee_;
}

ParcelItemType Parcel::itemType() const {
    return itemType_;
}

double Parcel::itemAmount() const {
    return itemAmount_;
}

const std::string& Parcel::courierUsername() const {
    return courierUsername_;
}

const std::string& Parcel::pickedAt() const {
    return pickedAt_;
}

// 返回包裹状态。
ParcelStatus Parcel::status() const {
    return status_;
}

void Parcel::assignCourier(std::string courierUsername) {
    courierUsername_ = std::move(courierUsername);
}

void Parcel::markPickedUp(std::string pickedAt) {
    pickedAt_ = std::move(pickedAt);
    status_ = ParcelStatus::WaitingForSign;
}

// 将包裹标记为已签收，并保存签收时间。
void Parcel::markSigned(std::string receivedAt) {
    receivedAt_ = std::move(receivedAt);
    status_ = ParcelStatus::Signed;
}

}  // namespace exf
