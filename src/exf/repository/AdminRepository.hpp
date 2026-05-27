#pragma once

#include "exf/storage/FileStorage.hpp"
#include "exf/domain/Admin.hpp"
namespace exf {

/**
 * AdminRepository 是阶段 1 的管理员持久化占位类。
 */
class AdminRepository {
   public:
    /**
     * 创建仓储占位对象。
     */
    AdminRepository(const FileStorage& storage)
        : storage_(storage) {};

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;

    Admin& getAdmin();

    private:
    const FileStorage& storage_;
};

}  // namespace exf
