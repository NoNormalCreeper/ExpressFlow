#pragma once

namespace exf {

/**
 * ParcelService 负责后续寄件、签收和查询流程。
 */
class ParcelService {
public:
    /**
     * 创建服务占位对象。
     */
    ParcelService();

    /**
     * 返回阶段 1 固定寄件价格。
     */
    double stage1ParcelPrice() const;
};

} // namespace exf
