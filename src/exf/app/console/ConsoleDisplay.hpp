#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "exf/domain/Courier.hpp"
#include "exf/domain/Parcel.hpp"
#include "exf/domain/User.hpp"

namespace exf {

/**
 * ConsoleDisplay 提供阶段 2/3 共用的列表展示。
 */
class ConsoleDisplay {
   public:
    static void printUsers(const std::vector<User>& users);
    static void printCouriers(const std::vector<Courier>& couriers);
    static void printParcels(const std::vector<Parcel>& parcels);

    static std::string parcelStatusText(ParcelStatus status);
    static std::string parcelItemTypeText(ParcelItemType itemType);

   private:
    static std::string formatTimestampForDisplay(std::string_view timestamp);
};

}  // namespace exf
