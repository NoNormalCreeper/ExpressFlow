#include "exf/service/ParcelService.hpp"
#include "exf/util/IdGenerator.hpp"
#include "exf/util/TimeUtil.hpp"

namespace exf {

// 创建包裹服务占位对象。
// ParcelService::ParcelService() = default;

// 返回阶段 1 要求的固定价格。
double ParcelService::stage1ParcelPrice() const {
    return 15.0;
}

std::tuple<std::string, ParcelServiceError> ParcelService::sendParcel(
    const std::string& senderUsername,
    const std::string& receiverUsername,
    const std::string& description) {
    if (!users_.userExists(senderUsername)) {
        return {"", ParcelServiceError::SenderNotFound};
    }
    if (!users_.userExists(receiverUsername)) {
        return {"", ParcelServiceError::ReceiverNotFound};
    }
    if (description.empty()) {
        return {"", ParcelServiceError::InvalidDescription};
    }

    const auto id = IdGenerator::newParcelId();
    const auto sentAt = TimeUtil::nowString();
    const auto fee = stage1ParcelPrice();
    if (!users_.modifyUser(senderUsername, [fee](User& user) {
            return user.account().debit(fee);
        })) {
        return {"", ParcelServiceError::InsufficientBalance};
    }
    // 占位，实现 admin 相关之后打钱

    const auto parcel =
        Parcel::createNew(id, senderUsername, receiverUsername, description,
                          sentAt, util::Money::from_double(fee));
    parcels_.createParcel(parcel);

    return {parcel.id(), ParcelServiceError::Nil};
}

ParcelServiceError ParcelService::signParcel(
    const std::string& receiverUsername,
    const std::string& parcelId) {}
std::vector<Parcel> ParcelService::queryUserParcels(
    const std::string& username,
    UserParcelView view,
    const ParcelQuery& query) const {}
std::vector<Parcel> ParcelService::queryAdminParcels(
    const ParcelQuery& query) const {}

}  // namespace exf
