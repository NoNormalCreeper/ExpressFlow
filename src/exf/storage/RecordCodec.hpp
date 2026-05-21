#pragma once

#include <string>
#include <vector>
#include <string_view>
#include "exf/domain/User.hpp"

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

    static std::vector<std::string> split(std::string_view line);
    static std::string join(const std::vector<std::string>& fields);
};

class UserRecordCodec : public RecordCodec {
   public:
    static std::string encode(const User& user);
    static User decode(const std::string_view line);
};

}  // namespace exf
