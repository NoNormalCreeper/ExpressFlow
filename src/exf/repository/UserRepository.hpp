#pragma once

namespace exf {

/**
 * UserRepository 是阶段 1 的用户持久化占位类。
 */
class UserRepository {
   public:
    /**
     * 创建仓储占位对象。
     */
    UserRepository();

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;
};

}  // namespace exf
