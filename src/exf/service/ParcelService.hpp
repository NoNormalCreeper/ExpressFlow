#pragma once

#include <optional>
#include <tuple>
#include <vector>

#include "exf/repository/AdminRepository.hpp"
#include "exf/repository/ParcelRepository.hpp"
#include "exf/repository/UserRepository.hpp"

namespace exf {

enum class UserParcelView { Sent, Received, Related, WaitingForSign };

struct ParcelQuery {
    std::optional<std::string> id;
    std::optional<std::string> senderUsername;
    std::optional<std::string> receiverUsername;
    std::optional<ParcelStatus> status;
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
    InvalidDescription
};

/**
 * ParcelService 负责后续寄件、签收和查询流程。
 */
class ParcelService {
   public:
    /**
     * 创建服务占位对象。
     */
    ParcelService(UserRepository& users,
                  AdminRepository& admins,
                  ParcelRepository& parcels)
        : users_(users), admins_(admins), parcels_(parcels) {};

    /**
     * 返回阶段 1 固定寄件价格。
     */
    double stage1ParcelPrice() const;

    // 用户申请发送快递到指定用户的手中，提交发送后系统为本次快递分配快递单号，本次快递状态变为待签收，并扣除用户的余额（每件快递15元），并将扣除的金额转到物流公司管理员的账号。
    std::tuple<std::string, ParcelServiceError> sendParcel(
        const std::string& senderUsername,
        const std::string& receiverUsername,
        const std::string& description);

    ParcelServiceError signParcel(const std::string& receiverUsername,
                                  const std::string& parcelId);

    std::vector<Parcel> queryUserParcels(const std::string& username,
                                         UserParcelView view,
                                         const ParcelQuery& query) const;

    std::vector<Parcel> queryAdminParcels(const ParcelQuery& query) const;

   private:
    UserRepository& users_;
    AdminRepository& admins_;
    ParcelRepository& parcels_;

    std::vector<Parcel> queryParcels(const ParcelQuery& query) const;
    static bool matchesUserView(const Parcel& parcel,
                                const std::string& username,
                                UserParcelView view);
};

}  // namespace exf
