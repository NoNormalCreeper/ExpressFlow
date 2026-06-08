cmake_minimum_required(VERSION 3.20)

project(ExpressFlow LANGUAGES CXX)

include(FetchContent)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(EXPRESSFLOW_USE_LIBCXX "Build with LLVM libc++ when using Clang" OFF)

if(EXPRESSFLOW_USE_LIBCXX)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "EXPRESSFLOW_USE_LIBCXX requires a Clang compiler")
    endif()

    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-stdlib=libc++>)
    add_link_options(-stdlib=libc++)
endif()

add_library(core STATIC
    src/exf/domain/Account.cpp
    src/exf/domain/Admin.cpp
    src/exf/domain/Courier.cpp
    src/exf/domain/Item.cpp
    src/exf/domain/Parcel.cpp
    src/exf/domain/User.cpp
    src/exf/repository/AdminRepository.cpp
    src/exf/repository/CourierRepository.cpp
    src/exf/repository/ParcelRepository.cpp
    src/exf/repository/UserRepository.cpp
    src/exf/service/AdminService.cpp
    src/exf/service/AuthService.cpp
    src/exf/service/CourierService.cpp
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

add_executable(stage2
    apps/stage2/main.cpp
    src/exf/app/console/ConsoleInput.cpp
    src/exf/app/stage2/Stage2ConsoleMenu.cpp
    src/exf/app/stage2/Stage2ConsoleApp.cpp
)

target_link_libraries(stage2 PRIVATE core)

add_custom_target(run
    COMMAND $<TARGET_FILE:stage2>
    DEPENDS stage2
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

set(EXPRESSFLOW_USE_SYSTEM_GTEST ON)
if(EXPRESSFLOW_USE_LIBCXX)
    set(EXPRESSFLOW_USE_SYSTEM_GTEST OFF)
endif()

if(EXPRESSFLOW_USE_SYSTEM_GTEST)
    find_package(GTest CONFIG QUIET)
    if(NOT GTest_FOUND)
        find_package(GTest QUIET)
    endif()
endif()

if(NOT EXPRESSFLOW_USE_SYSTEM_GTEST OR NOT GTest_FOUND)
    if(EXISTS "/usr/src/googletest/CMakeLists.txt")
        add_subdirectory(
            /usr/src/googletest
            ${CMAKE_CURRENT_BINARY_DIR}/_deps/system-googletest-build
            EXCLUDE_FROM_ALL
        )
    else()
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
            DOWNLOAD_EXTRACT_TIMESTAMP TRUE
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

if(TARGET GTest::gtest_main)
    set(EXPRESSFLOW_GTEST_MAIN GTest::gtest_main)
elseif(TARGET gtest_main)
    set(EXPRESSFLOW_GTEST_MAIN gtest_main)
else()
    message(FATAL_ERROR "GoogleTest was not found and could not be made available")
endif()

add_executable(unit_tests
    src/exf/app/console/ConsoleInput.cpp
    src/exf/app/stage2/Stage2ConsoleApp.cpp
    src/exf/app/stage2/Stage2ConsoleMenu.cpp
    tests/domain/AccountTest.cpp
    tests/domain/CourierTest.cpp
    tests/domain/ItemTest.cpp
    tests/domain/ParcelTest.cpp
    tests/repository/ParcelRepositoryTest.cpp
    tests/repository/CourierRepositoryTest.cpp
    tests/repository/UserRepositoryTest.cpp
    tests/service/AdminServiceTest.cpp
    tests/service/CourierServiceTest.cpp
    tests/service/UserServiceTest.cpp
    tests/service/ParcelServiceTest.cpp
    tests/app/console/ConsoleInputTest.cpp
    tests/app/stage2/Stage2ConsoleAppTest.cpp
    tests/storage/FileStorageTest.cpp
    tests/storage/CourierRecordCodecTest.cpp
    tests/storage/ParcelRecordCodecTest.cpp
    tests/storage/RecordCodecTest.cpp
    tests/storage/UserRecordCodecTest.cpp
    tests/util/TimeUtilTest.cpp
)
target_link_libraries(unit_tests PRIVATE core ${EXPRESSFLOW_GTEST_MAIN})

enable_testing()
include(GoogleTest)
gtest_discover_tests(unit_tests)
