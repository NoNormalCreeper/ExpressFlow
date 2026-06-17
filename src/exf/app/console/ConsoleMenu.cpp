#include "exf/app/console/ConsoleMenu.hpp"

#include <cstddef>
#include <iostream>

#include "exf/app/console/ConsoleInput.hpp"

namespace exf {

int ConsoleMenu::show(std::string_view title,
                      const std::vector<std::string>& options,
                      std::string_view footer,
                      std::string_view prompt) {
    printSeparator();
    print(title, options);
    if (!footer.empty()) {
        std::cout << footer << '\n';
    }
    return ConsoleInput::promptChoice(prompt, 1,
                                      static_cast<int>(options.size()));
}

void ConsoleMenu::print(std::string_view title,
                        const std::vector<std::string>& options,
                        int startIndex) {
    std::cout << title << '\n';
    for (size_t i = 0; i < options.size(); ++i) {
        std::cout << "  " << (startIndex + static_cast<int>(i)) << ". "
                  << options[i] << '\n';
    }
}

void ConsoleMenu::printSeparator(std::string_view separator, int length) {
    std::cout << std::string(length, separator.empty() ? '-' : separator[0])
              << '\n';
}

}  // namespace exf
