#pragma once

#include <functional>
#include <string_view>
#include <vector>

#include "exf/domain/Parcel.hpp"
#include "exf/storage/FileStorage.hpp"

namespace exf {

/**
 * ParcelRepository 是阶段 1 的包裹持久化占位类。
 */
class ParcelRepository {
   public:
    /**
     * 使用指定文件存储创建包裹仓储。
     */
    explicit ParcelRepository(const FileStorage& storage);

    const Parcel* findParcel(std::string_view id) const;
    void createParcel(const Parcel& parcel);
    void updateParcel(std::string_view id, const Parcel& parcel);

    const std::vector<Parcel>& listAll() const;
    std::vector<Parcel> listBySender(std::string_view username) const;
    std::vector<Parcel> listByReceiver(std::string_view username) const;
    std::vector<Parcel> listWaitingForReceiver(
        std::string_view username) const;

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;

   private:
    const FileStorage& storage_;
    std::vector<Parcel> parcels_;

    void loadParcels();
    void saveParcels();

    std::vector<Parcel> listIf(
        std::function<bool(const Parcel&)> predicate) const;
};

}  // namespace exf
