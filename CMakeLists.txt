cmake_minimum_required(VERSION 3.20)

project(ExpressFlow LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(core STATIC
    src/exf/domain/Account.cpp
    src/exf/domain/Admin.cpp
    src/exf/domain/Parcel.cpp
    src/exf/domain/User.cpp
    src/exf/repository/AdminRepository.cpp
    src/exf/repository/ParcelRepository.cpp
    src/exf/repository/UserRepository.cpp
    src/exf/service/AdminService.cpp
    src/exf/service/AuthService.cpp
    src/exf/service/ParcelService.cpp
    src/exf/service/UserService.cpp
    src/exf/storage/FileStorage.cpp
    src/exf/storage/RecordCodec.cpp
    src/exf/util/IdGenerator.cpp
    src/exf/util/StringUtil.cpp
    src/exf/util/TimeUtil.cpp
    src/exf/util/Validator.cpp
)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_features(core PUBLIC cxx_std_20)

add_executable(stage1
    apps/stage1/main.cpp
    src/exf/app/stage1/Stage1ConsoleApp.cpp
)

target_link_libraries(stage1 PRIVATE core)

add_custom_target(run
    COMMAND $<TARGET_FILE:stage1>
    DEPENDS stage1
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_executable(unit_tests
    tests/test_smoke.cpp
)
target_link_libraries(unit_tests PRIVATE core)

enable_testing()
add_test(NAME unit_tests COMMAND unit_tests)
