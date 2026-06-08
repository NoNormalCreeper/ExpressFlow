# ExpressFlow

ExpressFlow 是《面向对象编程实践（C++）》期末作业“物流管理系统设计与实现”的阶段式实现。当前工作树面向题目二单机版开发；题目一版本保留在前序提交中。

## 题目演进

- `stage1`：题目一单机版，保留在前序提交中。
- `stage2`：当前工作树，在题目一基础上加入快递员、快递分类、物品继承体系、待揽收状态、管理员分配快递员、快递员揽收收益。
- `stage3`：在题目二基础上改为传统 C/S socket 网络版，客户端和服务器为不同进程，不使用 RPC 框架。

## 当前阶段

当前题目二核心领域层和服务层已开始落地：

- `core`：业务核心库，包含 `domain`、`service`、`repository`、`storage`、`util`。
- `stage2`：题目二控制台入口，包含 `apps/stage2/main.cpp` 和 `src/exf/app/stage2/Stage2ConsoleApp`。
- `unit_tests`：单元测试目标，覆盖领域对象、仓储、服务、存储和主要控制台路径。

`core` 不包含 `main()`，不包含 `app/`，不直接使用 `cin/cout`。控制台输入输出只放在应用入口层。

## 目录结构

本项目把 `.hpp` 和 `.cpp` 放在同一个模块目录中，便于课程作业阶段频繁修改类声明和实现。

```text
.
├─ CMakeLists.txt
├─ README.md
├─ apps/
│  └─ stage2/
│     └─ main.cpp
├─ src/
│  └─ exf/
│     ├─ domain/
│     ├─ service/
│     ├─ repository/
│     ├─ storage/
│     ├─ util/
│     └─ app/
│        └─ stage2/
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
./build/stage2
```

Windows 环境下可运行生成的 `stage2.exe`。

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
- 题目二代码围绕快递员、物品分类、待揽收和揽收分账流程展开。
