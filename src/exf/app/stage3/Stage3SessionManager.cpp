#include "exf/app/stage3/Stage3SessionManager.hpp"

#include <utility>

#include "exf/util/TimeUtil.hpp"

namespace exf {

// 创建一个登录态对象。
Stage3Session::Stage3Session(Stage3Role role, std::string username)
    : role_(role), username_(std::move(username)) {}

// 返回登录态角色。
Stage3Role Stage3Session::role() const {
    return role_;
}

// 返回登录态用户名。
const std::string& Stage3Session::username() const {
    return username_;
}

// 生成用于限制单账号单会话的账号键。
std::string Stage3SessionManager::accountKey(Stage3Role role,
                                             const std::string& username) {
    return Stage3RoleCodec::encode(role) + ":" + username;
}

// 创建新会话并替换同账号旧会话。
std::string Stage3SessionManager::createSession(Stage3Role role,
                                                const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto key = accountKey(role, username);
    const auto oldToken = accountTokens_.find(key);
    if (oldToken != accountTokens_.end()) {
        sessions_.erase(oldToken->second);
    }

    ++counter_;
    const std::string token =
        "T" + TimeUtil::nowMillisecondString() + "-" +
        std::to_string(counter_);
    sessions_.emplace(token, Stage3Session(role, username));
    accountTokens_[key] = token;
    return token;
}

// 按 token 查询会话。
std::optional<Stage3Session> Stage3SessionManager::findSession(
    const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = sessions_.find(token);
    if (it == sessions_.end()) {
        return std::nullopt;
    }
    return it->second;
}

// 删除 token 对应会话。
void Stage3SessionManager::removeSession(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = sessions_.find(token);
    if (it != sessions_.end()) {
        accountTokens_.erase(
            accountKey(it->second.role(), it->second.username()));
    }
    sessions_.erase(token);
}

}  // namespace exf
