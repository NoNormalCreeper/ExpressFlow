#pragma once

#include "exf/storage/FileStorage.hpp"
namespace exf {

/**
 * UserRepository 是阶段 1 的用户持久化占位类。
 */
class UserRepository {
   public:
    /**
     * 创建仓储占位对象。
     */
    UserRepository(const FileStorage& storage)
        : storage_(storage) {}

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;

   private:
    const FileStorage& storage_;
};

}  // namespace exf
