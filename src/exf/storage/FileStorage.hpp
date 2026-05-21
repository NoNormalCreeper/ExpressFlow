#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace exf {

/**
 * FileStorage 保存文本文件持久化要使用的根目录。
 */
class FileStorage {
   public:
    /**
     * 用当前目录作为根目录创建存储对象。
     */
    FileStorage();

    /**
     * 用指定路径作为根目录创建存储对象。
     */
    explicit FileStorage(std::filesystem::path basePath);

    /**
     * 返回已配置的根路径。
     */
    const std::filesystem::path& basePath() const;

    /**
     * 从相对于根路径的文本文件中读取所有行。假定数据量不太大，适合一次性加载到内存中。
     */
    std::vector<std::string> readLines(
        const std::filesystem::path& relativePath) const;

    /**
     * 将文本行写入相对于根路径的文本文件。假定数据量不太大，适合一次性写入内存中的内容。
     */
    void writeLines(const std::filesystem::path& relativePath,
                    const std::vector<std::string>& lines) const;

   private:
    /** 后续数据文件的根目录。 */
    std::filesystem::path basePath_;
};

}  // namespace exf
