#include "exf/storage/FileStorage.hpp"

#include <utility>

namespace exf {

// Uses the current directory as the default storage root.
FileStorage::FileStorage() : basePath_(".") {}

// Stores the selected storage root.
FileStorage::FileStorage(std::filesystem::path basePath) : basePath_(std::move(basePath)) {}

// Returns the selected storage root.
const std::filesystem::path& FileStorage::basePath() const
{
    return basePath_;
}

} // namespace exf
