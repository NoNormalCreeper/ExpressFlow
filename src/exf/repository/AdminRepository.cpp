#include "exf/repository/AdminRepository.hpp"

#include <vector>

#include "exf/storage/RecordCodec.hpp"

namespace exf {

namespace {

Admin defaultAdmin() {
    return Admin("admin", "Logistics Admin", "admin", 0.0);
}

}  // namespace

// 使用指定存储加载管理员记录。
AdminRepository::AdminRepository(const FileStorage& storage)
    : storage_(storage) {
    loadAdmin();
}

// 表示占位对象可构造。
bool AdminRepository::isReady() const {
    return true;
}

// 返回物流公司管理员账户。
Admin& AdminRepository::getAdmin() {
    return admin_;
}

const Admin& AdminRepository::getAdmin() const {
    return admin_;
}

void AdminRepository::loadAdmin() {
    const auto lines = storage_.readLines("admins.txt");
    if (lines.empty()) {
        admin_ = defaultAdmin();
        return;
    }

    admin_ = AdminRecordCodec::decode(lines.front());
}

void AdminRepository::saveAdmin() {
    std::vector<std::string> lines{AdminRecordCodec::encode(admin_)};
    storage_.writeLines("admins.txt", lines);
}

}  // namespace exf
