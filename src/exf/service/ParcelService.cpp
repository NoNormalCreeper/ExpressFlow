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
    if (courierUsername.has_value() &&
        parcel.courierUsername() != *courierUsername) {
        return false;
    }
    if (status.has_value() && parcel.status() != *status) {
        return false;
    }
    if (itemType.has_value() && parcel.itemType() != *itemType) {
        return false;
    }
    if (!TimeUtil::isTimestampWithinRange(parcel.sentAt(), sentFrom, sentTo)) {
        return false;
    }

    return true;
}

std::tuple<std::string, ParcelServiceError> ParcelService::sendParcel(
    const std::string& senderUsername,
    const std::string& receiverUsername,
    const std::string& description,
    const Item& item) {
    if (!users_.userExists(senderUsername)) {
        return {"", ParcelServiceError::SenderNotFound};
    }
    if (!users_.userExists(receiverUsername)) {
        return {"", ParcelServiceError::ReceiverNotFound};
    }
    if (description.empty()) {
        return {"", ParcelServiceError::InvalidDescription};
    }
    if (!item.isValid()) {
        return {"", ParcelServiceError::InvalidItem};
    }

    const auto fee = item.getPrice();
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
    const auto parcel = Parcel::createWaitingForPickup(
        id,
        senderUsername,
        receiverUsername,
        description,
        sentAt,
        fee,
        item.type(),
        item.amount());
    parcels_.createParcel(parcel);

    return {parcel.id(), ParcelServiceError::Nil};
}

ParcelServiceError ParcelService::assignCourier(
    const std::string& parcelId,
    const std::string& courierUsername) {
    if (!couriers_.courierExists(courierUsername)) {
        return ParcelServiceError::CourierNotFound;
    }

    const Parcel* parcel = parcels_.findParcel(parcelId);
    if (parcel == nullptr) {
        return ParcelServiceError::ParcelNotFound;
    }
    if (parcel->status() != ParcelStatus::WaitingForPickup) {
        return ParcelServiceError::NotWaitingForPickup;
    }
    if (!parcel->courierUsername().empty()) {
        return ParcelServiceError::CourierAlreadyAssigned;
    }

    Parcel assignedParcel = *parcel;
    assignedParcel.assignCourier(courierUsername);
    parcels_.updateParcel(parcelId, assignedParcel);
    return ParcelServiceError::Nil;
}

ParcelServiceError ParcelService::pickupParcel(
    const std::string& courierUsername,
    const std::string& parcelId) {
    if (!couriers_.courierExists(courierUsername)) {
        return ParcelServiceError::CourierNotFound;
    }

    const Parcel* parcel = parcels_.findParcel(parcelId);
    if (parcel == nullptr) {
        return ParcelServiceError::ParcelNotFound;
    }
    if (parcel->courierUsername().empty()) {
        return ParcelServiceError::CourierNotAssigned;
    }
    if (parcel->courierUsername() != courierUsername) {
        return ParcelServiceError::NotAssignedCourier;
    }
    if (parcel->status() == ParcelStatus::WaitingForSign ||
        parcel->status() == ParcelStatus::Signed) {
        return ParcelServiceError::AlreadyPickedUp;
    }
    if (parcel->status() != ParcelStatus::WaitingForPickup) {
        return ParcelServiceError::NotWaitingForPickup;
    }

    const auto courierShare = parcel->fee() * 0.5;
    if (!admins_.getAdmin().account().canAfford(courierShare)) {
        return ParcelServiceError::AdminInsufficientBalance;
    }

    admins_.modifyAdmin([courierShare](Admin& admin) {
        admin.account().debit(courierShare);
    });
    couriers_.modifyCourier(courierUsername, [courierShare](Courier& courier) {
        courier.account().credit(courierShare);
    });

    Parcel pickedParcel = *parcel;
    pickedParcel.markPickedUp(TimeUtil::nowString());
    parcels_.updateParcel(parcelId, pickedParcel);
    return ParcelServiceError::Nil;
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
    if (parcel->status() != ParcelStatus::WaitingForSign) {
        return ParcelServiceError::NotWaitingForSign;
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

bool ParcelService::matchesCourierView(const Parcel& parcel,
                                       const std::string& courierUsername,
                                       CourierParcelView view) {
    if (parcel.courierUsername() != courierUsername) {
        return false;
    }

    switch (view) {
        case CourierParcelView::AssignedWaitingForPickup:
            return parcel.status() == ParcelStatus::WaitingForPickup;
        case CourierParcelView::PickedUpOrDelivered:
            return parcel.status() == ParcelStatus::WaitingForSign ||
                   parcel.status() == ParcelStatus::Signed;
        case CourierParcelView::Related:
            return true;
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

std::vector<Parcel> ParcelService::queryCourierParcels(
    const std::string& courierUsername,
    CourierParcelView view,
    const ParcelQuery& query) const {
    std::vector<Parcel> result;
    for (const auto& parcel : parcels_.listAll()) {
        if (!matchesCourierView(parcel, courierUsername, view)) {
            continue;
        }
        if (query.matches(parcel)) {
            result.push_back(parcel);
        }
    }

    return result;
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
