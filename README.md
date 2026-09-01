# ExpressFlow

ExpressFlow 是我们在《面向对象编程实践（C++）》课程中完成的物流管理系统。课程要求把同一组业务分三步实现，我们先做单机控制台程序，再加入快递员和物品分类，最后把客户端与服务端拆成两个进程，通过 TCP socket 通信。

这个仓库保留了三个阶段的代码和最终报告，适合了解一个小型 C++ 项目如何从领域对象、文件存储和业务服务，逐步扩展到网络入口。它仍然是课程作业，数据规模、并发模型和安全措施都按演示需求取舍。

## 完成内容

| 阶段 | 分支 | 主要内容 |
| --- | --- | --- |
| Stage 1 | `stage1-basic-system` | 用户、管理员、寄件、签收、余额和文件持久化 |
| Stage 2 | `stage2-deliveryman-tasks` | 增加快递员、物品分类、多态计价、分配和揽收流程 |
| Stage 3 | `stage3-socket-cs`、`main` | 拆分客户端与服务端，加入 TCP 文本协议、登录会话和多客户端处理 |

最终版本覆盖了用户注册和登录、充值、寄件、查询和签收；管理员可以管理快递员并分配待揽收快递；快递员可以查看任务、揽收快递并获得分账。用户、快递员、管理员和快递记录都保存在文本文件中。

实现时，我们把业务规则放在 `service` 层，把菜单和 socket 当作两种入口。Stage 3 收到请求后仍然调用 Stage 2 使用的业务服务，没有再写一套寄件、揽收和签收逻辑。金额由 `Money` 类型统一处理，物品价格通过 `Item::getPrice()` 的派生类实现，文本记录会转义字段中的分隔符。

## 构建

需要 CMake 3.20 以上版本、提供 C++20 `<format>` 的标准库，以及 Linux 或其他支持 POSIX socket 的环境。配置时如果系统中没有 GoogleTest，CMake 会下载 GoogleTest 1.14.0。

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

当前分支会生成 `stage2`、`stage3_server`、`stage3_client` 和 `unit_tests`。Stage 1 的完整程序保留在 `stage1-basic-system` 分支。

如果 Clang 环境找不到标准库中的 `<format>`，可以改用仓库提供的 libc++ 开关：

```bash
CC=clang CXX=clang++ cmake -S . -B build-clang-libcxx -DEXPRESSFLOW_USE_LIBCXX=ON
cmake --build build-clang-libcxx
```

## 运行

运行 Stage 2 单机版：

```bash
./build/stage2
```

运行 Stage 3 时，先启动服务端，再在另一个终端启动客户端：

```bash
./build/stage3_server 127.0.0.1 18080 data_stage3
./build/stage3_client 127.0.0.1 18080
```

服务端参数依次为监听地址、端口和数据目录，默认值是 `0.0.0.0`、`18080` 和 `data_stage3`。客户端默认连接 `127.0.0.1:18080`。

仓库附带了一组 Stage 3 演示数据，可以直接使用下面的账号：

| 角色 | 用户名 | 密码 |
| --- | --- | --- |
| 用户 | `u1` | `123456` |
| 快递员 | `c1` | `123456` |
| 管理员 | `admin` | `admin` |

程序会直接修改所选数据目录中的文本文件。需要恢复 Stage 3 演示数据时运行：

```bash
python3 tools/seed_stage3_demo_data.py
```

## 测试方式

单元测试覆盖领域对象、repository、service、文件编码、控制台输入、Stage 3 协议、会话、请求分发和 TCP 连接。我们还写了一个交互探针，用真实的服务端和客户端依次完成寄件、分配、揽收和签收：

```bash
python3 tools/stage3_interaction_probe.py \
  --build-dir build \
  --out-dir /tmp/expressflow-probe
```

单元测试用于检查局部规则，交互探针用于确认菜单、网络协议和业务流程能够一起工作。探针会把演示数据复制到临时目录，不会修改 `data_stage3/`。

## 代码结构

```text
apps/                   各阶段的程序入口
src/exf/domain/         用户、快递、账户和物品等领域对象
src/exf/service/        注册、寄件、分配、揽收和签收等业务流程
src/exf/repository/     对象查询和持久化入口
src/exf/storage/        文本文件读写与记录编码
src/exf/net/            TCP client、server 和 connection 封装
src/exf/app/            控制台交互、Stage 3 协议与请求分发
tests/                  GoogleTest 测试
tools/                  阶段导出、演示数据和交互验证脚本
docs/                   题目摘要、实验报告和补充说明
```

`main.cpp` 只负责解析参数和启动 app。菜单或网络请求进入 app 层后调用 service，service 再通过 repository 读写领域对象和文件。这是三个阶段能够继续复用业务代码的基础。

## 当前限制

- 服务端采用一连接一线程，最多同时接受 32 个客户端，业务请求通过一把全局锁串行执行。
- 文件存储没有事务能力，涉及多个文件的更新不能保证数据库级原子性。
- 登录 token 只保存在服务端内存中，没有过期时间，服务端重启后需要重新登录。
- 协议是按行传输的自定义文本格式，没有版本、请求 ID 和幂等控制。
- 演示账号的密码以明文保存在文件中，不适合保存真实信息。

这些限制来自课程规模和实现时间。Stage 3 只补充了连接数限制、单账号单会话、有限重连和单条消息长度限制，没有继续引入数据库、线程池或 `epoll`。

## 文档与阶段代码

- [作业要求摘要](docs/assignment_summary.md) 记录三个阶段的题目要求。
- [实验报告](docs/实验报告.md) 包含整体设计、类关系、协议和开发过程。
- [Stage 3 工程补充说明](docs/stage3_engineering_notes.md) 记录网络版的取舍和剩余问题。

课程提交要求三个独立目录。需要复现当时的提交结构时，可以在最终分支运行：

```bash
tools/export_stages.sh /tmp/expressflow-stages
```

脚本会从三个阶段分支导出代码，并在每个目录中记录来源 commit。它只读取已经提交的内容。
