# Chapter 23 — High-Performance Systems

[← Back to Table of Contents](../README.md) | [← Previous](22-os-level-services.md)

---

## 23.1 The Setting

When microseconds or millions of messages/second matter — high-frequency trading (HFT),
packet processing, databases, game engines, real-time media — the "normal" IPC overheads
(syscalls, copies, context switches, interrupts) become the bottleneck. This chapter
covers how the fastest systems eliminate them.

The unifying theme: **remove the kernel from the hot path, avoid copies, and avoid
context switches.**

---

## 23.2 The Enemies of Low Latency

```
 syscalls ──► mode switches               (hundreds of ns each)
 copies   ──► memory bandwidth burned     (per byte)
 context switches ──► scheduler + TLB      (µs + cache cold-start)
 interrupts ──► wakeup latency + jitter    (µs + unpredictable)
 cache misses / false sharing ──► stalls   (100+ ns per miss)
 page faults ──► disk/zero-fill stalls     (µs–ms)
 lock contention ──► serialization         (unbounded)
```

High-performance IPC is a systematic campaign against each of these.

---

## 23.3 Technique 1 — Shared Memory + Lock-Free Ring Buffers

The foundation: a **single-producer/single-consumer (SPSC) ring buffer** in shared
memory with **atomic** head/tail indices and **no locks**.

```
 producer: write slot[head]; store-release(head, head+1)
 consumer: load-acquire(head); if tail<head read slot[tail]; store(tail, tail+1)
   • zero copies (data lives in the ring)
   • zero syscalls in steady state
   • no mutex → no context switch on the fast path
```

This is the design behind LMAX **Disruptor**, DPDK rings, and many HFT pipelines. It
trades CPU (busy-polling) for **sub-microsecond** handoff.

---

## 23.4 Technique 2 — Busy-Polling Instead of Blocking

Blocking on IPC causes **two context switches** per handoff (Ch. 4). Latency-critical
systems instead **spin** (busy-wait) on the shared index:

```
 while (tail == load_acquire(head)) {
     cpu_pause();   // PAUSE instruction, hint to CPU
 }
 // data ready, no sleep, no wakeup latency
```

- ✅ Eliminates wakeup latency and scheduler jitter → lowest, most **predictable**
  latency.
- ❌ Burns a full CPU core. Acceptable only when latency >> CPU cost (dedicated cores).
- Pair with **CPU pinning** and **isolated cores** (`isolcpus`, `taskset`) so the OS
  doesn't schedule anything else there.

---

## 23.5 Technique 3 — Kernel Bypass (Networking)

For network IPC, even the kernel's TCP/IP stack is too slow. **Kernel bypass** lets user
space talk to the NIC directly:

- **DPDK** — poll-mode drivers map NIC queues into user space; packets never enter the
  kernel. Millions of packets/sec per core.
- **RDMA** (InfiniBand/RoCE) — the NIC writes **directly into remote memory** without
  involving the remote CPU/kernel — "remote shared memory."
- **AF_XDP / XDP** — fast-path packet processing at the driver level.
- **io_uring** — not full bypass, but **batches** submissions/completions and supports
  **registered buffers**, drastically cutting syscalls and copies.

```
 Normal:  NIC ─► kernel stack ─► copy ─► app      (syscalls, copies, IRQ)
 DPDK:    NIC ═poll═► app (user space)            (no kernel, no IRQ, no copy)
 RDMA:    remote NIC ═► local RAM directly         (no remote CPU)
```

---

## 23.6 Technique 4 — Zero-Copy Data Paths

Avoid the CPU copying bytes (Ch. 24 covers this in depth):

- **`sendfile` / `splice`** — move file data to a socket inside the kernel.
- **`MSG_ZEROCOPY`** — TCP send without copying the user buffer.
- **Shared memory** — the ultimate zero-copy (no transfer at all).
- **Scatter-gather I/O** (`writev`/`readv`) — send headers + payload without
  concatenating them into one buffer.

---

## 23.7 Technique 5 — Mechanical Sympathy (CPU/Cache Awareness)

At this level, **hardware behavior dominates**:

- **Cache-line alignment / padding** — keep producer and consumer indices on **separate
  cache lines** to avoid **false sharing** (two cores fighting over one line).
- **NUMA awareness** — allocate shared memory on the **same NUMA node** as the threads
  using it; cross-node memory access is much slower.
- **Huge pages** — reduce TLB misses for large shared regions.
- **Prefetching & batching** — process messages in batches to amortize and keep the
  pipeline full.
- **Avoid memory allocation** in the hot path — pre-allocate; allocation causes jitter.

---

## 23.8 Technique 6 — Eliminate Jitter (Tail Latency)

For real-time systems, **predictability** matters more than average speed:

- **Pin threads** to dedicated cores; **isolate** those cores from the scheduler.
- **Disable** power-saving / frequency scaling that adds variability.
- **`mlock`** memory to prevent page faults; pre-fault everything at startup.
- Avoid GC languages on the hot path, or tune/avoid GC pauses.
- Avoid syscalls and locks that can block unpredictably.

---

## 23.9 The Cost: Complexity and CPU

These techniques are powerful but **expensive in engineering and resources**:

| Technique | Buys you | Costs you |
|-----------|----------|-----------|
| Lock-free ring | Sub-µs handoff | Hard correctness (barriers, ABA) |
| Busy-poll | No wakeup jitter | A whole CPU core |
| Kernel bypass | Millions pps | Complex setup, lose kernel features |
| Zero-copy | Bandwidth | API constraints, careful lifetimes |
| Cache/NUMA tuning | Fewer stalls | Deep hardware knowledge, fragile |

> **Use only when measured need justifies it.** For most systems, a pipe or socket is
> fine. Premature kernel-bypass is classic overengineering (Ch. 31).

---

## 23.10 Reference Architecture — Low-Latency Pipeline

```
 [NIC] ═DPDK poll═► [Ingest core] ─SPSC ring (shm)─► [Worker core] ─ring─► [Output core] ═► [NIC]
   • each stage pinned to an isolated core, busy-polling
   • data passed by reference through shared-memory rings (zero copy)
   • no syscalls, no locks, no context switches on the hot path
   • pre-allocated buffers, huge pages, NUMA-local
```

This is the shape of HFT and high-end packet-processing systems.

---

## 23.11 Key Takeaways

- High performance = **remove the kernel from the hot path**, **avoid copies**, **avoid
  context switches and interrupts**.
- Core tools: **shared-memory lock-free SPSC rings**, **busy-polling**, **kernel bypass**
  (DPDK/RDMA/io_uring), **zero-copy**, and **mechanical sympathy** (cache/NUMA/huge
  pages).
- These trade **CPU and complexity** for **latency and throughput** — justify with
  measurements.
- For predictability, fight **jitter**: pin/isolate cores, `mlock`, pre-fault, avoid
  allocation and unpredictable syscalls on the hot path.

---

[← Previous](22-os-level-services.md) | [Back to ToC](../README.md) | [Next: Zero-Copy Mechanisms →](24-zero-copy.md)
