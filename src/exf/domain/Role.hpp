#pragma once

namespace exf {

/**
 * Role 列出阶段 1 需要的登录身份。
 */
enum class Role {
    /** 普通平台用户。 */
    User,

    /** 物流公司管理员。 */
    Admin
};

}  // namespace exf
