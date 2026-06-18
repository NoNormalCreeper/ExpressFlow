#include "exf/repository/CourierRepository.hpp"

namespace exf {

// 使用指定存储加载已有快递员记录。
CourierRepository::CourierRepository(const FileStorage& storage)
    : storage_(storage) {
    loadCouriers();
}

// 表示仓储对象可构造。
bool CourierRepository::isReady() const {
    return true;
}

// 按用户名查找快递员。
const Courier* CourierRepository::findCourier(
    std::string_view username) const {
    auto it = std::find_if(couriers_.begin(),
                           couriers_.end(),
                           [&username](const Courier& courier) {
                               return courier.username() == username;
                           });

    return (it != couriers_.end()) ? &(*it) : nullptr;
}

// 新增快递员并保存。
void CourierRepository::createCourier(const Courier& courier) {
    couriers_.push_back(courier);
    saveCouriers();
}

// 删除指定快递员并保存。
void CourierRepository::deleteCourier(std::string_view username) {
    auto it = std::find_if(couriers_.begin(),
                           couriers_.end(),
                           [&username](const Courier& courier) {
                               return courier.username() == username;
                           });

    if (it != couriers_.end()) {
        couriers_.erase(it);
    }
    saveCouriers();
}

// 替换指定快递员并保存。
void CourierRepository::updateCourier(std::string_view username,
                                      const Courier& courier) {
    auto it = std::find_if(couriers_.begin(),
                           couriers_.end(),
                           [&username](const Courier& courier) {
                               return courier.username() == username;
                           });

    if (it != couriers_.end()) {
        *it = courier;
    }
    saveCouriers();
}

// 返回所有快递员。
const std::vector<Courier>& CourierRepository::listAll() const {
    return couriers_;
}

// 判断指定快递员是否存在。
bool CourierRepository::courierExists(
    std::string_view username) const {
    return findCourier(username) != nullptr;
}

void CourierRepository::loadCouriers() {
    const auto lines = storage_.readLines("couriers.txt");
    for (const auto& line : lines) {
        couriers_.push_back(CourierRecordCodec::decode(line));
    }
}

void CourierRepository::saveCouriers() {
    std::vector<std::string> lines;
    for (const auto& courier : couriers_) {
        lines.push_back(CourierRecordCodec::encode(courier));
    }
    storage_.writeLines("couriers.txt", lines);
}

}  // namespace exf
