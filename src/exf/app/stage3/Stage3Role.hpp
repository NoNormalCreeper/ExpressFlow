#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace exf {

/**
 * Stage3Role 表示网络版登录后的角色。
 */
enum class Stage3Role {
    User,
    Courier,
    Admin
};

/**
 * Stage3RoleCodec 负责角色与协议文本互转。
 */
class Stage3RoleCodec {
   public:
    /**
     * 将角色编码为协议字段。
     */
    static std::string encode(Stage3Role role);

    /**
     * 从协议字段解析角色。
     */
    static std::optional<Stage3Role> decode(std::string_view text);

    /**
     * 返回适合控制台显示的角色名称。
     */
    static std::string displayName(Stage3Role role);
};

}  // namespace exf
