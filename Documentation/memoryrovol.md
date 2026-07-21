# MemoryRovol L1-L4 设计

> 本文档描述 memory 子仓中 MemoryRovol 四层记忆系统的设计。

Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.

License: GPL-2.0-only

---

## 1. 设计目标

提供 AirymaxOS 的四层记忆系统，根据数据热度自动分层，
平衡访问延迟与容量。

## 2. 四层分层模型

| 层级 | 枚举 | 介质 | 用途 | 回收策略 |
|------|------|------|------|---------|
| L1 hot | `AIRY_MEM_HOT` | DRAM | 热点数据 | aging 到 L2 |
| L2 warm | `AIRY_MEM_WARM` | DDR | 温数据 | MGLRU aging |
| L3 cold | `AIRY_MEM_COLD` | CXL/NVMe | 冷数据 | 迁移到 L4 |
| L4 persistent | `AIRY_MEM_PMEM` | PMEM | 持久数据 | Forgetting Engine |

## 3. L1 hot record 结构

```c
struct airy_mr_l1_record {
    __u64  record_id;       /* 记录唯一标识 */
    __u64  trace_id;        /* 分布式追踪 ID */
    __u64  timestamp_ns;    /* 创建时间戳（纳秒） */
    __u32  agent_id;        /* 所属 Agent ID */
    __u32  access_count;    /* 访问次数（热度） */
    __u32  mem_level;       /* 当前层级（AIRY_MEM_HOT） */
    __u32  flags;           /* 记录标志 */
    __u8   payload[64];     /* 记录载荷 */
    __u64  last_access_ns;  /* 最后访问时间戳 */
} __attribute__((aligned(64)));
```

## 4. 0.1.1 实现状态

### 4.1 已实现

- ✅ `airy_mr_l1_alloc()`：slab cache 分配 L1 hot 记录
- ✅ `airy_mr_l1_free()`：slab cache 释放 L1 hot 记录
- ✅ 模块 init/exit：`kmem_cache_create` / `kmem_cache_destroy`

### 4.2 待实现（1.0.1）

- ⏳ L2 warm（MGLRU aging）
- ⏳ L3 cold（CXL tier 迁移）
- ⏳ L4 persistent（PMEM 持久化）
- ⏳ Forgetting Engine 遗忘机制

## 5. 构建与加载

```bash
cd memory
make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
sudo insmod memoryrovol/airy_mr_l1.ko
dmesg | grep airy_mr_l1
```

## 6. 相关文档

- [07-directory-structure.md](../../../docs/AirymaxOS/10-architecture/07-directory-structure.md) §4.4 — memory 子仓完整目录结构
- [memory_types.h](../../kernel/include/uapi/linux/airymax/memory_types.h) — [SC] 记忆类型契约
