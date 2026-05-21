#include "exf/storage/RecordCodec.hpp"

namespace exf {

// 返回后续文本持久化记录使用的简单分隔符。
char RecordCodec::delimiter() {
    return '|';
}

}  // namespace exf
