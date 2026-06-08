#pragma once

#include "exf/domain/ParcelItemType.hpp"
#include "exf/util/Money.hpp"

namespace exf {

/**
 * Item 是阶段 2 物品计价继承体系的抽象基类。
 */
class Item {
   public:
    /**
     * 允许通过基类引用安全销毁子类对象。
     */
    virtual ~Item() = default;

    /**
     * 返回物品分类。
     */
    ParcelItemType type() const;

    /**
     * 返回计费用数量，重量类为 kg，图书为本数。
     */
    double amount() const;

    /**
     * 返回该物品输入是否满足计费要求。
     */
    bool isValid() const;

    /**
     * 按具体物品规则计算快递价格。
     */
    virtual util::Money getPrice() const = 0;

   protected:
    /**
     * 保存所有物品共有的分类和计费数量。
     */
    Item(ParcelItemType type, double amount);

   private:
    /** 物品分类。 */
    ParcelItemType type_;

    /** 计费用数量，重量类为 kg，图书为本数。 */
    double amount_;
};

/**
 * StandardItem 表示普通快递，按 5 元/kg 计费。
 */
class StandardItem final : public Item {
   public:
    /**
     * 用重量创建普通快递物品。
     */
    explicit StandardItem(double weightKg);

    /**
     * 按 5 元/kg 计算价格。
     */
    util::Money getPrice() const override;
};

/**
 * FragileItem 表示易碎品，按 8 元/kg 计费。
 */
class FragileItem final : public Item {
   public:
    /**
     * 用重量创建易碎品物品。
     */
    explicit FragileItem(double weightKg);

    /**
     * 按 8 元/kg 计算价格。
     */
    util::Money getPrice() const override;
};

/**
 * BookItem 表示图书，按 2 元/本计费。
 */
class BookItem final : public Item {
   public:
    /**
     * 用本数创建图书物品。
     */
    explicit BookItem(int count);

    /**
     * 按 2 元/本计算价格。
     */
    util::Money getPrice() const override;
};

}  // namespace exf
