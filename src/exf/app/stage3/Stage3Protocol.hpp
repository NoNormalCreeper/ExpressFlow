#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace exf {

/**
 * Stage3Request 表示客户端发往服务端的一条文本协议请求。
 */
class Stage3Request {
   public:
    /**
     * 用命令和字段列表创建请求。
     */
    Stage3Request(std::string command, std::vector<std::string> fields);

    /**
     * 返回请求命令。
     */
    const std::string& command() const;

    /**
     * 返回请求字段。
     */
    const std::vector<std::string>& fields() const;

   private:
    /** 请求命令。 */
    std::string command_;

    /** 命令参数字段。 */
    std::vector<std::string> fields_;
};

/**
 * Stage3Response 表示服务端返回给客户端的一条文本协议响应。
 */
class Stage3Response {
   public:
    /**
     * 创建成功响应。
     */
    static Stage3Response ok(std::string message,
                             std::vector<std::string> fields = {});

    /**
     * 创建失败响应。
     */
    static Stage3Response error(std::string code, std::string message);

    /**
     * 返回响应是否成功。
     */
    bool isOk() const;

    /**
     * 返回错误码；成功响应为空字符串。
     */
    const std::string& code() const;

    /**
     * 返回响应消息。
     */
    const std::string& message() const;

    /**
     * 返回附加字段。
     */
    const std::vector<std::string>& fields() const;

   private:
    /** 创建响应对象。 */
    Stage3Response(bool ok,
                   std::string code,
                   std::string message,
                   std::vector<std::string> fields);

    /** 成功标记。 */
    bool ok_;

    /** 错误码。 */
    std::string code_;

    /** 显示消息。 */
    std::string message_;

    /** 附加字段。 */
    std::vector<std::string> fields_;
};

/**
 * Stage3Protocol 负责阶段 3 文本协议的一行请求/响应编码。
 */
class Stage3Protocol {
   public:
    /**
     * 编码请求为单行文本，不包含换行符。
     */
    static std::string encodeRequest(const Stage3Request& request);

    /**
     * 解码一行请求文本。
     */
    static std::optional<Stage3Request> decodeRequest(std::string_view line);

    /**
     * 编码响应为单行文本，不包含换行符。
     */
    static std::string encodeResponse(const Stage3Response& response);

    /**
     * 解码一行响应文本。
     */
    static std::optional<Stage3Response> decodeResponse(std::string_view line);
};

}  // namespace exf
