#pragma once

namespace exf {

/**
 * ParcelRepository 是阶段 1 的包裹持久化占位类。
 */
class ParcelRepository {
public:
    /**
     * 创建仓储占位对象。
     */
    ParcelRepository();

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;
};

} // namespace exf
