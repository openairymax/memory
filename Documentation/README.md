# memory 子仓文档索引

> **子仓定位**：AirymaxOS 记忆子系统 —— MemoryRovol L1-L4 四层记忆 + CXL 内存池化 + PMEM 持久化 + MGLRU 多代 LRU。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 文档清单

### 已有文档

| 文档 | 说明 |
|------|------|
| [memoryrovol.md](memoryrovol.md) | MemoryRovol L1-L4 设计 |
| [cxl-tiering.md](cxl-tiering.md) | CXL 分层设计 |

### 待编写文档（0.1.1 → 1.0.1）

| 文档 | 说明 | 计划版本 |
|------|------|---------|
| `pmem.md` | PMEM 持久化设计 | 0.1.1 |
| `mglru.md` | MGLRU 多代 LRU 增强 | 0.1.1 |
| `vfs-persist.md` | VFS 持久化内核模块 | 1.0.1 |
| `rovol-kmod.md` | MemoryRovol 内核模块入口 | 1.0.1 |
| `forgetting-engine.md` | Forgetting Engine 遗忘机制 | 1.0.1 |

## 相关文档

- [07-directory-structure.md](../../../docs/AirymaxOS/10-architecture/07-directory-structure.md) §4.4 — memory 子仓完整目录结构
- [memory_types.h](../../kernel/include/uapi/linux/airymax/memory_types.h) — [SC] 记忆类型契约
- 顶层 [CONTRIBUTING.md](../CONTRIBUTING.md) — memory 子仓贡献指南
