#include "exf/storage/FileStorage.hpp"

#include <fstream>
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

std::vector<std::string> FileStorage::readLines(
    const std::filesystem::path& relativePath) const {
    std::vector<std::string> lines;
    std::ifstream file(basePath_ / relativePath);
    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

/**
 * 将文本行写入相对于根路径的文本文件。假定数据量不太大，适合一次性写入内存中的内容。
 */
void FileStorage::writeLines(const std::filesystem::path& relativePath,
                             const std::vector<std::string>& lines) const {
    const auto targetPath = basePath_ / relativePath;
    std::filesystem::create_directories(targetPath.parent_path());
    std::ofstream file(targetPath);

    for (const auto& line : lines) {
        file << line << '\n';
    }
}

}  // namespace exf
