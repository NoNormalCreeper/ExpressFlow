#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "exf/domain/Courier.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"

namespace exf {

/**
 * CourierRepository 负责阶段 2 快递员数据的文件持久化。
 */
class CourierRepository {
   public:
    /**
     * 使用指定文件存储创建快递员仓储。
     */
    explicit CourierRepository(const FileStorage& storage);

    /**
     * 返回仓储对象是否可用。
     */
    bool isReady() const;

    /**
     * 按用户名查找快递员。
     */
    const Courier* findCourier(std::string_view username) const;

    /**
     * 新增快递员并保存。
     */
    void createCourier(const Courier& courier);

    /**
     * 删除指定快递员并保存。
     */
    void deleteCourier(std::string_view username);

    /**
     * 替换指定快递员并保存。
     */
    void updateCourier(std::string_view username,
                       const Courier& courier);

    /**
     * 返回所有快递员。
     */
    const std::vector<Courier>& listAll() const;

    /**
     * 在仓储内部修改快递员并保存。
     */
    template <typename F>
    auto modifyCourier(std::string_view username, F&& modify) {
        using R = std::invoke_result_t<F, Courier&>;

        auto it = std::find_if(
            couriers_.begin(),
            couriers_.end(),
            [&username](const Courier& courier) {
                return courier.username() == username;
            });
        if (it == couriers_.end()) {
            if constexpr (std::is_void_v<R>) {
                return false;
            } else {
                return std::optional<R>{};
            }
        }

        if constexpr (std::is_void_v<R>) {
            std::invoke(std::forward<F>(modify), *it);
            saveCouriers();
            return true;
        } else {
            R result = std::invoke(std::forward<F>(modify), *it);
            saveCouriers();
            return std::optional<R>{std::move(result)};
        }
    }

    /**
     * 判断指定快递员是否存在。
     */
    bool courierExists(std::string_view username) const;

   private:
    /** 文件存储边界。 */
    const FileStorage& storage_;

    /** 内存中的快递员列表。 */
    std::vector<Courier> couriers_;

    /**
     * 从文件加载快递员。
     */
    void loadCouriers();

    /**
     * 将快递员保存到文件。
     */
    void saveCouriers();
};

}  // namespace exf
