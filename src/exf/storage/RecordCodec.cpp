#include "exf/storage/RecordCodec.hpp"

#include "exf/domain/User.hpp"

#include <stdexcept>
#include <utility>

namespace exf {

// 返回后续文本持久化记录使用的简单分隔符。
char RecordCodec::delimiter() {
    return '|';
}

std::vector<std::string> RecordCodec::split(std::string_view line) {
    char delim = delimiter();
    std::vector<std::string> fields;
    std::string cur;
    cur.reserve(line.size());
    bool esc = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (esc) {
            // 转义后的字符按字面值处理
            cur.push_back(c);
            esc = false;
        } else if (c == '\\') {
            esc = true;
        } else if (c == delim) {
            fields.push_back(std::move(cur));
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    // 行尾反斜杠按普通字符保留
    if (esc) {
        cur.push_back('\\');
    }
    fields.push_back(std::move(cur));
    return fields;
}

std::string RecordCodec::join(const std::vector<std::string>& fields) {
    char delim = delimiter();
    std::string out;
    // 预估容量，减少扩容次数
    size_t tot = 0;

    for (const auto& f : fields) {
        tot += f.size();
    }
    out.reserve(tot + fields.size());

    bool first = true;
    for (const auto& f : fields) {
        if (!first)
            out.push_back(delim);
        first = false;
        for (char c : f) {
            if (c == '\\' || c == delim) {
                out.push_back('\\');
            }
            out.push_back(c);
        }
    }

    return out;
}

std::string UserRecordCodec::encode(const User& user) {
    std::vector<std::string> fields = {
        user.username(), user.name(),
        user.phone(),    user.password(),
        user.address(),  exf::util::to_string(user.account().balance()),
    };
    return join(fields);
}

User UserRecordCodec::decode(const std::string_view line) {
    std::vector<std::string> fields = split(line);
    if (fields.size() != 6) {
        throw std::runtime_error("Invalid user record: " + std::string(line));
    }

    return User(fields[0],                                // 用户名
                fields[1],                                // 姓名
                fields[2],                                // 手机号
                fields[3],                                // 密码
                fields[4],                                // 地址
                exf::util::Money::from_string(fields[5])  // 初始余额
    );
}

std::string ParcelRecordCodec::encode(const Parcel& parcel) {
    std::vector<std::string> fields = {
        parcel.id(),                                       // 包裹单号
        parcel.senderUsername(),                           // 寄件人用户名
        parcel.receiverUsername(),                         // 收件人用户名
        parcel.description(),                              // 包裹描述
        parcel.sentAt(),                                   // 寄件时间
        parcel.receivedAt(),                               // 签收时间
        exf::util::to_string(parcel.fee()),                // 运费
        std::to_string(static_cast<int>(parcel.status()))  // 包裹状态
    };
    return join(fields);
}

Parcel ParcelRecordCodec::decode(std::string_view line) {
    const auto fields = split(line);
    if (fields.size() != 8) {
        throw std::runtime_error("Invalid parcel record: " + std::string(line));
    }

    return Parcel(fields[0], fields[1], fields[2], fields[3], fields[4],
                  fields[5], util::Money::from_string(fields[6]),
                  static_cast<ParcelStatus>(std::stoi(fields[7])));
}

std::string AdminRecordCodec::encode(const Admin& admin) {
    std::vector<std::string> fields = {
        admin.username(),                                // 管理员用户名
        admin.name(),                                    // 管理员显示名
        admin.password(),                                // 管理员密码
        exf::util::to_string(admin.account().balance())  // 管理员余额
    };
    return join(fields);
}

Admin AdminRecordCodec::decode(std::string_view line) {
    const auto fields = split(line);
    if (fields.size() != 4) {
        throw std::runtime_error("Invalid admin record: " + std::string(line));
    }

    return Admin(fields[0], fields[1], fields[2],
                 util::Money::from_string(fields[3]).to_double());
}

}  // namespace exf
