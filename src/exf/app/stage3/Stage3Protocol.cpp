#include "exf/app/stage3/Stage3Protocol.hpp"

#include <utility>

#include "exf/storage/RecordCodec.hpp"

namespace exf {

Stage3Request::Stage3Request(std::string command,
                             std::vector<std::string> fields)
    : command_(std::move(command)), fields_(std::move(fields)) {}

const std::string& Stage3Request::command() const {
    return command_;
}

const std::vector<std::string>& Stage3Request::fields() const {
    return fields_;
}

Stage3Response::Stage3Response(bool ok,
                               std::string code,
                               std::string message,
                               std::vector<std::string> fields)
    : ok_(ok),
      code_(std::move(code)),
      message_(std::move(message)),
      fields_(std::move(fields)) {}

Stage3Response Stage3Response::ok(std::string message,
                                  std::vector<std::string> fields) {
    return Stage3Response(true, "", std::move(message), std::move(fields));
}

Stage3Response Stage3Response::error(std::string code, std::string message) {
    return Stage3Response(false, std::move(code), std::move(message), {});
}

bool Stage3Response::isOk() const {
    return ok_;
}

const std::string& Stage3Response::code() const {
    return code_;
}

const std::string& Stage3Response::message() const {
    return message_;
}

const std::vector<std::string>& Stage3Response::fields() const {
    return fields_;
}

std::string Stage3Protocol::encodeRequest(const Stage3Request& request) {
    std::vector<std::string> fields;
    fields.reserve(request.fields().size() + 1);
    fields.push_back(request.command());
    for (const auto& field : request.fields()) {
        fields.push_back(field);
    }
    return RecordCodec::join(fields);
}

std::optional<Stage3Request> Stage3Protocol::decodeRequest(
    std::string_view line) {
    auto fields = RecordCodec::split(line);
    if (fields.empty() || fields.front().empty()) {
        return std::nullopt;
    }

    const std::string command = fields.front();
    fields.erase(fields.begin());
    return Stage3Request(command, std::move(fields));
}

std::string Stage3Protocol::encodeResponse(const Stage3Response& response) {
    std::vector<std::string> fields;
    fields.reserve(response.fields().size() + 3);
    if (response.isOk()) {
        fields.push_back("OK");
        fields.push_back(response.message());
    } else {
        fields.push_back("ERR");
        fields.push_back(response.code());
        fields.push_back(response.message());
    }
    for (const auto& field : response.fields()) {
        fields.push_back(field);
    }
    return RecordCodec::join(fields);
}

std::optional<Stage3Response> Stage3Protocol::decodeResponse(
    std::string_view line) {
    auto fields = RecordCodec::split(line);
    if (fields.size() < 2) {
        return std::nullopt;
    }
    if (fields[0] == "OK") {
        std::vector<std::string> extra(fields.begin() + 2, fields.end());
        return Stage3Response::ok(fields[1], std::move(extra));
    }
    if (fields[0] == "ERR" && fields.size() >= 3) {
        return Stage3Response::error(fields[1], fields[2]);
    }
    return std::nullopt;
}

}  // namespace exf
