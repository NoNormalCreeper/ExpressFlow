#include "exf/storage/FileStorage.hpp"

#include <fstream>
#include <stdexcept>
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
    const auto targetPath = basePath_ / relativePath;
    if (!std::filesystem::exists(targetPath)) {
        return lines;
    }

    std::ifstream file(targetPath);
    if (!file.is_open()) {
        throw std::runtime_error("无法读取数据文件: " + targetPath.string());
    }

    std::string line;

    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    if (file.bad()) {
        throw std::runtime_error("读取数据文件失败: " + targetPath.string());
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
    if (!file.is_open()) {
        throw std::runtime_error("无法写入数据文件: " + targetPath.string());
    }

    for (const auto& line : lines) {
        file << line << '\n';
    }
    if (!file) {
        throw std::runtime_error("写入数据文件失败: " + targetPath.string());
    }
}

}  // namespace exf
