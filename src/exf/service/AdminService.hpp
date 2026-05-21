#pragma once

namespace exf {

/**
 * AdminService 负责阶段 1 的管理员业务操作。
 */
class AdminService {
   public:
    /**
     * 创建服务占位对象。
     */
    AdminService();

    /**
     * 返回服务占位对象是否可用。
     */
    bool isReady() const;
};

}  // namespace exf
