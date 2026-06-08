#include "exf/app/stage2/Stage2ConsoleApp.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

class ScopedCinRedirect {
   public:
    explicit ScopedCinRedirect(std::istream& input)
        : original_(std::cin.rdbuf(input.rdbuf())) {}

    ~ScopedCinRedirect() {
        std::cin.rdbuf(original_);
    }

   private:
    std::streambuf* original_;
};

class ScopedCoutRedirect {
   public:
    explicit ScopedCoutRedirect(std::ostream& output)
        : original_(std::cout.rdbuf(output.rdbuf())) {}

    ~ScopedCoutRedirect() {
        std::cout.rdbuf(original_);
    }

   private:
    std::streambuf* original_;
};

class ScopedTempDir {
   public:
    explicit ScopedTempDir(std::string_view testName) {
        const auto suffix =
            std::chrono::steady_clock::now().time_since_epoch().count();
        path_ = std::filesystem::temp_directory_path() /
                ("ExpressFlow-" + std::string(testName) + "-" +
                 std::to_string(suffix));
        std::filesystem::create_directories(path_);
    }

    ~ScopedTempDir() {
        std::error_code ec;
        std::filesystem::remove_all(path_, ec);
    }

    const std::filesystem::path& path() const {
        return path_;
    }

   private:
    std::filesystem::path path_;
};

std::string runAppWithInput(std::string_view testName,
                            const std::string& inputText) {
    ScopedTempDir tempDir(testName);
    std::istringstream input(inputText);
    std::ostringstream output;
    ScopedCinRedirect cinRedirect(input);
    ScopedCoutRedirect coutRedirect(output);

    exf::Stage2ConsoleApp app(tempDir.path());
    EXPECT_EQ(app.run(), 0);

    return output.str();
}

TEST(Stage2ConsoleAppTest, DirectExitShowsMainMenuAndGoodbye) {
    const auto output = runAppWithInput("direct-exit", "6\n");

    EXPECT_NE(output.find("ExpressFlow Stage 2"), std::string::npos);
    EXPECT_NE(output.find("主菜单"), std::string::npos);
    EXPECT_NE(output.find("6. 退出程序"), std::string::npos);
    EXPECT_NE(output.find("再见。"), std::string::npos);
}

TEST(Stage2ConsoleAppTest, CompletesMainParcelWorkflow) {
    const std::string input =
        "1\n1\nalice\nAlice\n13800000001\nsecret1\nAlice Road\n"
        "1\n1\nbob\nBob\n13800000002\nsecret2\nBob Road\n"
        "1\n2\nalice\nsecret1\n"
        "2\n2\n100\n"
        "3\n1\nbob\nbook parcel\n3\n2\n"
        "1\n6\n"
        "1\n4\nadmin\nadmin\n"
        "5\n3\nc01\nCourier One\n13900000001\ncourier1\n"
        "5\n5\n1\nc01\n"
        "1\n6\n"
        "1\n3\nc01\ncourier1\n"
        "4\n2\nall\n"
        "1\n6\n"
        "1\n2\nbob\nsecret2\n"
        "3\n2\nall\n"
        "3\n3\n2\n\n\n\n4\n4\n\n\n"
        "6\n";

    const auto output = runAppWithInput("parcel-flow", input);

    EXPECT_NE(output.find("注册成功！"), std::string::npos);
    EXPECT_NE(output.find("用户登录成功！"), std::string::npos);
    EXPECT_NE(output.find("充值成功！当前余额: 100.00 元。"),
              std::string::npos);
    EXPECT_NE(output.find("寄件成功，快递单号:"), std::string::npos);
    EXPECT_NE(output.find("快递员添加成功。"), std::string::npos);
    EXPECT_NE(output.find("快递员分配成功。"), std::string::npos);
    EXPECT_NE(output.find("揽收完成，共成功揽收 1 件。"), std::string::npos);
    EXPECT_NE(output.find("签收完成，共成功签收 1 件。"), std::string::npos);
    EXPECT_NE(output.find("已签收"), std::string::npos);
}

TEST(Stage2ConsoleAppTest, CourierCanViewBalanceFromAccountBalanceMenu) {
    const std::string input =
        "1\n4\nadmin\nadmin\n"
        "5\n3\nc01\nCourier One\n13900000001\ncourier1\n"
        "1\n6\n"
        "1\n3\nc01\ncourier1\n"
        "2\n1\n"
        "6\n";

    const auto output = runAppWithInput("courier-account-balance", input);

    EXPECT_NE(output.find("账户余额菜单"), std::string::npos);
    EXPECT_NE(output.find("当前余额: 0.00 元。"), std::string::npos);
    EXPECT_EQ(output.find("请先使用用户身份登录。"), std::string::npos);
}

TEST(Stage2ConsoleAppTest, AdminCanViewBalanceFromAccountBalanceMenu) {
    const std::string input =
        "1\n4\nadmin\nadmin\n"
        "2\n1\n"
        "6\n";

    const auto output = runAppWithInput("admin-account-balance", input);

    EXPECT_NE(output.find("账户余额菜单"), std::string::npos);
    EXPECT_NE(output.find("当前余额: 0.00 元。"), std::string::npos);
    EXPECT_EQ(output.find("请先使用用户身份登录。"), std::string::npos);
}

}  // namespace
