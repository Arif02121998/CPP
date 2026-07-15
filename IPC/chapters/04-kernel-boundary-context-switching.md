# Chapter 4 — The Kernel Boundary and Context Switching

[← Back to Table of Contents](../README.md) | [← Previous](03-ipc-os-process-model.md)

---

## 4.1 Why This Chapter Comes First

Almost every performance claim about IPC ("pipes are slow," "shared memory is fast")
traces back to **how many times you cross the kernel boundary** and **how many times
data is copied**. Master this chapter and the rest of the book becomes intuitive.

---

## 4.2 The System Call: Crossing into the Kernel

When a process calls `read()`, `write()`, `send()`, etc., it triggers a **system
call** — a controlled entry into kernel mode.

```
 user mode                          kernel mode
 ─────────                          ───────────
 write(fd, buf, n)
   │  set up args (syscall number)
   │  TRAP instruction  ───────────►  enter kernel (save user regs)
   │                                  check fd, permissions
   │                                  copy data / queue it
   │                                  maybe block, maybe wake peer
   │  ◄───────────────────────────── return value, restore regs
 continue
```

Costs of a single syscall:
- **Mode switch** (user→kernel→user): save/restore registers, switch stacks.
- **Cache/TLB effects**: kernel code and data evict some of your hot cache lines.
- Typical direct cost: on the order of **hundreds of nanoseconds to ~1 µs**
  (varies by CPU, mitigations like Spectre/Meltdown patches raised it).

> **Rule of thumb:** fewer syscalls per byte = faster IPC. This is why batching and
> shared memory win.

---

## 4.3 Data Copies: The Other Big Cost

Most IPC copies data **twice**:

```
 Sender:   user buffer ──copy 1──► kernel buffer
 Receiver: kernel buffer ──copy 2──► user buffer
```

That's the classic "two-copy" path for pipes, sockets, and message queues.
For large transfers, copy bandwidth (memory bandwidth) dominates over syscall cost.

**Shared memory** removes both copies for the data path: both processes read/write the
*same* physical pages directly. **Zero-copy** techniques (Ch. 24) reduce copies for
sockets/files too (e.g., `sendfile`, `splice`, `MSG_ZEROCOPY`).

---

## 4.4 Context Switch vs Mode Switch (Don't Confuse Them)

| | Mode switch | Context switch |
|--|-------------|----------------|
| **What** | Same process enters/exits kernel | CPU stops running one process/thread and runs another |
| **Triggered by** | Any syscall, interrupt | Blocking, time-slice end, higher-priority wakeup |
| **Saves** | User registers (lightweight) | Full register set, possibly **switch page tables (CR3)** → TLB flush |
| **Cost** | ~hundreds of ns | ~1–several µs (plus cache/TLB cold-start afterward) |

IPC often forces **context switches**: process A `write()`s and blocks; the kernel
wakes process B and schedules it. That handoff is frequently the *dominant* latency in
request/response IPC, not the copy.

```
 A: write(req) ─ blocks ─────────────────────────► (A off CPU)
                  kernel wakes B, schedules B
 B:                       ◄── read(req) runs, computes, write(resp)
                  kernel wakes A, schedules A
 A: read(resp) ◄─────────────────────────────────
        └── one round trip ≈ 2 context switches + 2–4 copies
```

---

## 4.5 Blocking, Wakeups, and Wait Queues

When a process blocks on IPC (e.g., reading an empty pipe), the kernel:

1. Marks it **not runnable** and puts it on the IPC object's **wait queue**.
2. Runs **other** processes (the CPU is not wasted).
3. When data arrives, marks the waiter **runnable** and the scheduler eventually runs it.

This is far better than **busy-waiting** (spinning in user space), which burns CPU.
But blocking costs **two context switches** per handoff. For ultra-low-latency systems,
*spinning* can actually win (Ch. 23, 25) — a classic latency-vs-CPU tradeoff.

---

## 4.6 Putting Cost Together — A Cost Model

Approximate cost of moving a message of size *S*:

```
 total ≈ (syscalls × syscall_cost)
       + (copies × S / memory_bandwidth)
       + (context_switches × cs_cost)
       + scheduling_jitter
```

Implications:
- **Small messages** → syscall + context-switch cost dominates → reduce round trips,
  batch, or use shared memory + spinning.
- **Large messages** → copy cost dominates → use shared memory or zero-copy.

---

## 4.7 Text Diagram — Where Time Goes

```
 Small request/response (e.g., 64 bytes over a socket):
   [syscall][ctx switch][syscall][ctx switch]  ← ~90% of time
   [   copy   ]                                ← tiny

 Large bulk transfer (e.g., 100 MB):
   [syscalls] [ctx]                            ← tiny
   [================ copies ================]  ← ~99% of time
```

---

## 4.8 Key Takeaways

- IPC cost = **syscalls + copies + context switches + scheduling**.
- A **mode switch** (syscall) is cheaper than a **context switch** (process change,
  possible TLB flush).
- The classic pipe/socket/queue path copies data **twice**; shared memory copies
  **zero** times on the data path.
- **Small messages** are dominated by switch overhead; **large messages** by copy
  bandwidth. Optimize accordingly.
- Blocking saves CPU but costs context switches; spinning saves latency but burns CPU.

---

[← Previous](03-ipc-os-process-model.md) | [Back to ToC](../README.md) | [Next: Blocking vs Non-Blocking →](05-blocking-vs-nonblocking.md)
