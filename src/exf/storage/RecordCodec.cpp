#include "exf/storage/RecordCodec.hpp"
#include "exf/domain/User.hpp"

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
            // accept any escaped character literally (e.g. \ or \|)
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
    // if line ends with escape, treat trailing backslash as literal
    if (esc) {
        cur.push_back('\\');
    }
    fields.push_back(std::move(cur));
    return fields;
}

std::string RecordCodec::join(const std::vector<std::string>& fields) {
    char delim = delimiter();
    std::string out;
    // estimate size
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
    return User(fields[0],                                // username
                fields[1],                                // name
                fields[2],                                // phone
                fields[3],                                // password
                fields[4],                                // address
                exf::util::Money::from_string(fields[5])  // initial balance
    );
}

}  // namespace exf
