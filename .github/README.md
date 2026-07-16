# `.github/` — memory Leaf Repository Automation

> GitHub automation directory for the **memory** leaf repository of
> [agentrt-linux (AirymaxOS)](https://atomgit.com/openairymax/agentrt-linux).

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

---

## Positioning

This directory hosts memory-specific GitHub automation. Management-level
orchestration (governance integrity, `[SC]` dual-CI for `memory_types.h`, nightly
formal verification of `mm/share_pool.c`, 72h soak for memory-leak/refcount drift,
release SBOM, SSoT validation) runs in the management repository; this leaf
repository hosts language-level CI for the C memory modules.

## Directory Contents

```
memory/.github/
└── README.md    # This file
```

## Applicable Management-Repository Workflows

| Workflow | Jobs | Relevance to memory |
|----------|------|----------------------|
| `mgmt-orchestrator.yml` | `file-integrity` + `orchestrate-leaf-ci` | Verifies the `memory/` submodule dir exists; aggregates this repo's CI status |
| `sc-dual-ci.yml` | `sc-validate` + `sc-trigger-and-await` | Guards `memory_types.h` (`[SC]` 6+2 set); creates agentrt mirror PR on changes |
| `nightly.yml` | `nightly-test-suite` + `nightly-revert-or-budget` | seL4-style formal verification on `mm/share_pool.c`; 72h soak detects memory leaks / refcount drift; chaos (mem hotremove) |
| `release.yml` | `build-and-sign` + `publish-release` | `syft memory/` SBOM; signed release artifacts |

## Development Guide

- Add leaf-local workflows for MemoryRovol L1–L4 tier tests, CXL/PMEM driver
  coverage, and MGLRU reclaim tests; keep each workflow ≤ 2 jobs.
- `[SC]` header `memory_types.h` lives at `kernel/include/airymax/memory_types.h`
  — single physical source, no duplicates (OS-IRON-014).
- Memory APIs use the `airy_*` prefix.

## License

Dual-licensed under **AGPL v3 + Apache 2.0** (SPDX: `AGPL-3.0-or-later OR Apache-2.0`).
See the repository root [LICENSE](../LICENSE) and [NOTICE](../NOTICE).

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
