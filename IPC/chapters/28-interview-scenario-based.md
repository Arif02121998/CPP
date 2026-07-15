# Chapter 28 — Scenario-Based Questions

[← Back to Table of Contents](../README.md) | [← Previous](27-interview-beginner-advanced.md)

---

> Scenario questions test **judgment**: choosing the right mechanism and justifying
> tradeoffs. Each item gives the scenario, a recommended approach, and the reasoning.

---

## Scenario 1 — Streaming logs from a child process

**"A parent process spawns a child that produces a continuous stream of log lines. The
parent must read and process them. What IPC do you use?"**

**Answer:** An **anonymous pipe**. Create it before `fork()`; the child writes stdout to
the write end, the parent reads the read end. It's a byte stream (fine for log lines),
gives automatic **backpressure** and **EOF** when the child exits, and needs no naming.
Remember to **close unused ends** or the parent won't see EOF. (Ch. 7)

**Trap:** If asked "what if logs are structured records?" → add **framing** (newline
delimiter or length prefix), since pipes have no message boundaries. (Ch. 17)

---

## Scenario 2 — Two processes sharing a 2 GB dataset

**"A producer generates a 2 GB dataset that a consumer must process, on the same machine,
with minimal latency. How do you transfer it?"**

**Answer:** **Shared memory** (or a `MAP_SHARED` memory-mapped file). Copying 2 GB through
a pipe/socket would cost two copies and saturate memory bandwidth. With shared memory the
data is written **once** and read **in place** — zero-copy. Add a **semaphore or
condition** to signal "ready," and protect concurrent regions if both write. Use
**offsets, not pointers**, inside the region. If persistence is needed, use an mmap'd file.
(Ch. 9, 13)

**Trap:** "Don't forget synchronization and cleanup (`shm_unlink`)" — interviewers look
for this.

---

## Scenario 3 — Microservice A needs data from microservice B

**"Service A needs the current account balance from service B to render a page. Sync or
async?"**

**Answer:** **Synchronous RPC** (e.g., gRPC) — A needs an **immediate result** to render.
Add a **timeout**, **retries with backoff** (only if the read is idempotent — a balance
read is), and a **circuit breaker** so a slow B doesn't cascade. If many such calls are
needed per page, **batch** them to avoid an N+1 latency blowup. (Ch. 12, 21)

**Contrast:** "Service A wants to *notify* others that an order was placed" → **async
event** via a broker (fan-out, decoupling). (Ch. 21)

---

## Scenario 4 — Graceful shutdown

**"Your daemon must finish in-flight work and flush buffers when the system shuts it
down. How is it told, and how do you handle it?"**

**Answer:** The OS sends **`SIGTERM`** (then `SIGKILL` after a grace period). Install a
handler that — **async-signal-safely** — sets a `volatile sig_atomic_t` flag (or writes to
a **self-pipe/`signalfd`**). The main loop notices the flag, stops accepting new work,
finishes in-flight requests, flushes, and exits. **Never** do the cleanup work *inside*
the handler. (Ch. 11, 26)

**Trap:** "Why not do cleanup in the handler?" → handlers can interrupt anywhere and may
only call async-signal-safe functions; doing real work risks deadlock/corruption.

---

## Scenario 5 — 50,000 concurrent client connections

**"Design the I/O model for a chat server handling 50k simultaneous TCP connections."**

**Answer:** **Event-driven** architecture: non-blocking sockets + **`epoll`** (Reactor),
**not** thread-per-connection (C10K). Scale across cores with **multiple event loops** +
**`SO_REUSEPORT`**. Each connection is a state machine. Offload any CPU-heavy work to a
**thread pool** so handlers never block the loop. Apply **backpressure** (pause reads when
write buffers fill). (Ch. 5, 26)

---

## Scenario 6 — Local-then-remote evolution

**"You're building a service consumed locally now but may move to another host later.
What transport do you pick?"**

**Answer:** Use the **socket API** — start with a **Unix domain socket** (fast locally),
and you can switch to **TCP** later with minimal code change because the API is the same.
Even better, put **RPC (gRPC)** on top so the interface is transport-agnostic and
typed. Design a **transport-agnostic** API from day one. (Ch. 10, 20)

---

## Scenario 7 — Producer faster than consumer

**"A producer enqueues work faster than the consumer can handle. What happens and what do
you do?"**

**Answer:** This is **backpressure**. With a bounded buffer (pipe/queue/ring), the producer
**blocks** (or gets `EAGAIN`) when full — naturally throttling it. Options: (a) let it
block (flow control), (b) **drop** messages (acceptable for telemetry/video), (c) **shed
load** / return errors, or (d) **scale consumers** (add workers, partition the queue).
Never use an **unbounded** queue — it just defers the failure to an OOM crash. (Ch. 5, 18,
21)

---

## Scenario 8 — Distributed counter / idempotency

**"A client calls 'increment balance' over RPC, times out, and retries. The balance was
incremented twice. How do you prevent this?"**

**Answer:** The timeout was ambiguous — the first call may have **succeeded but lost its
reply**. Make the operation **idempotent**: attach a unique **idempotency key** per logical
request; the server records processed keys and **dedups** retries. Now at-least-once
delivery is safe. (Ch. 17, 20)

---

## Scenario 9 — High-frequency trading tick pipeline

**"Market data must flow from a NIC through several processing stages with sub-microsecond
latency. Design the IPC."**

**Answer:** **Kernel bypass** at ingest (DPDK/RDMA), then **shared-memory SPSC ring
buffers** between stages with **busy-polling** (no blocking, no syscalls, no context
switches), **lock-free** with release/acquire ordering. **Pin** each stage to an
**isolated core**, use **huge pages**, **NUMA-local** memory, and **pre-allocated**
buffers to eliminate jitter. Pass data **by reference** through the ring (zero copy).
(Ch. 23, 24, 25)

**Trap:** Mention the cost — burns CPU cores and is complex; justified only by measured
latency requirements.

---

## Scenario 10 — Choosing between message queue and shared memory

**"Several worker processes pull tasks from a dispatcher. Tasks are small JSON blobs,
moderate rate. Mechanism?"**

**Answer:** A **message queue** (or a local broker). Tasks are **discrete messages**
(boundaries preserved), the rate is moderate (copy cost negligible), and a queue gives
**work distribution** (one task → one worker) and **time decoupling**. Shared memory would
be over-engineering here — you'd reinvent queuing, synchronization, and boundaries for no
real benefit. (Ch. 8, 31)

---

## How to Approach Any Scenario (framework)

1. **Local or distributed?** (same host → shm/pipe/UDS; cross host → socket/RPC/broker)
2. **Data shape?** (stream → pipe/TCP; messages → queue/datagram; bulk → shared memory)
3. **Sync or async?** (need a reply now → RPC; fire-and-forget/fan-out → broker)
4. **Performance target?** (latency-critical → shm + spin; throughput → batch/pipeline)
5. **Failure & lifecycle?** (timeouts, idempotency, backpressure, cleanup)
6. **Justify tradeoffs and name the pitfalls** — that's what interviewers reward.

---

[← Previous](27-interview-beginner-advanced.md) | [Back to ToC](../README.md) | [Next: Debugging and Performance Questions →](29-interview-debugging-performance.md)
