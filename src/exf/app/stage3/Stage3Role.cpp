#include "exf/app/stage3/Stage3Role.hpp"

#include <array>

namespace exf {
namespace {

struct Stage3RoleEntry {
    std::string_view text;
    Stage3Role role;
};

constexpr std::array<Stage3RoleEntry, 3> kStage3Roles{{
    {"user", Stage3Role::User},
    {"courier", Stage3Role::Courier},
    {"admin", Stage3Role::Admin},
}};

}  // namespace

std::string Stage3RoleCodec::encode(Stage3Role role) {
    switch (role) {
        case Stage3Role::User:
            return "user";
        case Stage3Role::Courier:
            return "courier";
        case Stage3Role::Admin:
            return "admin";
    }
    return "";
}

std::optional<Stage3Role> Stage3RoleCodec::decode(std::string_view text) {
    for (const auto& entry : kStage3Roles) {
        if (entry.text == text) {
            return entry.role;
        }
    }
    return std::nullopt;
}

std::string Stage3RoleCodec::displayName(Stage3Role role) {
    switch (role) {
        case Stage3Role::User:
            return "用户";
        case Stage3Role::Courier:
            return "快递员";
        case Stage3Role::Admin:
            return "管理员";
    }
    return "未知";
}

}  // namespace exf
