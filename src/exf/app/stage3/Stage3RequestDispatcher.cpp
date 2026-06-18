#include "exf/app/stage3/Stage3RequestDispatcher.hpp"

#include <array>
#include <exception>
#include <string>
#include <string_view>
#include <utility>

#include "exf/storage/RecordCodec.hpp"

namespace exf {
namespace {

struct ParcelItemTypeEntry {
    std::string_view text;
    ParcelItemType type;
};

constexpr std::array<ParcelItemTypeEntry, 3> kParcelItemTypes{{
    {"standard", ParcelItemType::Standard},
    {"fragile", ParcelItemType::Fragile},
    {"book", ParcelItemType::Book},
}};

}  // namespace

// 构造请求分发器并绑定业务服务。
Stage3RequestDispatcher::Stage3RequestDispatcher(UserService& users,
                                                 AdminService& admins,
                                                 CourierService& couriers,
                                                 ParcelService& parcels,
                                                 Stage3SessionManager& sessions)
    : users_(users),
      admins_(admins),
      couriers_(couriers),
      parcels_(parcels),
      sessions_(sessions) {}

// 解码一行请求并编码响应。
std::string Stage3RequestDispatcher::handleLine(const std::string& line) {
    try {
        const auto request = Stage3Protocol::decodeRequest(line);
        if (!request.has_value()) {
            return Stage3Protocol::encodeResponse(
                Stage3Response::error("BadRequest", "请求格式错误"));
        }
        return Stage3Protocol::encodeResponse(handleRequest(*request));
    } catch (const std::exception&) {
        return Stage3Protocol::encodeResponse(
            Stage3Response::error("StorageError",
                                  "数据读写失败或数据文件损坏"));
    }
}

// 按命令类型分派已解码请求。
Stage3Response Stage3RequestDispatcher::handleRequest(
    const Stage3Request& request) {
    std::lock_guard<std::mutex> lock(businessMutex_);

    const auto& command = request.command();
    const auto& fields = request.fields();
    switch (parseStage3Command(command)) {
        case Stage3Command::Login:
            return handleLogin(fields);
        case Stage3Command::SendParcel:
            return handleSendParcel(fields);
        case Stage3Command::ListUnassigned:
            return handleListUnassigned(fields);
        case Stage3Command::AssignCourier:
            return handleAssignCourier(fields);
        case Stage3Command::ListUsers:
            return handleListUsers(fields);
        case Stage3Command::ListCouriers:
            return handleListCouriers(fields);
        case Stage3Command::AddCourier:
            return handleAddCourier(fields);
        case Stage3Command::DeleteCourier:
            return handleDeleteCourier(fields);
        case Stage3Command::ListAllParcels:
            return handleListAllParcels(fields);
        case Stage3Command::ListMyParcels:
            return handleListMyParcels(fields);
        case Stage3Command::ListCourierParcels:
            return handleListCourierParcels(fields);
        case Stage3Command::ListPickupTasks:
            return handleListPickupTasks(fields);
        case Stage3Command::Pickup:
            return handlePickup(fields);
        case Stage3Command::ListWaitingSign:
            return handleListWaitingSign(fields);
        case Stage3Command::Sign:
            return handleSign(fields);
        case Stage3Command::Quit:
            return handleQuit(fields);
        case Stage3Command::Unknown:
            return Stage3Response::error("UnknownCommand", "未知请求命令");
    }
    return Stage3Response::error("UnknownCommand", "未知请求命令");
}

// 将协议命令文本转换为内部命令枚举。
Stage3RequestDispatcher::Stage3Command
Stage3RequestDispatcher::parseStage3Command(std::string_view text) {
    struct Entry {
        std::string_view text;
        Stage3Command command;
    };

    constexpr std::array<Entry, 16> commands{{
        {"LOGIN", Stage3Command::Login},
        {"SEND_PARCEL", Stage3Command::SendParcel},
        {"LIST_UNASSIGNED", Stage3Command::ListUnassigned},
        {"ASSIGN_COURIER", Stage3Command::AssignCourier},
        {"LIST_USERS", Stage3Command::ListUsers},
        {"LIST_COURIERS", Stage3Command::ListCouriers},
        {"ADD_COURIER", Stage3Command::AddCourier},
        {"DELETE_COURIER", Stage3Command::DeleteCourier},
        {"LIST_ALL_PARCELS", Stage3Command::ListAllParcels},
        {"LIST_MY_PARCELS", Stage3Command::ListMyParcels},
        {"LIST_COURIER_PARCELS", Stage3Command::ListCourierParcels},
        {"LIST_PICKUP_TASKS", Stage3Command::ListPickupTasks},
        {"PICKUP", Stage3Command::Pickup},
        {"LIST_WAITING_SIGN", Stage3Command::ListWaitingSign},
        {"SIGN", Stage3Command::Sign},
        {"QUIT", Stage3Command::Quit},
    }};

    for (const auto& entry : commands) {
        if (entry.text == text) {
            return entry.command;
        }
    }
    return Stage3Command::Unknown;
}

// 处理登录请求并创建会话。
Stage3Response Stage3RequestDispatcher::handleLogin(
    const std::vector<std::string>& fields) {
    if (fields.size() != 3) {
        return Stage3Response::error("BadRequest", "登录参数数量错误");
    }

    const auto& role = fields[0];
    const auto& username = fields[1];
    const auto& password = fields[2];
    const auto decodedRole = Stage3RoleCodec::decode(role);
    if (!decodedRole.has_value()) {
        return Stage3Response::error("InvalidRole", "登录角色不支持");
    }

    switch (*decodedRole) {
        case Stage3Role::User: {
            const auto err = users_.loginUser(username, password);
            if (err != UserServiceError::Nil) {
                return userError(err);
            }
            return Stage3Response::ok(
                "用户登录成功",
                {sessions_.createSession(Stage3Role::User, username),
                 Stage3RoleCodec::encode(Stage3Role::User), username});
        }
        case Stage3Role::Courier: {
            const auto err = couriers_.loginCourier(username, password);
            if (err != CourierServiceError::Nil) {
                return courierError(err);
            }
            return Stage3Response::ok(
                "快递员登录成功",
                {sessions_.createSession(Stage3Role::Courier, username),
                 Stage3RoleCodec::encode(Stage3Role::Courier), username});
        }
        case Stage3Role::Admin:
            if (!admins_.loginAdmin(username, password)) {
                return Stage3Response::error("IncorrectPassword",
                                             "管理员账号或密码错误");
            }
            return Stage3Response::ok(
                "管理员登录成功",
                {sessions_.createSession(Stage3Role::Admin, username),
                 Stage3RoleCodec::encode(Stage3Role::Admin), username});
    }
    return Stage3Response::error("InvalidRole", "登录角色不支持");
}

// 处理用户寄件请求。
Stage3Response Stage3RequestDispatcher::handleSendParcel(
    const std::vector<std::string>& fields) {
    if (fields.size() != 5) {
        return Stage3Response::error("BadRequest", "寄件参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::User);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用用户身份登录");
    }

    const auto itemType = parseItemType(fields[2]);
    if (!itemType.has_value()) {
        return Stage3Response::error("InvalidItem", "快递类型不支持");
    }
    double amount = 0.0;
    try {
        amount = std::stod(fields[3]);
    } catch (const std::exception&) {
        return Stage3Response::error("InvalidItem", "计费数量不合法");
    }
    const auto item = makeItem(*itemType, amount);
    if (!item || !item->isValid()) {
        return Stage3Response::error("InvalidItem", "计费数量不合法");
    }

    const auto [parcelId, err] =
        parcels_.sendParcel(session->username(), fields[1], fields[4], *item);
    if (err != ParcelServiceError::Nil) {
        return parcelError(err);
    }
    return Stage3Response::ok("寄件成功", {parcelId});
}

// 查询等待管理员分配快递员的快递。
Stage3Response Stage3RequestDispatcher::handleListUnassigned(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    ParcelQuery query;
    query.status = ParcelStatus::WaitingForPickup;
    query.courierUsername = "";
    const auto parcels = parcels_.queryAdminParcels(query);
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 处理管理员分配快递员请求。
Stage3Response Stage3RequestDispatcher::handleAssignCourier(
    const std::vector<std::string>& fields) {
    if (fields.size() != 3) {
        return Stage3Response::error("BadRequest", "分配参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto err = parcels_.assignCourier(fields[1], fields[2]);
    if (err != ParcelServiceError::Nil) {
        return parcelError(err);
    }
    return Stage3Response::ok("快递员分配成功");
}

// 查询快递员列表。
Stage3Response Stage3RequestDispatcher::handleListCouriers(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto couriers = couriers_.listCouriers();
    std::vector<std::string> encoded;
    encoded.reserve(couriers.size());
    for (const auto& courier : couriers) {
        encoded.push_back(encodeCourier(courier));
    }
    return Stage3Response::ok(std::to_string(couriers.size()), encoded);
}

// 查询用户列表。
Stage3Response Stage3RequestDispatcher::handleListUsers(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto users = admins_.listUsers();
    std::vector<std::string> encoded;
    encoded.reserve(users.size());
    for (const auto& user : users) {
        encoded.push_back(encodeUser(user));
    }
    return Stage3Response::ok(std::to_string(users.size()), encoded);
}

// 处理管理员新增快递员请求。
Stage3Response Stage3RequestDispatcher::handleAddCourier(
    const std::vector<std::string>& fields) {
    if (fields.size() != 5) {
        return Stage3Response::error("BadRequest", "添加快递员参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto err =
        couriers_.addCourier(fields[1], fields[2], fields[3], fields[4]);
    if (err != CourierServiceError::Nil) {
        return courierError(err);
    }
    return Stage3Response::ok("快递员添加成功");
}

// 处理管理员删除快递员请求。
Stage3Response Stage3RequestDispatcher::handleDeleteCourier(
    const std::vector<std::string>& fields) {
    if (fields.size() != 2) {
        return Stage3Response::error("BadRequest", "删除快递员参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto err = couriers_.deleteCourier(fields[1]);
    if (err != CourierServiceError::Nil) {
        return courierError(err);
    }
    return Stage3Response::ok("快递员删除成功");
}

// 查询全部快递。
Stage3Response Stage3RequestDispatcher::handleListAllParcels(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::Admin);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用管理员身份登录");
    }

    const auto parcels = parcels_.queryAdminParcels(ParcelQuery{});
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 查询当前用户相关快递。
Stage3Response Stage3RequestDispatcher::handleListMyParcels(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::User);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用用户身份登录");
    }

    const auto parcels = parcels_.queryUserParcels(
        session->username(), UserParcelView::Related, ParcelQuery{});
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 查询指定或当前快递员相关快递。
Stage3Response Stage3RequestDispatcher::handleListCourierParcels(
    const std::vector<std::string>& fields) {
    const auto session = requireAnySession(fields);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先登录");
    }

    std::string courierUsername = session->username();
    switch (session->role()) {
        case Stage3Role::Admin:
            if (fields.size() != 2) {
                return Stage3Response::error("BadRequest",
                                             "快递员参数数量错误");
            }
            courierUsername = fields[1];
            break;
        case Stage3Role::Courier:
            break;
        case Stage3Role::User:
            return Stage3Response::error("Unauthorized",
                                         "请使用快递员或管理员身份");
    }

    const auto parcels = parcels_.queryCourierParcels(
        courierUsername, CourierParcelView::Related, ParcelQuery{});
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 查询快递员待揽收任务。
Stage3Response Stage3RequestDispatcher::handleListPickupTasks(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::Courier);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用快递员身份登录");
    }

    const auto parcels = parcels_.queryCourierParcels(
        session->username(), CourierParcelView::AssignedWaitingForPickup,
        ParcelQuery{});
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 处理快递员揽收请求。
Stage3Response Stage3RequestDispatcher::handlePickup(
    const std::vector<std::string>& fields) {
    if (fields.size() != 2) {
        return Stage3Response::error("BadRequest", "揽收参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::Courier);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用快递员身份登录");
    }

    const auto err = parcels_.pickupParcel(session->username(), fields[1]);
    if (err != ParcelServiceError::Nil) {
        return parcelError(err);
    }
    return Stage3Response::ok("揽收成功");
}

// 查询用户待签收快递。
Stage3Response Stage3RequestDispatcher::handleListWaitingSign(
    const std::vector<std::string>& fields) {
    const auto session = requireSession(fields, Stage3Role::User);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用用户身份登录");
    }

    const auto parcels = parcels_.queryUserParcels(
        session->username(), UserParcelView::WaitingForSign, ParcelQuery{});
    std::vector<std::string> encoded;
    encoded.reserve(parcels.size());
    for (const auto& parcel : parcels) {
        encoded.push_back(encodeParcel(parcel));
    }
    return Stage3Response::ok(std::to_string(parcels.size()), encoded);
}

// 处理用户签收请求。
Stage3Response Stage3RequestDispatcher::handleSign(
    const std::vector<std::string>& fields) {
    if (fields.size() != 2) {
        return Stage3Response::error("BadRequest", "签收参数数量错误");
    }
    const auto session = requireSession(fields, Stage3Role::User);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "请先使用用户身份登录");
    }

    const auto err = parcels_.signParcel(session->username(), fields[1]);
    if (err != ParcelServiceError::Nil) {
        return parcelError(err);
    }
    return Stage3Response::ok("签收成功");
}

// 处理退出登录请求。
Stage3Response Stage3RequestDispatcher::handleQuit(
    const std::vector<std::string>& fields) {
    const auto session = requireAnySession(fields);
    if (!session.has_value()) {
        return Stage3Response::error("Unauthorized", "当前未登录");
    }
    sessions_.removeSession(fields[0]);
    return Stage3Response::ok("已退出登录");
}

// 校验指定角色的登录会话。
std::optional<Stage3Session> Stage3RequestDispatcher::requireSession(
    const std::vector<std::string>& fields,
    Stage3Role role) const {
    const auto session = requireAnySession(fields);
    if (!session.has_value() || session->role() != role) {
        return std::nullopt;
    }
    return session;
}

// 校验任意角色的登录会话。
std::optional<Stage3Session> Stage3RequestDispatcher::requireAnySession(
    const std::vector<std::string>& fields) const {
    if (fields.empty()) {
        return std::nullopt;
    }
    return sessions_.findSession(fields[0]);
}

// 根据物品类型创建对应物品对象。
std::unique_ptr<Item> Stage3RequestDispatcher::makeItem(ParcelItemType type,
                                                        double amount) {
    switch (type) {
        case ParcelItemType::Standard:
            return std::make_unique<StandardItem>(amount);
        case ParcelItemType::Fragile:
            return std::make_unique<FragileItem>(amount);
        case ParcelItemType::Book:
            return std::make_unique<BookItem>(static_cast<int>(amount));
    }
    return std::make_unique<BookItem>(static_cast<int>(amount));
}

// 解析物品类型协议字段。
std::optional<ParcelItemType> Stage3RequestDispatcher::parseItemType(
    const std::string& text) {
    for (const auto& entry : kParcelItemTypes) {
        if (entry.text == text) {
            return entry.type;
        }
    }
    return std::nullopt;
}

// 将快递对象编码为协议字段。
std::string Stage3RequestDispatcher::encodeParcel(const Parcel& parcel) {
    return ParcelRecordCodec::encode(parcel);
}

// 将快递员对象编码为协议字段。
std::string Stage3RequestDispatcher::encodeCourier(const Courier& courier) {
    return CourierRecordCodec::encode(courier);
}

// 将用户对象编码为协议字段。
std::string Stage3RequestDispatcher::encodeUser(const User& user) {
    return UserRecordCodec::encode(user);
}

// 将快递服务错误转换为协议响应。
Stage3Response Stage3RequestDispatcher::parcelError(ParcelServiceError error) {
    switch (error) {
        case ParcelServiceError::SenderNotFound:
            return Stage3Response::error("SenderNotFound", "发件人不存在");
        case ParcelServiceError::ReceiverNotFound:
            return Stage3Response::error("ReceiverNotFound", "收件人不存在");
        case ParcelServiceError::AdminNotFound:
            return Stage3Response::error("AdminNotFound", "管理员不存在");
        case ParcelServiceError::InsufficientBalance:
            return Stage3Response::error("InsufficientBalance", "余额不足");
        case ParcelServiceError::ParcelNotFound:
            return Stage3Response::error("ParcelNotFound", "快递不存在");
        case ParcelServiceError::NotReceiver:
            return Stage3Response::error("NotReceiver", "只能签收自己的快递");
        case ParcelServiceError::AlreadySigned:
            return Stage3Response::error("AlreadySigned", "快递已签收");
        case ParcelServiceError::InvalidDescription:
            return Stage3Response::error("InvalidDescription",
                                         "快递描述不能为空");
        case ParcelServiceError::CourierNotFound:
            return Stage3Response::error("CourierNotFound", "快递员不存在");
        case ParcelServiceError::InvalidItem:
            return Stage3Response::error("InvalidItem", "快递类型或数量不合法");
        case ParcelServiceError::NotWaitingForPickup:
            return Stage3Response::error("NotWaitingForPickup",
                                         "快递不处于待揽收状态");
        case ParcelServiceError::CourierAlreadyAssigned:
            return Stage3Response::error("CourierAlreadyAssigned",
                                         "快递已分配快递员");
        case ParcelServiceError::CourierNotAssigned:
            return Stage3Response::error("CourierNotAssigned",
                                         "快递尚未分配快递员");
        case ParcelServiceError::NotAssignedCourier:
            return Stage3Response::error("NotAssignedCourier",
                                         "该快递不属于当前快递员");
        case ParcelServiceError::AlreadyPickedUp:
            return Stage3Response::error("AlreadyPickedUp", "快递已揽收");
        case ParcelServiceError::NotWaitingForSign:
            return Stage3Response::error("NotWaitingForSign",
                                         "快递不处于待签收状态");
        case ParcelServiceError::AdminInsufficientBalance:
            return Stage3Response::error("AdminInsufficientBalance",
                                         "管理员余额不足");
        case ParcelServiceError::Nil:
            return Stage3Response::ok("成功");
    }
    return Stage3Response::error("ParcelError", "快递操作失败");
}

// 将用户服务错误转换为协议响应。
Stage3Response Stage3RequestDispatcher::userError(UserServiceError error) {
    switch (error) {
        case UserServiceError::UserNotFound:
            return Stage3Response::error("UserNotFound", "用户不存在");
        case UserServiceError::IncorrectPassword:
            return Stage3Response::error("IncorrectPassword", "密码错误");
        case UserServiceError::UserAlreadyExists:
            return Stage3Response::error("UserAlreadyExists", "用户已存在");
        case UserServiceError::InvalidInitialBalance:
            return Stage3Response::error("InvalidInitialBalance",
                                         "初始余额不合法");
        case UserServiceError::Nil:
            return Stage3Response::ok("成功");
    }
    return Stage3Response::error("UserError", "用户操作失败");
}

// 将快递员服务错误转换为协议响应。
Stage3Response Stage3RequestDispatcher::courierError(
    CourierServiceError error) {
    switch (error) {
        case CourierServiceError::CourierNotFound:
            return Stage3Response::error("CourierNotFound", "快递员不存在");
        case CourierServiceError::IncorrectPassword:
            return Stage3Response::error("IncorrectPassword", "密码错误");
        case CourierServiceError::CourierAlreadyExists:
            return Stage3Response::error("CourierAlreadyExists",
                                         "快递员已存在");
        case CourierServiceError::InvalidInitialBalance:
            return Stage3Response::error("InvalidInitialBalance",
                                         "初始余额不合法");
        case CourierServiceError::InvalidName:
            return Stage3Response::error("InvalidName", "姓名不合法");
        case CourierServiceError::InvalidPhone:
            return Stage3Response::error("InvalidPhone", "手机号不合法");
        case CourierServiceError::InvalidPassword:
            return Stage3Response::error("InvalidPassword", "密码不合法");
        case CourierServiceError::Nil:
            return Stage3Response::ok("成功");
    }
    return Stage3Response::error("CourierError", "快递员操作失败");
}

}  // namespace exf
