#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "exf/app/stage3/Stage3Role.hpp"

namespace exf {

/**
 * Stage3Session 保存一个客户端登录态。
 */
class Stage3Session {
   public:
    /**
     * 创建登录态。
     */
    Stage3Session(Stage3Role role, std::string username);

    /**
     * 返回登录角色。
     */
    Stage3Role role() const;

    /**
     * 返回登录用户名。
     */
    const std::string& username() const;

   private:
    /** 登录角色。 */
    Stage3Role role_;

    /** 登录用户名。 */
    std::string username_;
};

/**
 * Stage3SessionManager 维护网络版 token 到登录态的映射。
 */
class Stage3SessionManager {
   public:
    /**
     * 创建会话并返回 token。
     */
    std::string createSession(Stage3Role role, const std::string& username);

    /**
     * 查找 token 对应的会话。
     */
    std::optional<Stage3Session> findSession(const std::string& token) const;

    /**
     * 删除 token 对应的会话。
     */
    void removeSession(const std::string& token);

   private:
    /** 生成角色和用户名组成的账号键。 */
    static std::string accountKey(Stage3Role role, const std::string& username);

    /** 保护会话表和计数器。 */
    mutable std::mutex mutex_;

    /** token 自增序号。 */
    unsigned long counter_ = 0;

    /** 登录态表。 */
    std::unordered_map<std::string, Stage3Session> sessions_;

    /** 账号到当前 token 的映射，用于限制单账号单会话。 */
    std::unordered_map<std::string, std::string> accountTokens_;
};

}  // namespace exf
