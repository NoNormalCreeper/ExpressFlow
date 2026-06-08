#pragma once

namespace exf {

/**
 * ParcelStatus 列出阶段 2 快递流转需要的包裹状态。
 */
enum class ParcelStatus {
    /** 等待收件人签收。 */
    WaitingForSign = 0,

    /** 收件人已签收。 */
    Signed = 1,

    /** 等待快递员揽收。 */
    WaitingForPickup = 2
};

}  // namespace exf
