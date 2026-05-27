#include "exf/service/ParcelService.hpp"
#include "exf/util/IdGenerator.hpp"
#include "exf/util/TimeUtil.hpp"

namespace exf {

bool ParcelQuery::matches(const Parcel& parcel) const {
    if (id.has_value() && parcel.id() != *id) {
        return false;
    }
    if (senderUsername.has_value() &&
        parcel.senderUsername() != *senderUsername) {
        return false;
    }
    if (receiverUsername.has_value() &&
        parcel.receiverUsername() != *receiverUsername) {
        return false;
    }
    if (status.has_value() && parcel.status() != *status) {
        return false;
    }
    if (!TimeUtil::isTimestampWithinRange(parcel.sentAt(), sentFrom, sentTo)) {
        return false;
    }

    return true;
}

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

    const auto fee = util::Money::from_double(stage1ParcelPrice());
    const auto debited =
        users_
            .modifyUser(senderUsername,
                        [fee](User& user) { return user.account().debit(fee); })
            .value_or(false);
    if (!debited) {
        return {"", ParcelServiceError::InsufficientBalance};
    }

    if (!admins_.modifyAdmin(
            [fee](Admin& admin) { admin.account().credit(fee); })) {
        return {"", ParcelServiceError::AdminNotFound};
    }

    const auto id = IdGenerator::newParcelId();
    const auto sentAt = TimeUtil::nowString();

    const auto parcel = Parcel::createNew(id, senderUsername, receiverUsername,
                                          description, sentAt, fee);
    parcels_.createParcel(parcel);

    return {parcel.id(), ParcelServiceError::Nil};
}

ParcelServiceError ParcelService::signParcel(
    const std::string& receiverUsername,
    const std::string& parcelId) {
    const Parcel* parcel = parcels_.findParcel(parcelId);
    if (parcel == nullptr) {
        return ParcelServiceError::ParcelNotFound;
    }
    if (parcel->receiverUsername() != receiverUsername) {
        return ParcelServiceError::NotReceiver;
    }
    if (parcel->status() == ParcelStatus::Signed) {
        return ParcelServiceError::AlreadySigned;
    }

    Parcel signedParcel = *parcel;
    signedParcel.markSigned(TimeUtil::nowString());
    parcels_.updateParcel(parcelId, signedParcel);
    return ParcelServiceError::Nil;
}

bool ParcelService::matchesUserView(const Parcel& parcel,
                                    const std::string& username,
                                    UserParcelView view) {
    switch (view) {
        case UserParcelView::Sent:
            return parcel.senderUsername() == username;
        case UserParcelView::Received:
            return parcel.receiverUsername() == username;
        case UserParcelView::Related:
            return parcel.senderUsername() == username ||
                   parcel.receiverUsername() == username;
        case UserParcelView::WaitingForSign:
            return parcel.receiverUsername() == username &&
                   parcel.status() == ParcelStatus::WaitingForSign;
    }

    return false;
}

std::vector<Parcel> ParcelService::queryUserParcels(
    const std::string& username,
    UserParcelView view,
    const ParcelQuery& query) const {
    std::vector<Parcel> result;
    for (const auto& parcel : parcels_.listAll()) {
        if (!matchesUserView(parcel, username, view)) {
            continue;
        }
        if (query.matches(parcel)) {
            result.push_back(parcel);
        }
    }

    return result;
}

std::vector<Parcel> ParcelService::queryAdminParcels(
    const ParcelQuery& query) const {
    return queryParcels(query);
}

std::vector<Parcel> ParcelService::queryParcels(
    const ParcelQuery& query) const {
    std::vector<Parcel> result;
    for (const auto& parcel : parcels_.listAll()) {
        if (query.matches(parcel)) {
            result.push_back(parcel);
        }
    }

    return result;
}

}  // namespace exf
