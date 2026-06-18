# ExpressFlow

这是我《面向对象编程实践（C++）》期末作业“物流管理系统设计与实现”的代码仓库，名字是自己起的，因为感觉 *Express* 这个单词特别酷、非常有物流的感觉。

仓库按题目一到题目三保留三条阶段分支，便于分别导出提交；最终的 `stage3-socket-cs` 分支会作为主分支 `main` 使用。

## 目标

- `stage1-basic-system`：题目一单机版，完成用户、管理员、快递、余额和文件持久化。
- `stage2-deliveryman-tasks`：题目二单机版，在题目一基础上加入快递员、物品分类、计价继承体系、待揽收状态和快递员收益。
- `stage3-socket-cs`：题目三 C/S 网络版，在题目二业务层基础上拆分客户端和服务端，通过 socket 通信，不使用 RPC 框架。

作业要求 task1/task2/task3 分别提交，所以把 stage1/stage2 仍保留为可导出的历史阶段。最终的主分支 `main` 基于 stage3-socket-cs，包含题目三的完整实现和完整的其他工具等。

## 构建和测试

普通构建：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

本地使用 Clang + libc++ 时可以打开仓库已有开关，避免无法引入我莫名其妙想用但是可能会影响构建的 `<format>` 头文件：

```bash
CC=clang CXX=clang++ cmake -S . -B build-clang-libcxx -DEXPRESSFLOW_USE_LIBCXX=ON
cmake --build build-clang-libcxx --target stage2 stage3_server stage3_client unit_tests
ctest --test-dir build-clang-libcxx --output-on-failure
```

下面是一个比较典型的编译错误，如果你编译的时候也报了这个错，那么说明你需要打开上面说的 `EXPRESSFLOW_USE_LIBCXX` 开关：

```bash
/home/rikka/ExpressFlow/src/exf/util/IdGenerator.cpp:6:10: fatal error: 'format' file not found
    6 | #include <format>
      |          ^~~~~~~~
1 error generated.
gmake[2]: *** [CMakeFiles/core.dir/build.make:314: CMakeFiles/core.dir/src/exf/util/IdGenerator.cpp.o] Error 1
gmake[1]: *** [CMakeFiles/Makefile2:92: CMakeFiles/core.dir/all] Error 2
```

## 运行

题目二单机版：

```bash
./build/stage2
```

题目三网络版需要先启动服务端，再启动客户端：

```bash
./build/stage3_server 127.0.0.1 18080 data_stage3
./build/stage3_client 127.0.0.1 18080
```

参数可省略。服务端默认使用 `0.0.0.0 18080 data_stage3`，客户端默认连接 `127.0.0.1 18080`。

## 导出提交目录

作业提交需要三个目录：`task1/`、`task2/`、`task3/`。在 stage3/main 分支运行：

```bash
tools/export_stages.sh
```

默认导出到 `exported_stages/`，也可以指定目录：

```bash
tools/export_stages.sh /tmp/expressflow-submit
```

脚本使用 `git archive` 从三个阶段分支导出提交内容，不包含当前工作区未提交文件。每个 task 目录内会生成 `EXPORT_INFO.txt`，记录来源分支和 commit。

## 目录结构

```text
apps/
  stage1/              题目一入口
  stage2/              题目二入口
  stage3_client/       题目三客户端入口
  stage3_server/       题目三服务端入口
src/exf/
  app/                 控制台和阶段应用层
  domain/              领域对象
  service/             业务流程
  repository/          内存仓储和查询
  storage/             文件读写和记录编码
  net/                 TCP socket 封装
  util/                通用工具
tests/                 GoogleTest 单元测试
tools/                 导出、数据重置和交互验证工具
docs/                  作业摘要、实验报告材料和工程补充说明
data_stage2/           题目二运行数据
data_stage3/           题目三运行数据
```

代码以 `src/` 作为 include 根目录，例如 `#include "exf/domain/User.hpp"`。

## 工具

保留了一些为了作业提交和自我检查服务的工具，希望以后自己回来看时能快速跑起来。

- `tools/export_stages.sh`：按分支导出 `task1/`、`task2/`、`task3/`。
- `tools/seed_stage3_demo_data.py`：重置 stage3 演示数据，默认写入 `data_stage3/`。
- `tools/stage3_interaction_probe.py`：启动临时 stage3 服务端并驱动客户端交互，保存 transcript。
- `tools/cli_probe.py`：通用控制台交互驱动，供 probe 脚本复用。

## 基础设施

- C++20 + CMake。
- GoogleTest，测试目标为 `unit_tests`。
- `.clang-format` 基于 Chromium，缩进为 4。
- `.clangd` 默认读取 `build/compile_commands.json`。
- `.gitignore` 忽略构建目录、运行数据目录和导出目录。

## 代码边界

- `main.cpp` 保持很薄，只解析启动参数并创建对应 app 类。
- app 层负责菜单、控制台交互和网络入口。
- service 层负责业务流程和规则。
- repository/storage 层负责持久化边界。
- domain 层表达对象自身状态和行为。
- stage3 的网络边界主要是 `TcpClient`、`TcpServer`、`TcpConnection`、`Stage3Protocol`、`Stage3RequestDispatcher` 和 `Stage3ClientService`。

具体设计细节和实现细节请参见 `docs/` 目录下的实验报告，不多讲了。
