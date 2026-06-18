#include "exf/repository/ParcelRepository.hpp"

#include <algorithm>

#include "exf/storage/RecordCodec.hpp"

namespace exf {

// 使用指定存储加载已有包裹记录。
ParcelRepository::ParcelRepository(const FileStorage& storage)
    : storage_(storage) {
    loadParcels();
}

std::vector<Parcel> ParcelRepository::listIf(
    std::function<bool(const Parcel&)> predicate) const {
    std::vector<Parcel> temp;
    for (const auto& p : parcels_) {
        if (predicate(p)) {
            temp.push_back(p);
        }
    }
    return temp;
}
const Parcel* ParcelRepository::findParcel(std::string_view id) const {
    auto it = std::find_if(parcels_.begin(), parcels_.end(),
                           [&id](const Parcel& p) { return p.id() == id; });

    return (it != parcels_.end()) ? &(*it) : nullptr;
}
void ParcelRepository::createParcel(const Parcel& parcel) {
    parcels_.emplace_back(parcel);
    saveParcels();
}
void ParcelRepository::updateParcel(std::string_view id,
                                    const Parcel& parcel) {
    auto it = std::find_if(parcels_.begin(), parcels_.end(),
                           [&id](const Parcel& p) { return p.id() == id; });

    if (it != parcels_.end()) {
        *it = parcel;
    }
    saveParcels();
}
const std::vector<Parcel>& ParcelRepository::listAll() const {
    return parcels_;
}
std::vector<Parcel> ParcelRepository::listBySender(
    std::string_view username) const {
    return listIf([&username](const Parcel& p) {
        return p.senderUsername() == username;
    });
}
std::vector<Parcel> ParcelRepository::listByReceiver(
    std::string_view username) const {
    return listIf([&username](const Parcel& p) {
        return p.receiverUsername() == username;
    });
}
std::vector<Parcel> ParcelRepository::listWaitingForReceiver(
    std::string_view username) const {
    return listIf([&username](const Parcel& p) {
        return p.status() == ParcelStatus::WaitingForSign &&
               p.receiverUsername() == username;
    });
}
// 表示占位对象可构造。
bool ParcelRepository::isReady() const {
    return true;
}

void ParcelRepository::loadParcels() {
    auto lines = storage_.readLines("parcels.txt");
    for (const auto& line : lines) {
        parcels_.push_back(ParcelRecordCodec::decode(line));
    }
}

void ParcelRepository::saveParcels() {
    std::vector<std::string> lines;
    for (const auto& parcel : parcels_) {
        lines.push_back(ParcelRecordCodec::encode(parcel));
    }
    storage_.writeLines("parcels.txt", lines);
}

}  // namespace exf
