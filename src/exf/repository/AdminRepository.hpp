#pragma once

#include <functional>
#include <utility>

#include "exf/domain/Admin.hpp"
#include "exf/storage/FileStorage.hpp"

namespace exf {

/**
 * AdminRepository 是阶段 1 的管理员持久化占位类。
 */
class AdminRepository {
   public:
    /**
     * 使用指定文件存储创建管理员仓储。
     */
    explicit AdminRepository(const FileStorage& storage);

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;

    /**
     * 返回物流公司管理员账户。
     */
    Admin& getAdmin();
    const Admin& getAdmin() const;

    /**
     * 在仓储内部修改管理员并保存。
     */
    template <typename F>
    bool modifyAdmin(F&& modify) {
        std::invoke(std::forward<F>(modify), admin_);
        saveAdmin();
        return true;
    }

   private:
    const FileStorage& storage_;
    Admin admin_;

    void loadAdmin();
    void saveAdmin();
};

}  // namespace exf
