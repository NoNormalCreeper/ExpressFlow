#pragma once

#include <filesystem>

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

   private:
    /** 后续数据文件的根目录。 */
    std::filesystem::path basePath_;
};

}  // namespace exf
