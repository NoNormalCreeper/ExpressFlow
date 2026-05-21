#pragma once

#include <string>
#include <vector>

#include "exf/domain/User.hpp"
#include "exf/storage/FileStorage.hpp"
#include "exf/storage/RecordCodec.hpp"

namespace exf {

/**
 * UserRepository 是阶段 1 的用户持久化占位类。
 */
class UserRepository {
   public:
    /**
     * 创建仓储占位对象。
     */
    UserRepository(const FileStorage& storage) : storage_(storage) {
        loadUsers();
    };

    /**
     * 返回仓储占位对象是否可用。
     */
    bool isReady() const;

    // 下面的增删改查操作默认传入的用户名唯一，需要在服务层判断并处理重复用户名的情况。
    const User* findUser(const std::string_view& username) const;
    void createUser(const User& user);
    void deleteUser(const std::string_view& username);
    void updateUser(const std::string_view& username, const User& user);

    bool userExists(const std::string_view& username) const {
        return findUser(username) != nullptr;
    }

   private:
    const FileStorage& storage_;

    std::vector<User> users_;

    void loadUsers();
    void saveUsers();
};

}  // namespace exf
