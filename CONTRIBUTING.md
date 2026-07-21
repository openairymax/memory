# 贡献指南 - memory 子仓（记忆子系统）

> **治理依据**：[`docs/AirymaxOS/50-engineering-standards/07-maintainers-and-governance.md`](https://github.com/openairymax/docs/blob/main/AirymaxOS/50-engineering-standards/07-maintainers-and-governance.md)
> **开发流程**：[`docs/AirymaxOS/50-engineering-standards/05-development-process.md`](https://github.com/openairymax/docs/blob/main/AirymaxOS/50-engineering-standards/05-development-process.md)

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 子仓职责

memory 子仓负责 AirymaxOS 的全部记忆管理功能，包括：

- **MemoryRovol**（`memoryrovol/`）：L1-L4 四层记忆
  - L1 hot（DRAM）/ L2 warm（MGLRU aging）/ L3 cold（CXL tier）/ L4 persistent（PMEM）
- **CXL 内存池化**（`cxl/`）：FAST/CXL/PMEM/SSD 分层
- **PMEM 持久化**（`pmem/`）：persist/flush/map/unmap
- **MGLRU 多代 LRU**（`mglru/`）：多代管理 + 老化策略
- **VFS 持久化内核模块**（`vfs-persist-kern/`）
- **rovol-kmod**（`rovol-kmod/`）：MemoryRovol 内核模块入口

### IRON-9 主层

- `[IND]`：全部组件均为 agentrt-linux 专属实现

## 2. 开发环境要求

- **C 标准**：GNU C11（内核模块）
- **编译器**：gcc ≥ 11 或 clang ≥ 14
- **内核**：Linux 6.6+（内核模块构建）
- **内核头**：通过 `-I../kernel/include` 引用 [SC] 头文件

### [SC] 头文件引用约束（OS-IRON-014）

- [SC] 头文件**唯一物理宿主**：`../kernel/include/uapi/linux/airymax/`
- 本子仓通过 `-I` 引用，**禁止物理副本**
- Kbuild 配置：
  ```makefile
  ccflags-y += -I$(src)/../kernel/include
  ccflags-y += -I$(src)/../kernel/include/uapi/linux
  ```

## 3. 构建命令

```bash
# 从子仓根目录构建（需要内核源码树或 build 目录）
cd memory
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules

# 构建产物
#   memoryrovol/airy_mr_l1.ko
```

## 4. 代码规范

### 4.1 C 代码风格

- **OS-STD-FMT-001**：Tab-8 缩进（由 `.clang-format` 强制）
- **OS-STD-FMT-002**：80 列硬限制
- **GPL-2.0-only**：所有 `.c` / `.h` 文件必须包含 `SPDX-License-Identifier: GPL-2.0-only`
- **版权头**：`Copyright (c) 2025-2026 SPHARX Ltd.`
- 提交前运行 `clang-format -i` 格式化

### 4.2 记忆分层常量（[SC] memory_types.h）

| 层级 | 枚举 | GFP 标志 | 说明 |
|------|------|---------|------|
| L1 hot | `AIRY_MEM_HOT` | `AIRY_GFP_HOT` | DRAM 热数据 |
| L2 warm | `AIRY_MEM_WARM` | `AIRY_GFP_WARM` | DDR 温数据 |
| L3 cold | `AIRY_MEM_COLD` | `AIRY_GFP_COLD` | CXL/NVMe 冷数据 |
| L4 persistent | `AIRY_MEM_PMEM` | `AIRY_GFP_PMEM` | PMEM 持久数据 |

## 5. 提交规范

### 5.1 DCO 签名（OS-IRON-007 / OS-KER-068）

```bash
git commit -s
```

### 5.2 提交信息格式

```
memory: 简短描述（≤72 字符）

详细说明 what 和 why，72 字符换行。

Signed-off-by: Your Name <your.email@example.com>
```

### 5.3 子系统前缀

| 前缀 | 范围 |
|------|------|
| `memory:` | memory/ 子仓整体 |
| `memory: mr_l1:` | MemoryRovol L1 hot |
| `memory: cxl:` | CXL 内存池化 |
| `memory: pmem:` | PMEM 持久化 |
| `memory: mglru:` | MGLRU 多代 LRU |

## 6. 分支策略

- **开发分支**：`feature/official-hubs-01`
- PR 目标分支：`feature/official-hubs-01`

## 7. 测试要求

- **OS-STD-TEST-***：所有变更必须包含或更新测试
- 单元测试：随代码放在各组件目录
- 集成测试：在 `tests-linux/` 子仓
- 记忆类型测试：见 `tests-linux/unit/test_memory_types.c`

## 8. 审查流程

1. 向 `feature/official-hubs-01` 提交 PR
2. CI 运行：SSoT 校验 + Kbuild 构建 + sparse 检查 + 测试
3. 至少一名维护者审批
4. Squash-merge

## 9. 报告问题

- **Bug**：在 [memory issues](https://github.com/openairymax/memory/issues) 提交
- **设计讨论**：使用管理仓的 GitHub Discussions
