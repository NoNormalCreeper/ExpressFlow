#include "exf/app/stage2/Stage2ConsoleApp.hpp"

#include <iostream>
#include <utility>

namespace exf {

Stage2ConsoleApp::Stage2ConsoleApp(std::filesystem::path dataDir)
    : storage_(std::move(dataDir)),
      userRepository_(storage_),
      adminRepository_(storage_),
      parcelRepository_(storage_) {}

int Stage2ConsoleApp::run() {
    std::cout << "ExpressFlow Stage 2" << '\n';
    std::cout << "阶段 2 入口已创建，后续将在此加入快递员和待揽收流程。\n";
    return 0;
}

}  // namespace exf
