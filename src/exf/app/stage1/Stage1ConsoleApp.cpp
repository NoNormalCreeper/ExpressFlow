#include "exf/app/stage1/Stage1ConsoleApp.hpp"

#include <iostream>

namespace exf {

// 打印阶段 1 的占位菜单。
int Stage1ConsoleApp::run() const {
    std::cout << "ExpressFlow Stage 1" << '\n';
    std::cout << "Logistics management system skeleton" << '\n';
    std::cout << "Business features will be implemented in later iterations."
              << '\n';
    return 0;
}

}  // namespace exf
