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
    // 在 repo 内部修改用户并保存；返回业务结果 + 是否修改。非 void 返回 optional<T>，void 返回 bool
    template <typename F>
    auto modifyUser(std::string_view username, F&& modify) {
        using R = std::invoke_result_t<F, User&>;

        auto it = std::find_if(
        users_.begin(), users_.end(),
        [&username](const User& user) { return user.username() == username; });
        if (it == users_.end()) {
            if constexpr (std::is_void_v<R>) {
                return false;
            } else {
                return std::optional<R>{};
            }
        }

        if constexpr (std::is_void_v<R>) {
            std::invoke(std::forward<F>(modify), *it);
            saveUsers();
            return true;
        } else {
            R result = std::invoke(std::forward<F>(modify), *it);
            saveUsers();
            return std::optional<R>{std::move(result)};
        }
    }

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
