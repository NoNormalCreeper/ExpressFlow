#pragma once

#include <cstddef>
#include <optional>
#include <string_view>
#include <vector>

namespace exf {

/**
 * ConsoleSelection 提供编号列表选择的解析和提示能力。
 */
class ConsoleSelection {
   public:
    static std::optional<std::vector<size_t>> parseSelectionIndices(
        std::string_view input,
        size_t maxCount);

    static std::optional<size_t> promptSingleIndex(std::string_view label,
                                                   size_t maxCount);
};

}  // namespace exf
