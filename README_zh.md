**语言:** [English](README.md) | 简体中文

# agentrt-liunx 内存管理（AirymaxOS Memory）

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/memory)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> [agentrt-liunx（AirymaxOS）](https://atomgit.com/openairymax/agentrt-linux)（智能体操作系统）的内存子系统。
> 由 [agentrt-linux](https://atomgit.com/openairymax/agentrt-linux) 管理仓聚合的叶子仓之一。
> 复用并扩展 Airymax `heapstore` 与 `memoryrovol` 模块以提供 OS 级内存管理。

---

## 概述

**agentrt-liunx 内存管理（AirymaxOS Memory）**（`airymaxos-memory`）是 agentrt-liunx（AirymaxOS）（智能体操作系统）的内存管理子系统。它将 MemoryRovol 卷载演进引擎引入内核态，集成 CXL 与 PMEM 实现异构内存分层，并采用 MGLRU 2.0 多代 LRU 进行 AI 负载感知的页面回收。

在 agentrt-liunx 0.1.1 中，本仓库为**文档体系完成**，包含设计文档、参考发行版规范及架构草案。实际的内核与 OS 开发在 1.0.1 版本进行。

### 核心技术

- **MemoryRovol 内核态** — 直接在内核内存区域上运行的卷载与演进引擎
- **CXL（Compute Express Link）** 实现一致性、分层、可分解的内存池
- **PMEM（持久内存）** 提供低延迟持久内存与快速重启
- **MGLRU 2.0** 多代 LRU 实现 AI 负载感知的页面回收
- **HeapStore** 内核驻留堆记账，与 agentrt 运行时共享

### 与 Airymax heapstore + memoryrovol 的关系

agentrt-liunx 内存管理（AirymaxOS Memory）复用并扩展了 Airymax 运行时平台的 `heapstore` 与 `memoryrovol` 模块。堆记账、卷载日志与演进 API 在用户态运行时（agentrt）与 OS 级内存层（agentrt-liunx（AirymaxOS））之间共享，确保架构同源、无适配层。

## 仓库结构（0.1.1（文档体系完成））

```
memory/
├── README.md           # 本文件（英文）
├── README_zh.md        # 中文翻译
├── LICENSE             # AGPL-3.0 + Apache-2.0 双许可证
├── NOTICE              # 版权、商标与第三方声明
└── .gitignore
```

设计文档与参考发行版规范维护在伞仓的 `docs/AirymaxAgentOS/` 目录。

## 上下游依赖

### 上游

- **agentrt-liunx 内核（AirymaxOS Kernel）** — 提供内存管理 API、MGLRU 钩子与 PMEM/CXL 驱动
- **Airymax heapstore + memoryrovol** — 提供被复用并扩展的堆与卷载演进引擎
- **Euler 24.03 LTS / 26.03** — 内存与 CXL 标准参考发行版

### 下游

- **agentrt-liunx 认知引擎（AirymaxOS Cognition）** — 依赖分层内存进行 LLM 服务与 KV-cache 的认知引擎
- **agentrt-liunx 服务态（AirymaxOS Services）** — 从共享堆与卷载区域分配的服务层

## 分支策略

本叶子仓在 **`feature/official-hubs-01`** 分支上开发。聚合管理仓 `agentrt-linux` 保持在 `main` 分支。

## 许可证

采用 **AGPL v3 + Apache 2.0** 双许可证（SPDX：`AGPL-3.0-or-later OR Apache-2.0`）。完整文本见 [LICENSE](LICENSE)。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
