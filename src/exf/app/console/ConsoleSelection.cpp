#include "exf/app/console/ConsoleSelection.hpp"

#include <algorithm>
#include <exception>
#include <iostream>
#include <sstream>
#include <string>

#include "exf/app/console/ConsoleInput.hpp"

namespace exf {

std::optional<std::vector<size_t>> ConsoleSelection::parseSelectionIndices(
    std::string_view input,
    size_t maxCount) {
    std::string normalized(input);
    std::replace(normalized.begin(), normalized.end(), ',', ' ');

    std::istringstream stream(normalized);
    std::vector<std::string> tokens;
    std::string token;
    while (stream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return std::vector<size_t>{};
    }
    if (tokens.size() == 1 && tokens.front() == "all") {
        std::vector<size_t> all;
        all.reserve(maxCount);
        for (size_t i = 0; i < maxCount; ++i) {
            all.push_back(i);
        }
        return all;
    }

    std::vector<size_t> indices;
    std::vector<bool> used(maxCount, false);
    for (const auto& item : tokens) {
        size_t parsedLength = 0;
        int parsedIndex = 0;
        try {
            parsedIndex = std::stoi(item, &parsedLength);
        } catch (const std::exception&) {
            return std::nullopt;
        }

        if (parsedLength != item.size() || parsedIndex <= 0 ||
            static_cast<size_t>(parsedIndex) > maxCount) {
            return std::nullopt;
        }

        const size_t zeroBasedIndex = static_cast<size_t>(parsedIndex - 1);
        if (!used[zeroBasedIndex]) {
            indices.push_back(zeroBasedIndex);
            used[zeroBasedIndex] = true;
        }
    }

    return indices;
}

std::optional<size_t> ConsoleSelection::promptSingleIndex(
    std::string_view label,
    size_t maxCount) {
    while (true) {
        const auto selected =
            parseSelectionIndices(ConsoleInput::promptLine(label), maxCount);
        if (!selected.has_value()) {
            std::cout << "选择不合法。\n";
            continue;
        }
        if (selected->empty()) {
            std::cout << "未选择。\n";
            return std::nullopt;
        }
        if (selected->size() == 1) {
            return selected->front();
        }
        std::cout << "一次只能选择一个序号。\n";
    }
}

}  // namespace exf
