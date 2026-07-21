# CXL 分层设计

> 本文档描述 memory 子仓中 CXL 内存池化与分层设计。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 设计目标

利用 CXL（Compute Express Link）实现内存池化与分层，
扩展 MemoryRovol 的容量 beyond DRAM。

## 2. CXL 分层模型

| 层级 | 介质 | 延迟 | 容量 | 用途 |
|------|------|------|------|------|
| FAST | DRAM | ~80ns | 小 | L1 hot / L2 warm |
| CXL | CXL.mem | ~170ns | 中 | L3 cold |
| PMEM | PMEM | ~300ns | 大 | L4 persistent |
| SSD | NVMe SSD | ~10μs | 超大 | 冷存储 |

## 3. 迁移策略

- **升级（promotion）**：访问频繁的 L3 cold 数据迁移到 L1/L2
- **降级（demotion）**：L1/L2 aging 后迁移到 L3 cold
- **持久化（persist）**：L3 cold 迁移到 L4 PMEM

## 4. 0.1.1 实现状态

- ⏳ CXL 内存池化：1.0.1 补齐
- 0.1.1 仅提供 [SC] 类型契约（`memory_types.h`）

## 5. 相关文档

- [07-directory-structure.md](../../../docs/AirymaxOS/10-architecture/07-directory-structure.md) §4.4 — memory 子仓完整目录结构
- [memory_types.h](../../kernel/include/uapi/linux/airymax/memory_types.h) — [SC] 记忆类型契约
