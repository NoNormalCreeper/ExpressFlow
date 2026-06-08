#include "exf/domain/Item.hpp"

namespace exf {

Item::Item(ParcelItemType type, double amount)
    : type_(type), amount_(amount) {}

ParcelItemType Item::type() const {
    return type_;
}

double Item::amount() const {
    return amount_;
}

bool Item::isValid() const {
    return amount_ > 0.0;
}

StandardItem::StandardItem(double weightKg)
    : Item(ParcelItemType::Standard, weightKg) {}

util::Money StandardItem::getPrice() const {
    if (!isValid()) {
        return util::Money::from_double(0.0);
    }
    return util::Money::from_double(amount() * 5.0);
}

FragileItem::FragileItem(double weightKg)
    : Item(ParcelItemType::Fragile, weightKg) {}

util::Money FragileItem::getPrice() const {
    if (!isValid()) {
        return util::Money::from_double(0.0);
    }
    return util::Money::from_double(amount() * 8.0);
}

BookItem::BookItem(int count)
    : Item(ParcelItemType::Book, static_cast<double>(count)) {}

util::Money BookItem::getPrice() const {
    if (!isValid()) {
        return util::Money::from_double(0.0);
    }
    return util::Money::from_double(amount() * 2.0);
}

}  // namespace exf
