#include "exf/app/stage3/Stage3SessionManager.hpp"

#include <utility>

#include "exf/util/TimeUtil.hpp"

namespace exf {

Stage3Session::Stage3Session(Stage3Role role, std::string username)
    : role_(role), username_(std::move(username)) {}

Stage3Role Stage3Session::role() const {
    return role_;
}

const std::string& Stage3Session::username() const {
    return username_;
}

std::string Stage3SessionManager::createSession(Stage3Role role,
                                                const std::string& username) {
    std::lock_guard<std::mutex> lock(mutex_);
    ++counter_;
    const std::string token =
        "T" + TimeUtil::nowMillisecondString() + "-" +
        std::to_string(counter_);
    sessions_.emplace(token, Stage3Session(role, username));
    return token;
}

std::optional<Stage3Session> Stage3SessionManager::findSession(
    const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);
    const auto it = sessions_.find(token);
    if (it == sessions_.end()) {
        return std::nullopt;
    }
    return it->second;
}

void Stage3SessionManager::removeSession(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    sessions_.erase(token);
}

}  // namespace exf
