#include "exf/storage/FileStorage.hpp"

#include <utility>

namespace exf {

// 默认使用当前目录作为存储根目录。
FileStorage::FileStorage() : basePath_(".") {}

// 保存选定的存储根目录。
FileStorage::FileStorage(std::filesystem::path basePath)
    : basePath_(std::move(basePath)) {}

// 返回选定的存储根目录。
const std::filesystem::path& FileStorage::basePath() const {
    return basePath_;
}

}  // namespace exf
