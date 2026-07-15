# Appendix B — Quick Reference Cheat Sheet

[← Back to Table of Contents](../README.md) | [← Previous](appendix-a-glossary.md)

---

## B.1 Mechanism Selector (one-line picks)

| If you need... | Use | Chapter |
|----------------|-----|---------|
| Simple parent→child byte stream | **Anonymous pipe** | [7](07-pipes.md) |
| Named channel for unrelated local procs | **FIFO / Unix socket** | [7](07-pipes.md), [10](10-sockets.md) |
| Discrete messages with priority/type | **Message queue** | [8](08-message-queues.md) |
| Fastest large/high-rate local transfer | **Shared memory** | [9](09-shared-memory.md) |
| Persistent shared state / big-file access | **Memory-mapped file** | [13](13-memory-mapped-files.md) |
| Local client/server, fd passing | **Unix domain socket** | [10](10-sockets.md) |
| Cross-machine communication | **TCP socket / RPC** | [10](10-sockets.md), [12](12-rpc.md) |
| Typed cross-language service calls | **RPC (gRPC)** | [12](12-rpc.md) |
| Lossy real-time (video/games) | **UDP** | [10](10-sockets.md) |
| Lifecycle/notification poke | **Signal** | [11](11-signals.md) |
| Decouple producers/consumers in time | **Message queue / broker** | [8](08-message-queues.md), [21](21-microservices-communication.md) |

---

## B.2 The Master Comparison (condensed)

| Mechanism | Dir | Unit | Copies | Cross-host | Built-in sync | Speed |
|-----------|-----|------|:------:|:----------:|:-------------:|-------|
| Pipe/FIFO | 1-way | bytes | 2 | ✗ | ✅ | moderate |
| Message queue | n-way | message | 2 | ✗ | ✅ | moderate |
| **Shared memory** | any | bytes | **0** | ✗ | ✗ DIY | **fastest** |
| mmap file | any | bytes | **0** | ✗ | ✗ DIY | fastest |
| Unix socket | 2-way | stream/dgram | 2 | ✗ | ✅ | fast |
| TCP socket | 2-way | stream | 2+ | ✅ | ✅ | net-bound |
| UDP socket | 2-way | datagram | 2+ | ✅ | unreliable | net-bound |
| Signal | 1-way | number | — | ✗ | — | fast |
| RPC | 2-way | typed call | 2+ | ✅ | ✅ | net-bound |

---

## B.3 Cost Model (memorize)

```
 total ≈ syscalls×syscall_cost + copies×(S/bandwidth) + ctx_switches×cs_cost + scheduling
```
- **Small messages** → dominated by **syscalls + context switches** → batch / shared mem.
- **Large messages** → dominated by **copies** → zero-copy / shared mem.
- **Distributed** → dominated by **network RTT** → minimize round trips.

Speed ranking (local): **shared mem ≈ mmap > UDS ≈ pipe ≈ queue > TCP loopback > TCP/RPC remote**

---

## B.4 Latency Numbers (approx., order of magnitude)

```
 L1 cache ~1ns   RAM ~100ns   shm IPC ~0.1–1µs   ctx switch ~1–5µs
 pipe/UDS RTT ~2–10µs   TCP loopback ~10–30µs
 same-DC RTT ~0.5ms   cross-continent RTT ~100–150ms
```

---

## B.5 Blocking Models

| Model | API | Use |
|-------|-----|-----|
| Blocking | plain read/write | few conns, simple |
| Non-blocking | `O_NONBLOCK` + retry | juggle work |
| Readiness | **`epoll`/`kqueue`** | thousands of conns (Reactor) |
| Completion | **`io_uring`/IOCP** | highest perf (Proactor) |

**Readiness** = "you can now"; **Completion** = "it's already done."

---

## B.6 Synchronization Primitives

| Primitive | For | Note |
|-----------|-----|------|
| Mutex | exclusive access | use **robust + process-shared** for IPC |
| Semaphore | count / signal | ownerless; binary or counting |
| Condition variable | wait-for-predicate | always with mutex + loop |
| Spinlock | tiny critical section | burns CPU; low latency |
| Atomics + barriers | lock-free | release on publish, acquire on consume |
| Futex | builds the above | no syscall when uncontended |

Classic combo: **shared memory + (empty/full) semaphores + mutex** = safe producer/consumer.

---

## B.7 Deadlock — Break One Coffman Condition

```
 Mutual exclusion · Hold-and-wait · No preemption · Circular wait
 Prevent: consistent lock ordering · try-lock+backoff · acquire-all-or-none · TIMEOUTS
```

---

## B.8 Throughput vs Latency Levers

| Technique | Latency | Throughput |
|-----------|:-------:|:----------:|
| Batching | worse | better |
| Pipelining | ~ | better |
| Shared mem / zero-copy | better | better |
| Busy-poll | better | CPU cost |
| TCP_NODELAY | better | worse |
| Adaptive batching | balanced | balanced |

**Little's Law:** $L = \lambda W$ — without concurrency, latency caps throughput; pipeline.

---

## B.9 Pitfall Checklist (per mechanism)

- **Pipe/FIFO:** frame messages · close unused ends (EOF) · handle SIGPIPE
- **Message queue:** unlink (leak) · small size limits · blocking on full/empty
- **Shared memory/mmap:** add sync · offsets not pointers · robust mutex · unlink · SIGBUS · false sharing
- **Unix socket:** stream has no boundaries · fd passing available
- **TCP:** frame yourself · `TCP_NODELAY` · handle resets/partial I/O · `SO_REUSEADDR`
- **UDP:** unreliable/unordered — add acks/sequence numbers
- **Signals:** async-signal-safe handlers only · standard signals coalesce · handle EINTR
- **RPC:** not local — timeouts, retries+backoff, **idempotency**, circuit breakers

---

## B.10 Diagnosis Tools

```
 fds:        lsof, ls -l /proc/<pid>/fd
 syscalls:   strace, bpftrace
 SysV/POSIX: ipcs, ipcrm, /dev/shm, /dev/mqueue
 sockets:    ss, netstat, tcpdump/wireshark
 CPU:        perf top, perf record (flame graphs)
 ctx switch: vmstat, pidstat -w
 races:      ThreadSanitizer, Helgrind/DRD
 deadlock:   gdb, pstack, /proc/<pid>/stack
```

---

## B.11 Design Decision Flow

```
 1. Local or distributed?       → shm/pipe/UDS   vs   socket/RPC/broker
 2. Stream or messages?         → pipe/TCP       vs   queue/datagram
 3. Sync or async?              → RPC            vs   broker/queue
 4. Latency or throughput?      → shm+spin       vs   batch/pipeline
 5. Failure handling?           → timeouts, idempotency, backpressure, cleanup
 6. Simplest that works?        → avoid premature shm / microservices / lock-free
```

---

## B.12 Golden Rules

```
 • Simplest mechanism that meets the requirement.
 • Measure before optimizing.
 • Shared memory = power + full responsibility for safety.
 • Always handle: peer death, timeouts, backpressure, cleanup.
 • Streams need framing; distributed needs idempotency.
 • Signal handlers: tiny + async-signal-safe.
 • Don't distribute / go lock-free / zero-copy prematurely.
```

---

[← Previous](appendix-a-glossary.md) | [Back to Table of Contents](../README.md)

---

*End of the IPC Study Guide. Return to the [Table of Contents](../README.md) to revisit
any chapter.*
