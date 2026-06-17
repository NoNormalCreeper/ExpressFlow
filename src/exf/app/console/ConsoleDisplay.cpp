#include "exf/app/console/ConsoleDisplay.hpp"

#include <exception>
#include <iostream>

#include "exf/util/TimeUtil.hpp"

namespace exf {

void ConsoleDisplay::printUsers(const std::vector<User>& users) {
    if (users.empty()) {
        std::cout << "暂无注册用户。\n";
        return;
    }

    std::cout << "共 " << users.size() << " 个用户：\n";
    for (size_t i = 0; i < users.size(); ++i) {
        const auto& user = users[i];
        std::cout << "  [" << (i + 1) << "] " << user.username() << " | "
                  << user.name() << " | " << user.phone() << " | "
                  << user.address() << " | 余额: "
                  << user.account().balance() << " 元\n";
    }
}

void ConsoleDisplay::printCouriers(const std::vector<Courier>& couriers) {
    if (couriers.empty()) {
        std::cout << "暂无快递员。\n";
        return;
    }

    std::cout << "共 " << couriers.size() << " 个快递员：\n";
    for (size_t i = 0; i < couriers.size(); ++i) {
        const auto& courier = couriers[i];
        std::cout << "  [" << (i + 1) << "] " << courier.username() << " | "
                  << courier.name() << " | " << courier.phone()
                  << " | 余额: " << courier.account().balance() << " 元\n";
    }
}

void ConsoleDisplay::printParcels(const std::vector<Parcel>& parcels) {
    if (parcels.empty()) {
        std::cout << "没有符合条件的快递。\n";
        return;
    }

    std::cout << "共 " << parcels.size() << " 件快递：\n";
    for (size_t i = 0; i < parcels.size(); ++i) {
        const auto& parcel = parcels[i];
        std::cout << "  [" << (i + 1) << "] " << parcel.id() << " | "
                  << parcel.senderUsername() << " -> "
                  << parcel.receiverUsername() << " | "
                  << parcelStatusText(parcel.status()) << " | "
                  << parcelItemTypeText(parcel.itemType()) << "("
                  << parcel.itemAmount() << ")"
                  << " | 快递员: "
                  << (parcel.courierUsername().empty()
                          ? "-"
                          : parcel.courierUsername())
                  << " | 寄件: " << formatTimestampForDisplay(parcel.sentAt())
                  << " | 揽收: "
                  << formatTimestampForDisplay(parcel.pickedAt())
                  << " | 签收: "
                  << formatTimestampForDisplay(parcel.receivedAt())
                  << " | 运费: " << parcel.fee() << " 元 | "
                  << parcel.description() << '\n';
    }
}

std::string ConsoleDisplay::parcelStatusText(ParcelStatus status) {
    switch (status) {
        case ParcelStatus::WaitingForPickup:
            return "待揽收";
        case ParcelStatus::WaitingForSign:
            return "待签收";
        case ParcelStatus::Signed:
            return "已签收";
    }
    return "未知";
}

std::string ConsoleDisplay::parcelItemTypeText(ParcelItemType itemType) {
    switch (itemType) {
        case ParcelItemType::Standard:
            return "普通快递";
        case ParcelItemType::Fragile:
            return "易碎品";
        case ParcelItemType::Book:
            return "图书";
    }
    return "未知";
}

std::string ConsoleDisplay::formatTimestampForDisplay(
    std::string_view timestamp) {
    if (timestamp.empty()) {
        return "-";
    }
    try {
        return TimeUtil::formatTimestamp(timestamp);
    } catch (const std::exception&) {
        return std::string(timestamp);
    }
}

}  // namespace exf
