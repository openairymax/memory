**Language:** English | [简体中文](README_zh.md)

# memory — agentrt-linux (AirymaxOS) Memory

[![Version](https://img.shields.io/badge/version-0.1.1-5a6b7e)](https://atomgit.com/openairymax/memory)
[![License](https://img.shields.io/badge/license-AGPL--3.0+Apache--2.0-4a90d9)](LICENSE)

> Memory subsystem of [agentrt-linux (AirymaxOS)](https://atomgit.com/openairymax/agentrt-linux) — the AI Agent Operating System.
> One of the 8 leaf repositories aggregated by the [agentrt-linux](https://atomgit.com/openairymax/agentrt-linux) management repo.
> Reuses and extends the Airymax `memoryrovol` (and `heapstore`) module for OS-level memory management.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

---

## Positioning

The **memory** leaf repository is the memory management subsystem of agentrt-linux
(AirymaxOS). It brings the MemoryRovol rolling-and-evolution engine into the
kernel (the L1–L4 memory-volume tiers), integrates CXL and PMEM for heterogeneous
memory tiering, and adopts the MGLRU (multi-generational LRU) for AI
workload-aware page reclaim.

## Core Responsibilities

- **MemoryRovol L1–L4 memory volumes** — rolling & evolution engine operating
  directly on kernel memory regions, shared with the agentrt runtime.
- **CXL (Compute Express Link)** for coherent, tiered, disaggregated memory pools.
- **PMEM (persistent memory)** for low-latency durable memory and fast restart.
- **MGLRU** (multi-generational LRU) for AI workload-aware page reclaim.
- **HeapStore** kernel-resident heap accounting shared with the agentrt runtime.
- **`[SC]` contribution** — owns the `memory_types.h` shared-contract header
  (single physical source under `kernel/include/uapi/linux/airymax/`).

## Relationship with Airymax `memoryrovol` + `heapstore`

The memory leaf repo reuses and extends the `memoryrovol` and `heapstore` modules
from the Airymax runtime platform. The heap accounting, rolling journal and
evolution API are shared between the user-space runtime (`agentrt`) and the
OS-level memory layer, ensuring architectural homology with no adaptation layer.

## Document & File List

```
memory/
├── README.md           # This file (English)
├── README_zh.md        # Chinese translation
├── LICENSE             # AGPL-3.0 + Apache-2.0 dual license
├── NOTICE              # Copyright, trademark and third-party notices
├── .gitignore
└── .github/
    └── README.md       # GitHub automation for this leaf repo
```

Design documents and reference distribution specifications are maintained in the
`docs/AirymaxOS/` directory of the umbrella documentation repository.

## CI Status

Memory changes are governed by management-repository workflows (each ≤ 2 jobs):

| Workflow | Jobs | Applies to memory via |
|----------|------|------------------------|
| `mgmt-orchestrator.yml` | `file-integrity` + `orchestrate-leaf-ci` | Verifies the `memory/` submodule dir; aggregates this repo's CI status |
| `sc-dual-ci.yml` | `sc-validate` + `sc-trigger-and-await` | Guards `memory_types.h` in the `[SC]` 10 core headers; triggers agentrt mirror PR on changes |
| `nightly.yml` | `nightly-test-suite` (formal verification of `mm/share_pool.c`; 72h soak for memory-leak / refcount drift) + `nightly-revert-or-budget` | Nightly cron |
| `release.yml` | `build-and-sign` (SBOM scan of `memory/`) + `publish-release` | Release tag |

Language-level CI (C, memory-tier unit tests) is delegated to this leaf
repository's own `.github/workflows/`.

## Development Guide

- **Branch**: `feature/official-hubs-01` (the management repo stays on `main`).
- **DCO**: every commit must be `Signed-off-by` (`git commit -s`).
- **Commit prefix**: `memory:`.
- **Code style**: C — tab-8, 80 cols (`.clang-format`); run `make format-check`.
- **`[SC]` changes**: edits to `memory_types.h` require dual CI (agentrt-linux
  `sc-dual-ci.yml` + agentrt mirror PR) and L1+L3 approval per OS-IRON-014.
- **Function prefix**: `airy_*` (not legacy `airymaxos_*`).

## Upstream & Downstream

- **Upstream** — `kernel` (memory APIs, MGLRU hooks, PMEM/CXL drivers); Airymax `memoryrovol` + `heapstore`; Euler 24.03 LTS / 26.03 standards.
- **Downstream** — `cognition` (tiered memory for LLM serving and KV-cache); `services` (allocates from the shared heap and rolling regions).

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`).
See [LICENSE](LICENSE) for the full text.

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
