// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * mr_l1.c — MemoryRovol L1 hot record 分配/释放
 *
 * 职责：
 *   实现 MemoryRovol 四层记忆中的 L1 hot tier（DRAM）记录管理。
 *   使用 slab cache（kmem_cache）实现高频记录的快速分配与释放。
 *
 * L1 hot tier 特性：
 *   - 介质：DRAM（最快访问延迟）
 *   - 用途：存放热点记忆记录（近期访问、高频率）
 *   - 分配策略：slab cache（O(1) 分配/释放）
 *   - 回收策略：aging 到 L2 warm（MGLRU 管理）
 *
 * 决策 F1：真实可编译实现（非桩，非 return -ENOSYS）。
 *
 * 相关文档：
 *   docs/AirymaxOS/10-architecture/07-directory-structure.md §4.4
 *   kernel/include/uapi/linux/airymax/memory_types.h
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/atomic.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>

/* [SC] 共享契约头文件（通过 -I../kernel/include/uapi/linux 引用） */
#include <airymax/memory_types.h>

/* ─── 模块信息 ──────────────────────────────────────────────────────── */

#define AIRY_MR_L1_NAME		"airy_mr_l1"
#define AIRY_MR_L1_VERSION	"0.1.1"
#define AIRY_MR_L1_CACHE_NAME	"airy_mr_l1_record"
#define AIRY_MR_L1_RECORDS_MAX	1024

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("SPHARX Engineering Standards Group <eng-standards@spharx.com>");
MODULE_DESCRIPTION("AirymaxOS MemoryRovol L1 hot record allocator");
MODULE_VERSION(AIRY_MR_L1_VERSION);

/* ─── L1 hot record 结构（memory 子仓内部，非 [SC]） ───────────────── */

/*
 * airy_mr_l1_record — L1 hot tier 记录
 *
 * 存放热点记忆数据，固定大小以提高 slab cache 效率。
 * 对齐到 64 字节（cache line）以避免 false sharing。
 */
struct airy_mr_l1_record {
	__u64	record_id;		/* 记录唯一标识 */
	__u64	trace_id;		/* 分布式追踪 ID */
	__u64	timestamp_ns;		/* 创建时间戳（纳秒） */
	__u32	agent_id;		/* 所属 Agent ID */
	__u32	access_count;		/* 访问次数（热度） */
	__u32	mem_level;		/* 当前层级（AIRY_MEM_HOT） */
	__u32	flags;			/* 记录标志 */
	__u8	payload[64];		/* 记录载荷（64 字节） */
	__u64	last_access_ns;		/* 最后访问时间戳 */
} __attribute__((aligned(64)));

/* ─── 全局状态 ──────────────────────────────────────────────────────── */

static struct kmem_cache *airy_mr_l1_cache;
static atomic_t airy_mr_l1_alloc_count = ATOMIC_INIT(0);
static atomic_t airy_mr_l1_free_count = ATOMIC_INIT(0);

/* ─── 分配/释放接口 ─────────────────────────────────────────────────── */

/*
 * airy_mr_l1_alloc - 分配一条 L1 hot 记录
 *
 * @agent_id: 所属 Agent ID
 * @trace_id: 分布式追踪 ID
 *
 * 返回值：
 *   非 NULL - 成功，返回分配的记录指针
 *   NULL    - 失败（内存不足）
 *
 * 实现：
 *   1. 从 slab cache 分配一条记录（GFP_KERNEL 允许睡眠）
 *   2. 初始化记录字段（record_id / timestamp / access_count 等）
 *   3. 设置 mem_level = AIRY_MEM_HOT
 *   4. 递增全局分配计数器
 *
 * 注意：本函数是真实实现，非 return -ENOSYS 桩。
 */
struct airy_mr_l1_record *airy_mr_l1_alloc(__u32 agent_id, __u64 trace_id)
{
	struct airy_mr_l1_record *rec;
	static atomic64_t next_id = ATOMIC64_INIT(1);

	if (!airy_mr_l1_cache) {
		pr_err(AIRY_MR_L1_NAME ": slab cache 未初始化\n");
		return NULL;
	}

	/* 从 slab cache 分配（GFP_KERNEL 允许睡眠） */
	rec = kmem_cache_alloc(airy_mr_l1_cache, GFP_KERNEL);
	if (!rec) {
		pr_warn(AIRY_MR_L1_NAME ": kmem_cache_alloc 失败 "
			"(agent=%u trace=0x%llx)\n",
			agent_id, (unsigned long long)trace_id);
		return NULL;
	}

	/* 初始化记录字段 */
	rec->record_id = (__u64)atomic64_inc_return(&next_id);
	rec->trace_id = trace_id;
	rec->timestamp_ns = ktime_get_ns();
	rec->agent_id = agent_id;
	rec->access_count = 0;
	rec->mem_level = AIRY_MEM_HOT;	/* L1 hot tier */
	rec->flags = 0;
	rec->last_access_ns = rec->timestamp_ns;
	memset(rec->payload, 0, sizeof(rec->payload));

	atomic_inc(&airy_mr_l1_alloc_count);

	pr_debug(AIRY_MR_L1_NAME ": alloc 成功 id=%llu agent=%u "
		 "trace=0x%llx rec=%px\n",
		 (unsigned long long)rec->record_id,
		 agent_id, (unsigned long long)trace_id, rec);

	return rec;
}

/*
 * airy_mr_l1_free - 释放一条 L1 hot 记录
 *
 * @rec: 待释放的记录指针（由 airy_mr_l1_alloc 返回）
 *
 * 实现：
 *   1. 参数校验（NULL 检查）
 *   2. 清零敏感字段（防止信息泄漏）
 *   3. 调用 kmem_cache_free 归还到 slab cache
 *   4. 递增全局释放计数器
 */
void airy_mr_l1_free(struct airy_mr_l1_record *rec)
{
	if (!rec) {
		pr_warn(AIRY_MR_L1_NAME ": free 收到 NULL 指针\n");
		return;
	}

	if (!airy_mr_l1_cache) {
		pr_err(AIRY_MR_L1_NAME ": slab cache 未初始化\n");
		return;
	}

	pr_debug(AIRY_MR_L1_NAME ": free id=%llu rec=%px\n",
		 (unsigned long long)rec->record_id, rec);

	/* 清零敏感字段（防止信息泄漏） */
	memset(rec, 0, sizeof(*rec));

	/* 归还到 slab cache */
	kmem_cache_free(airy_mr_l1_cache, rec);

	atomic_inc(&airy_mr_l1_free_count);
}

/* ─── 模块初始化 / 退出 ─────────────────────────────────────────────── */

/*
 * airy_mr_l1_init - 模块初始化
 *
 * 创建 slab cache。
 *
 * 返回值：
 *   0 - 成功
 *   负值 - 失败
 */
static int __init airy_mr_l1_init(void)
{
	pr_info(AIRY_MR_L1_NAME ": 初始化 v" AIRY_MR_L1_VERSION "\n");
	pr_info(AIRY_MR_L1_NAME ": record_size=%zu bytes "
		"(aligned to 64-byte cache line)\n",
		sizeof(struct airy_mr_l1_record));

	/* 创建 slab cache */
	airy_mr_l1_cache = kmem_cache_create(
		AIRY_MR_L1_CACHE_NAME,
		sizeof(struct airy_mr_l1_record),
		__alignof__(struct airy_mr_l1_record),
		SLAB_HWCACHE_ALIGN | SLAB_POISON | SLAB_ACCOUNT,
		NULL);

	if (!airy_mr_l1_cache) {
		pr_err(AIRY_MR_L1_NAME ": kmem_cache_create 失败\n");
		return -ENOMEM;
	}

	pr_info(AIRY_MR_L1_NAME ": slab cache '%s' 创建成功 "
		"(obj_size=%zu)\n",
		AIRY_MR_L1_CACHE_NAME,
		sizeof(struct airy_mr_l1_record));

	return 0;
}

/*
 * airy_mr_l1_exit - 模块退出
 *
 * 销毁 slab cache。
 */
static void __exit airy_mr_l1_exit(void)
{
	int alloc_total = atomic_read(&airy_mr_l1_alloc_count);
	int free_total = atomic_read(&airy_mr_l1_free_count);

	pr_info(AIRY_MR_L1_NAME ": 退出 — 分配=%d 释放=%d "
		"泄漏=%d\n",
		alloc_total, free_total, alloc_total - free_total);

	if (airy_mr_l1_cache) {
		kmem_cache_destroy(airy_mr_l1_cache);
		airy_mr_l1_cache = NULL;
		pr_info(AIRY_MR_L1_NAME ": slab cache 已销毁\n");
	}
}

module_init(airy_mr_l1_init);
module_exit(airy_mr_l1_exit);

/* ─── 符号导出 ──────────────────────────────────────────────────────── */

EXPORT_SYMBOL_GPL(airy_mr_l1_alloc);
EXPORT_SYMBOL_GPL(airy_mr_l1_free);

MODULE_INFO(integrity, "AirymaxOS MemoryRovol L1 hot tier — "
			"slab cache allocator");
