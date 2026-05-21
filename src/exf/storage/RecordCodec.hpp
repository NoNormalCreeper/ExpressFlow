#pragma once

namespace exf {

/**
 * RecordCodec 统一保存文件记录使用的文本分隔符。
 */
class RecordCodec {
public:
    /**
     * 返回简单文本记录使用的分隔符。
     */
    static char delimiter();
};

} // namespace exf
