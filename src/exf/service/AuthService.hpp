#pragma once

namespace exf {

/**
 * AuthService 负责后续注册、登录和改密流程。
 */
class AuthService {
public:
    /**
     * 创建服务占位对象。
     */
    AuthService();

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;
};

} // namespace exf
