**Language:** English | [简体中文](README_zh.md)

# agentrt-linux Memory (AirymaxOS Memory)

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/memory)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> Memory subsystem of [agentrt-linux（AirymaxOS）](https://atomgit.com/openairymax/agentrt-linux) — the AI Agent Operating System.
> One of the leaf repositories aggregated by the [agentrt-linux](https://atomgit.com/openairymax/agentrt-linux) management repo.
> Reuses and extends the Airymax `heapstore` and `memoryrovol` modules for OS-level memory management.

---

## Overview

The **agentrt-linux Memory (AirymaxOS Memory)** (`airymaxos-memory`) is the memory management subsystem of agentrt-linux（AirymaxOS）, the AI Agent Operating System. It brings the MemoryRovol rolling-and-evolution engine into the kernel, integrates CXL and PMEM for heterogeneous memory tiering, and adopts the MGLRU (multi-generational LRU) for AI workload-aware page reclaim.

In agentrt-linux 0.1.1, this repository is **documentation complete** (文档体系完成) containing design documents, reference distribution specifications, and architectural drafts. Actual kernel and OS development takes place in version 1.0.1.

### Core Technologies

- **MemoryRovol in-kernel mode** — rolling & evolution engine operating directly on kernel memory regions
- **CXL (Compute Express Link)** for coherent, tiered, disaggregated memory pools
- **PMEM (persistent memory)** for low-latency durable memory and fast restart
- **MGLRU** (multi-generational LRU) for AI workload-aware page reclaim
- **HeapStore** kernel-resident heap accounting shared with the agentrt runtime

### Relationship with Airymax heapstore + memoryrovol

The agentrt-linux Memory (AirymaxOS Memory) reuses and extends the `heapstore` and `memoryrovol` modules from the Airymax runtime platform. The heap accounting, rolling journal and evolution API are shared between the user-space runtime (agentrt) and the OS-level memory layer (agentrt-linux（AirymaxOS）), ensuring architectural homology with no adaptation layer.

## Repository Structure (0.1.1 Documentation Complete)

```
memory/
├── README.md           # This file (English)
├── README_zh.md        # Chinese translation
├── LICENSE             # AGPL-3.0 + Apache-2.0 dual license
├── NOTICE              # Copyright, trademark and third-party notices
└── .gitignore
```

Design documents and reference distribution specifications are maintained in the `docs/AirymaxAgentOS/` directory of the umbrella repository.

## Upstream & Downstream Dependencies

### Upstream

- **agentrt-linux Kernel (AirymaxOS Kernel)** — provides the memory management APIs, MGLRU hooks and PMEM/CXL drivers
- **Airymax heapstore + memoryrovol** — provides the heap and rolling-evolution engine that are reused and extended
- **Euler 24.03 LTS / 26.03** — reference distribution for memory and CXL standards

### Downstream

- **agentrt-linux Cognition（AirymaxOS Cognition）** — cognition engine that relies on tiered memory for LLM serving and KV-cache
- **agentrt-linux Services (AirymaxOS Services)** — service layer that allocates from the shared heap and rolling regions

## Branch Strategy

This leaf repository is developed on **`feature/official-hubs-01`**. The aggregating `agentrt-linux` management repo stays on `main`.

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`). See [LICENSE](LICENSE) for the full text.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
