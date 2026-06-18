#include "exf/repository/UserRepository.hpp"

#include <algorithm>

namespace exf {

// 创建用户仓储占位对象。
// UserRepository::UserRepository() = default;

// 表示占位对象可构造。
bool UserRepository::isReady() const {
    return true;
}

void UserRepository::loadUsers() {
    auto lines = storage_.readLines("users.txt");
    for (const auto& line : lines) {
        users_.push_back(UserRecordCodec::decode(line));
    }
}

void UserRepository::saveUsers() {
    std::vector<std::string> lines;
    for (const auto& user : users_) {
        lines.push_back(UserRecordCodec::encode(user));
    }
    storage_.writeLines("users.txt", lines);
}

const User* UserRepository::findUser(std::string_view username) const {
    auto it = std::find_if(
        users_.begin(), users_.end(),
        [&username](const User& user) { return user.username() == username; });

    return (it != users_.end()) ? &(*it) : nullptr;
}

void UserRepository::createUser(const User& user) {
    users_.push_back(user);
    saveUsers();
}

void UserRepository::deleteUser(std::string_view username) {
    auto it = std::find_if(
        users_.begin(), users_.end(),
        [&username](const User& user) { return user.username() == username; });

    if (it != users_.end()) {
        users_.erase(it);
    }
    saveUsers();
}

void UserRepository::updateUser(std::string_view username, const User& user) {
    auto it = std::find_if(
        users_.begin(), users_.end(),
        [&username](const User& user) { return user.username() == username; });

    if (it != users_.end()) {
        *it = user;
    }
    saveUsers();
}

const std::vector<User>& UserRepository::listAll() const {
    return users_;
}

}  // namespace exf
