#pragma once

#include <filesystem>

namespace exf {

/**
 * FileStorage stores the base directory planned for text-file persistence.
 */
class FileStorage {
public:
    /**
     * Creates storage rooted at the current directory.
     */
    FileStorage();

    /**
     * Creates storage rooted at the given base path.
     */
    explicit FileStorage(std::filesystem::path basePath);

    /**
     * Returns the configured base path.
     */
    const std::filesystem::path& basePath() const;

private:
    /** Base directory for future data files. */
    std::filesystem::path basePath_;
};

} // namespace exf
