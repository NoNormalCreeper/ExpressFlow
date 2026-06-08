#pragma once

namespace exf {

/**
 * ParcelItemType 列出阶段 2 快递分类所需的物品类型。
 */
enum class ParcelItemType {
    /** 普通快递，后续阶段 2 按重量计费。 */
    Standard,

    /** 易碎品，后续阶段 2 按重量计费。 */
    Fragile,

    /** 图书，后续阶段 2 按本数计费。 */
    Book
};

}  // namespace exf
