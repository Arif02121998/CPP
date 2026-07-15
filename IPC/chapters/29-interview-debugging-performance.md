# Chapter 29 — Debugging and Performance Questions

[← Back to Table of Contents](../README.md) | [← Previous](28-interview-scenario-based.md)

---

> These questions test whether you can **diagnose** real IPC problems and **reason about
> performance** with tools and numbers — not just recite definitions.

---

## 29.1 Debugging Scenarios

**Q1. "Two processes communicate over a pipe. The reader hangs forever even though the
writer finished. Why?"**

Most likely the reader (or some other process) still holds the **write end open**, so the
kernel never signals **EOF** (`read` keeps blocking for more data). After `fork`, every
process must **close the fds it doesn't use**. Check open fds with `ls -l /proc/<pid>/fd`.
(Ch. 6, 7)

---

**Q2. "A process using shared memory occasionally reads garbage / a half-updated
struct. What's wrong?"**

A **race condition / torn read** — concurrent access without synchronization, or missing
**memory barriers**. Shared memory provides **no** synchronization. Fix: protect with a
**mutex/semaphore** or use **atomic publish + release/acquire** ordering. Detect with
**ThreadSanitizer/Helgrind**. (Ch. 9, 16, 17)

---

**Q3. "Two processes each freeze, doing nothing. CPU is idle. How do you diagnose?"**

Likely a **deadlock**. Both are blocked waiting (on locks or on each other's IPC).
Diagnose: attach `gdb` / `pstack` to each and inspect **backtraces** — look for both stuck
in `lock`/`read`/`recv`. Build the **who-waits-for-whom** graph; a **cycle** confirms
deadlock. Check for **inconsistent lock ordering** or two full pipes with no reader.
Fix with consistent lock order / timeouts / draining both directions. (Ch. 16)

---

**Q4. "A process spins at 100% CPU but throughput is low. What might be happening?"**

Likely **busy-waiting / polling without sleeping** — e.g., a non-blocking loop that never
blocks on `epoll`, or a spinlock under contention, or edge-triggered `epoll` that wasn't
fully drained and keeps re-looping. Profile with `perf top` to find the hot loop. Replace
busy-wait with **blocking + epoll** unless you intentionally chose spinning for latency.
(Ch. 5, 23, 26)

---

**Q5. "Messages are occasionally lost between two services. Where do you look?"**

Clarify the transport: **UDP** is inherently lossy (no retransmit) → add acks/sequence
numbers or use TCP. **Standard signals** coalesce → don't count events with them. A
**full bounded queue** dropping under backpressure. A consumer crashing **after** dequeue
but **before** processing (need ack-after-process / at-least-once + idempotency). (Ch. 11,
17, 21)

---

**Q6. "After restarting your daemon, you see stale shared memory / `bind: Address already
in use`. Why?"**

**Kernel-persistent IPC objects leaked.** System V/POSIX shm, mq, sem survive process
exit until **unlinked** (`shm_unlink`, `ipcrm`, inspect with `ipcs`). For sockets,
`TIME_WAIT` holds the address — set **`SO_REUSEADDR`**. Always clean up named IPC on exit.
(Ch. 6, 10)

---

**Q7. "A signal handler occasionally deadlocks the program. Why?"**

The handler calls a **non-async-signal-safe** function (e.g., `malloc`, `printf`) and
interrupted the main thread *while it held an internal lock* (like the malloc arena lock)
→ self-deadlock. Handlers may only call **async-signal-safe** functions; set a flag or use
the **self-pipe/`signalfd`** pattern instead. (Ch. 11)

---

## 29.2 Performance Reasoning

**Q8. "Your IPC throughput is low for small messages. How do you improve it?"**

Small-message cost is dominated by **syscalls + context switches**, not copies.
- **Batch** multiple messages per syscall (amortize overhead).
- Use **shared memory + ring buffer** to avoid per-message syscalls entirely.
- Reduce **round trips** (pipeline; don't ping-pong).
- Consider **io_uring** to batch submissions/completions.
(Ch. 4, 18, 23)

---

**Q9. "Throughput is low for large messages, CPU is pegged in memcpy. Fix?"**

You're **copy/bandwidth-bound**.
- **Zero-copy**: `sendfile`/`splice` for files→sockets, `MSG_ZEROCOPY` for big sends,
  `mmap` for file access.
- **Shared memory** for same-host bulk (no transfer at all).
- **Scatter-gather** (`writev`) to avoid concatenation copies.
(Ch. 18, 24)

---

**Q10. "Median latency is great but p99 is terrible. What causes tail latency in IPC?"**

Sources of jitter: **context-switch scheduling delays**, **lock contention**, **GC
pauses**, **page faults**, **interrupt coalescing**, and **queueing under load**
(unbounded buffers). Mitigate: bound queues, **pin/isolate cores**, `mlock`/pre-fault,
busy-poll the hot path, avoid allocation in critical sections, set `TCP_NODELAY`. Measure
**percentiles**, not averages. (Ch. 18, 23)

---

**Q11. "How do you decide whether to use blocking I/O with threads or an event loop?"**

Estimate concurrency and blocking profile. A **few** connections / simple flow →
**blocking + threads** (simplest). **Thousands** of connections, mostly idle → **event
loop (epoll)**. Mixed CPU-heavy work → event loop + **worker thread pool**. Measure
context-switch rate (`vmstat`, `pidstat`) and thread memory. (Ch. 5, 26)

---

**Q12. "A localhost client/server is slower than expected. Quick wins?"**

- Switch **TCP loopback → Unix domain socket** (skip the IP stack).
- Set **`TCP_NODELAY`** if you must use TCP (kill Nagle's delay on small req/resp).
- **Batch** small messages; avoid chatty round trips.
- For bulk data, move to **shared memory**.
(Ch. 10, 18)

---

## 29.3 Tools Cheat Sheet

| Goal | Tools |
|------|-------|
| Inspect open fds / which pipe/socket | `lsof`, `ls -l /proc/<pid>/fd` |
| Trace syscalls (see the IPC calls) | `strace`, `ltrace`, `dtrace`/`bpftrace` |
| List/clean System V & POSIX IPC | `ipcs`, `ipcrm`, `/dev/shm`, `/dev/mqueue` |
| Socket states / addresses | `ss`, `netstat`, `tcpdump`/`wireshark` |
| CPU profiling / hot loops | `perf top`, `perf record`, flame graphs |
| Context switches / scheduling | `vmstat`, `pidstat -w`, `perf sched` |
| Race/deadlock detection | **ThreadSanitizer**, **Helgrind/DRD** (Valgrind) |
| Deadlock backtraces | `gdb`, `pstack`, `/proc/<pid>/stack` |
| Latency measurement | `perf`, `bpftrace`, histograms (HdrHistogram) |

---

## 29.4 A Diagnosis Framework

1. **Reproduce & observe**: is it a *hang* (deadlock/EOF), *corruption* (race), *loss*
   (delivery semantics), or *slowness* (overhead/copy/jitter)?
2. **Identify the mechanism & its guarantees** (Ch. 19) — what *should* happen?
3. **Use the right tool** (above) to see what *is* happening (fds, syscalls, stacks,
   perf).
4. **Map to root cause**: unclosed fd, missing sync/barrier, lock cycle, busy-wait, copy
   bound, tail jitter, leaked kernel object.
5. **Fix at the right layer** and **add a regression guard** (TSan in CI, invariants,
   timeouts).

---

[← Previous](28-interview-scenario-based.md) | [Back to ToC](../README.md) | [Next: System Design Questions →](30-interview-system-design.md)
