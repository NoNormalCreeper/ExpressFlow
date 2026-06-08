#pragma once

#include <optional>
#include <tuple>
#include <vector>

#include "exf/domain/Item.hpp"
#include "exf/domain/ParcelItemType.hpp"
#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/CourierRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"

namespace exf {

enum class UserParcelView { Sent, Received, Related, WaitingForSign };

enum class CourierParcelView {
    AssignedWaitingForPickup,
    PickedUpOrDelivered,
    Related
};

struct ParcelQuery {
    std::optional<std::string> id;
    std::optional<std::string> senderUsername;
    std::optional<std::string> receiverUsername;
    std::optional<std::string> courierUsername;
    std::optional<ParcelStatus> status;
    std::optional<ParcelItemType> itemType;
    std::optional<std::string> sentFrom;
    std::optional<std::string> sentTo;

    bool matches(const Parcel& parcel) const;
};

enum class ParcelServiceError {
    Nil,
    SenderNotFound,
    ReceiverNotFound,
    AdminNotFound,
    InsufficientBalance,
    ParcelNotFound,
    NotReceiver,
    AlreadySigned,
    InvalidDescription,
    CourierNotFound,
    InvalidItem,
    NotWaitingForPickup,
    CourierAlreadyAssigned,
    CourierNotAssigned,
    NotAssignedCourier,
    AlreadyPickedUp,
    NotWaitingForSign,
    AdminInsufficientBalance
};

/**
 * ParcelService 负责后续寄件、签收和查询流程。
 */
class ParcelService {
   public:
    /**
     * 创建支持阶段 2 快递员流程的服务。
     */
    ParcelService(UserRepository& users,
                  AdminRepository& admins,
                  ParcelRepository& parcels,
                  CourierRepository& couriers)
        : users_(users),
          admins_(admins),
          parcels_(parcels),
          couriers_(couriers) {};

    /**
     * 阶段 2 按物品类型计价发送快递，创建待揽收包裹。
     */
    std::tuple<std::string, ParcelServiceError> sendParcel(
        const std::string& senderUsername,
        const std::string& receiverUsername,
        const std::string& description,
        const Item& item);

    /**
     * 管理员为待揽收包裹分配快递员。
     */
    ParcelServiceError assignCourier(const std::string& parcelId,
                                     const std::string& courierUsername);

    /**
     * 快递员揽收包裹，并获得运费的一半。
     */
    ParcelServiceError pickupParcel(const std::string& courierUsername,
                                    const std::string& parcelId);

    ParcelServiceError signParcel(const std::string& receiverUsername,
                                  const std::string& parcelId);

    std::vector<Parcel> queryUserParcels(const std::string& username,
                                         UserParcelView view,
                                         const ParcelQuery& query) const;

    std::vector<Parcel> queryAdminParcels(const ParcelQuery& query) const;

    std::vector<Parcel> queryCourierParcels(
        const std::string& courierUsername,
        CourierParcelView view,
        const ParcelQuery& query) const;

   private:
    UserRepository& users_;
    AdminRepository& admins_;
    ParcelRepository& parcels_;
    CourierRepository& couriers_;

    std::vector<Parcel> queryParcels(const ParcelQuery& query) const;
    static bool matchesUserView(const Parcel& parcel,
                                const std::string& username,
                                UserParcelView view);
    static bool matchesCourierView(const Parcel& parcel,
                                   const std::string& courierUsername,
                                   CourierParcelView view);
};

}  // namespace exf
