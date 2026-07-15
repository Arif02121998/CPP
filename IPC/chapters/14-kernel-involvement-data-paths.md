# Chapter 14 — Kernel Involvement and Data Paths

[← Back to Table of Contents](../README.md) | [← Previous](13-memory-mapped-files.md)

---

## 14.1 The Central Question

For any IPC mechanism, ask: **how many times does the kernel touch the data, and how
many copies happen?** The answer predicts its performance. This chapter classifies all
mechanisms by their **data path**.

---

## 14.2 Three Data-Path Classes

### Class A — Two-copy, kernel-buffered
Data goes **user → kernel buffer → user**. The kernel mediates every transfer.

```
 Sender user buf ──copy1──► [kernel buffer] ──copy2──► Receiver user buf
```
**Members:** pipes, FIFOs, message queues, stream/datagram sockets (incl. UDS, TCP).
**Cost:** 2 copies + ≥2 syscalls + likely a context switch per exchange.

### Class B — Zero-copy data path, kernel only at setup
The kernel maps shared pages once; transfers are **plain memory accesses**.

```
 Sender writes ──► [shared physical pages] ◄── Receiver reads   (no per-op syscall)
```
**Members:** shared memory, memory-mapped files (`MAP_SHARED`).
**Cost:** setup syscalls + page faults; then ~0 kernel involvement (sync is separate).

### Class C — Reduced-copy / kernel-assisted zero-copy
Special calls let the kernel move data **without copying through user space**.

```
 file/page ──► [kernel moves pages/DMA directly] ──► socket/NIC
```
**Members:** `sendfile`, `splice`, `vmsplice`, `MSG_ZEROCOPY`, `io_uring` fixed buffers.
**Cost:** fewer copies; details in [Ch. 24 — Zero-Copy](24-zero-copy.md).

---

## 14.3 Anatomy of a Two-Copy Transfer (e.g., a pipe write/read)

```
 1. A: write(fd, buf, n)         → syscall (mode switch to kernel)
 2.    copy n bytes user→kernel  → CPU copy (memory bandwidth)
 3.    if reader waiting: mark runnable
 4. A: returns                   → mode switch back
 5.    scheduler runs B          → CONTEXT SWITCH (maybe TLB flush)
 6. B: read(fd, buf, n)          → syscall
 7.    copy n bytes kernel→user  → CPU copy
 8. B: returns
```

The expensive parts for **small** messages are the **syscalls (steps 1,6)** and the
**context switch (step 5)**; for **large** messages, the **copies (steps 2,7)**.

---

## 14.4 Why Shared Memory Is Different

Shared memory's data path has **no steps 1–8** for transfer. After `mmap`, writing a
struct is just a `mov` instruction. The kernel reappears only when you need
**synchronization** — and even then, a **futex** stays in user space in the
uncontended case, dropping to the kernel only to **sleep/wake** a waiter.

```
 Uncontended lock (futex): user-space atomic CAS — no syscall.
 Contended lock:           futex() syscall to sleep/wake — kernel involved.
```

This is why shared memory + lock-free or futex-based sync is the latency king.

---

## 14.5 The Page Cache: Files and IPC Converge

`read()/write()` and `mmap` both go through the **page cache**:

```
   read():  disk ─► page cache ─copy─► user buffer
   mmap():  disk ─► page cache  ═(mapped)═ user sees it directly (no copy)
```

So memory-mapped files **save the cache→user copy** that `read()` pays. This is the
same insight as Class B: avoid the copy by sharing the pages.

---

## 14.6 Interrupts, DMA, and the NIC (distributed IPC)

For network IPC, hardware participates:

```
 Outbound: user buf ─copy─► socket buffer ─► NIC via DMA ─► wire
 Inbound:  wire ─► NIC ─DMA─► kernel ring ─► socket buffer ─copy─► user buf
                                   ▲
                              IRQ / NAPI poll wakes the kernel
```

- **DMA** lets the NIC move packets to/from RAM without the CPU copying each byte.
- **Interrupts** (or polled **NAPI**) tell the kernel data arrived, which then wakes the
  waiting process (a context switch). Busy-polling NICs (DPDK, `SO_BUSY_POLL`) trade CPU
  for latency by avoiding the interrupt/wakeup path.

---

## 14.7 Summary Table — Kernel Involvement by Mechanism

| Mechanism | Copies (data path) | Syscall per transfer? | Context switch typical? | Class |
|-----------|-------------------:|:---------------------:|:-----------------------:|:----:|
| Anonymous pipe / FIFO | 2 | Yes | Often | A |
| Message queue | 2 | Yes | Often | A |
| Unix domain socket | 2 | Yes | Often | A |
| TCP/IP socket | 2 (+ stack) | Yes | Often + IRQ | A |
| Signals | ~0 (no data) | Yes (send) | On delivery | — |
| **Shared memory** | **0** | **No** (after setup) | Only to sleep/wake | B |
| **Memory-mapped file** | **0** (cache↔user) | **No** (after setup) | On page fault | B |
| sendfile/splice/MSG_ZEROCOPY | 0–1 | Yes (but fewer copies) | Maybe | C |

---

## 14.8 Key Takeaways

- IPC mechanisms fall into **two-copy kernel-buffered (A)**, **zero-copy shared (B)**,
  and **kernel-assisted reduced-copy (C)** data paths.
- The kernel is involved **per transfer** in Class A, only at **setup** in Class B.
- Small-message cost is dominated by **syscalls + context switches**; large-message cost
  by **copies (memory bandwidth)**.
- **Futexes** keep shared-memory synchronization in user space until contention forces a
  sleep/wake.
- Network IPC adds **DMA + interrupts**; busy-polling trades CPU for lower latency.

---

[← Previous](13-memory-mapped-files.md) | [Back to ToC](../README.md) | [Next: Synchronization Primitives →](15-synchronization-primitives.md)
