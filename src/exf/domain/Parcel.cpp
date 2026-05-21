#include "exf/domain/Parcel.hpp"

#include <utility>

namespace exf {

// 创建空包裹占位对象。
Parcel::Parcel() : status_(ParcelStatus::WaitingForSign) {}

// 保存阶段 1 的包裹数据。
Parcel::Parcel(std::string id,
               std::string senderUsername,
               std::string receiverUsername,
               std::string description,
               std::string sentAt,
               ParcelStatus status)
    : id_(std::move(id)),
      senderUsername_(std::move(senderUsername)),
      receiverUsername_(std::move(receiverUsername)),
      description_(std::move(description)),
      sentAt_(std::move(sentAt)),
      status_(status) {}

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

// 返回包裹状态。
ParcelStatus Parcel::status() const {
    return status_;
}

// 将包裹标记为已签收，并保存签收时间。
void Parcel::markSigned(std::string receivedAt) {
    receivedAt_ = std::move(receivedAt);
    status_ = ParcelStatus::Signed;
}

}  // namespace exf
