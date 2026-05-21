#pragma once

namespace exf {

/**
 * AdminRepository 是阶段 1 的管理员持久化占位类。
 */
class AdminRepository {
public:
    /**
     * 创建仓储占位对象。
     */
    AdminRepository();

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;
};

} // namespace exf
