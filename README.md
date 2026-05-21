# ExpressFlow

ExpressFlow 是《面向对象编程实践（C++）》期末作业“物流管理系统设计与实现”的工程骨架。当前仓库处于题目一初始化阶段，只搭建可编译、可测试、可继续演进的基础结构，尚未实现完整业务功能。

## 题目演进

- `stage1`：单机版，后续实现用户注册/登录、修改密码、余额管理、发送快递、接收快递、查询快递、管理员物流业务管理。
- `stage2`：在题目一基础上加入快递员、快递分类、物品继承体系、待揽收状态、管理员分配快递员、快递员揽收收益。
- `stage3`：在题目二基础上改为传统 C/S socket 网络版，客户端和服务器为不同进程，不使用 RPC 框架。

## 当前阶段

当前只初始化题目一工程骨架：

- `core`：业务核心库，包含 `domain`、`service`、`repository`、`storage`、`util`。
- `stage1`：题目一控制台入口，包含 `apps/stage1/main.cpp` 和 `src/exf/app/stage1/Stage1ConsoleApp`。
- `unit_tests`：最小单元测试目标，链接 `core`。

`core` 不包含 `main()`，不包含 `app/`，不直接使用 `cin/cout`。控制台输入输出只放在应用入口层。

## 目录结构

本项目把 `.hpp` 和 `.cpp` 放在同一个模块目录中，便于课程作业阶段频繁修改类声明和实现。

```text
.
├─ CMakeLists.txt
├─ README.md
├─ apps/
│  └─ stage1/
│     └─ main.cpp
├─ src/
│  └─ exf/
│     ├─ domain/
│     ├─ service/
│     ├─ repository/
│     ├─ storage/
│     ├─ util/
│     └─ app/
│        └─ stage1/
├─ tests/
│  └─ test_smoke.cpp
├─ docs/
│  └─ assignment_summary.md
└─ data/
   └─ sample/
```

代码仍然使用 `#include "exf/domain/User.hpp"` 这样的路径；CMake 将 `src/` 作为 include 根目录。

## 技术选择

- C++20
- CMake
- CLI/控制台菜单
- 文件持久化
- 简单单元测试
- 后续题目三预留 socket 网络版演进方向

## 构建

```bash
cmake -S . -B build
cmake --build build
```

如果本地环境的 `ccache` 无法写入缓存目录，可以显式指定系统编译器：

```bash
cmake -S . -B build -DCMAKE_CXX_COMPILER=/usr/bin/c++
cmake --build build
```

## 运行

```bash
./build/stage1
```

Windows 环境下可运行生成的 `stage1.exe`。

## 测试

```bash
ctest --test-dir build
```

也可以直接运行：

```bash
./build/unit_tests
```

## 设计原则

- `main.cpp` 保持很薄，只启动对应应用类。
- 菜单层只负责输入、输出和调用 service。
- service 负责业务流程。
- repository/storage 负责持久化边界。
- domain 表达领域对象和对象自身行为。
- util 只放不依赖界面的通用工具。
- 题目一代码不提前加入快递员、待揽收状态或网络层。
