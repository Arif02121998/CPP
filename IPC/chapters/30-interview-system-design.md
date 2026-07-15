# Chapter 30 — System Design Questions

[← Back to Table of Contents](../README.md) | [← Previous](29-interview-debugging-performance.md)

---

> System-design questions ask you to **architect** a system and justify the IPC/comms
> choices. Each below gives the prompt, a structured approach, and the key tradeoffs an
> interviewer wants to hear.

---

## 30.1 Design a Shell-Style Pipeline Executor

**Prompt:** "Implement `cmd1 | cmd2 | cmd3`. How does data flow and what IPC do you use?"

**Approach:**
- Create N-1 **anonymous pipes**, one between each adjacent pair.
- `fork()` each command; in each child, **`dup2`** the proper pipe ends onto
  `stdin`/`stdout`, then **close all other pipe fds**, then `exec`.
- Parent closes all pipe ends and `wait`s for children.

**Key points:** automatic **backpressure** (a slow stage blocks upstream), **EOF**
propagation on close, and the critical detail that **every process must close unused
fds** or downstream stages never see EOF. (Ch. 3, 7)

---

## 30.2 Design a High-Throughput Logging System

**Prompt:** "Many app processes emit logs; you must collect, buffer, and persist them
without slowing the apps."

**Approach:**
- Apps write to a local **Unix domain socket** (or `/dev/log`) — fast, non-blocking.
- A **collector daemon** reads via an **event loop (epoll)**, buffers in memory, batches
  writes to disk/remote (**throughput** via batching).
- Apply **backpressure or drop** policy when overloaded (logs are often droppable).
- For cross-host shipping, forward batches over **TCP** to a central aggregator or
  **broker (Kafka)** for durability and replay.

**Tradeoffs:** UDS vs TCP (local speed vs remote reach), **batching** (throughput) vs
log **latency**, **drop vs block** under load, durability (fsync/group commit). (Ch. 18,
21, 22)

---

## 30.3 Design Communication for a Web Browser

**Prompt:** "A browser runs each tab/renderer in a separate sandboxed process. How do they
talk to the privileged main process?"

**Approach:**
- **Multi-process for security/fault isolation** — a renderer crash or exploit is
  contained.
- Renderers are **sandboxed** (no direct syscalls/filesystem); they talk to the browser
  process via **IPC over a pipe/Unix socket** using a **typed message protocol** (Chrome
  uses Mojo).
- Large data (rendered frames, images) goes through **shared memory** to avoid copying
  pixels (**control over socket, bulk over shared memory**).
- The main process brokers privileged operations (network, disk, GPU).

**Tradeoffs:** isolation/security vs IPC overhead; shared memory speed vs the larger
attack surface it creates between a sandbox and the broker (validate everything crossing
the boundary). (Ch. 2, 9, 22)

---

## 30.4 Design a Message Broker (mini Kafka/RabbitMQ)

**Prompt:** "Design a system to decouple producers and consumers across services."

**Approach:**
- **Producers** publish to **topics/queues**; **consumers** subscribe (pub/sub) or
  compete (work queue).
- Persist messages to an **append-only log** for durability and **replay** (consumers
  track an **offset**).
- **Delivery semantics:** at-least-once by default → consumers must be **idempotent**.
- **Partitioning** for scale + ordering *within* a partition; **consumer groups** for
  parallelism.
- **Backpressure** via bounded buffers / consumer lag monitoring; **retention** policy.

**Tradeoffs:** durability (fsync) vs throughput (batch/group commit); ordering vs
parallelism (partitions); at-least-once simplicity vs exactly-once complexity. (Ch. 8, 17,
21)

---

## 30.5 Design a Low-Latency Trading / Real-Time Pipeline

**Prompt:** "Process market data through several stages with the lowest possible,
predictable latency."

**Approach (data plane):**
- **Kernel bypass** at the NIC (DPDK/RDMA) — no kernel stack, no interrupts.
- **Shared-memory SPSC ring buffers** between stages; pass data **by reference**
  (zero-copy).
- **Busy-poll**, **lock-free** with release/acquire ordering — no syscalls, locks, or
  context switches on the hot path.
- **Pin** each stage to an **isolated core**; **huge pages**; **NUMA-local**;
  **pre-allocated** buffers; `mlock`.

**Tradeoffs:** burns whole CPU cores and is complex/fragile — justified **only** by
measured latency needs. Predictability (bounded tail) matters more than average. (Ch. 23,
24, 25)

---

## 30.6 Design Inter-Service Communication for Microservices

**Prompt:** "Architect how 20 microservices communicate."

**Approach:**
- **Synchronous request/response** (queries, commands needing a result) → **gRPC**
  (HTTP/2 + Protobuf): typed, cross-language, streaming.
- **Asynchronous events** (state changes, fan-out, decoupling) → **message broker**
  (Kafka/RabbitMQ).
- **Cross-cutting:** service **discovery**, client **load balancing**, **timeouts +
  retries with backoff**, **circuit breakers**, **distributed tracing**, **mTLS** —
  often via a **service mesh**.
- **Data consistency** across services → **sagas** with compensating actions;
  **transactional outbox** for reliable event publishing.

**Tradeoffs:** sync (simple consistency, temporal coupling, cascade risk) vs async
(decoupled, resilient, eventual consistency, harder to trace). Avoid the **distributed
monolith** and **chatty** APIs. (Ch. 12, 20, 21)

---

## 30.7 Design a Producer/Consumer Work Queue (single host)

**Prompt:** "Multiple workers process tasks from a dispatcher on one machine."

**Approach:**
- **Bounded queue** in **shared memory** (ring buffer) with **counting semaphores**
  (`empty`, `full`) + a **mutex** — or a **POSIX message queue** for simplicity.
- Workers `wait(full)` → dequeue → process; dispatcher `wait(empty)` → enqueue →
  `post(full)`.
- **Backpressure** built in (bounded buffer blocks the producer when full).
- Scale by adding workers; for higher rates, **shard** into multiple queues.

**Tradeoffs:** message queue (simple, safe, kernel-synced, 2 copies) vs shared-memory ring
(fastest, but you own synchronization and correctness). Pick by measured rate. (Ch. 8, 9,
15, 16)

---

## 30.8 A Reusable System-Design Framework

For any "design the communication" question, structure your answer:

```
 1. Requirements:   latency? throughput? consistency? scale? failure tolerance?
 2. Boundary:       local (same host) or distributed (cross host)?
 3. Pattern:        request/response (RPC) vs event/stream (broker/queue)?
 4. Mechanism:      pick per Ch.19 matrix (pipe/queue/shm/socket/RPC/broker)
 5. Data path:      copies, framing, serialization, zero-copy needs
 6. Sync & safety:  locks/atomics, ordering, idempotency, backpressure
 7. Failure model:  timeouts, retries, partial failure, cleanup/leaks
 8. Scale & evolve: sharding, multiple loops/cores, transport-agnostic API
 9. Tradeoffs:      state what you optimized for and what you gave up
```

> Interviewers reward **explicit tradeoff reasoning** and **naming the pitfalls**
> (Ch. 31) far more than picking a single "right" mechanism.

---

## 30.9 Key Takeaways

- Anchor every design in **requirements → boundary → pattern → mechanism → tradeoffs**.
- The recurring winning pattern: **control/commands over sockets/RPC, bulk data over
  shared memory; sync RPC for queries, async broker for events.**
- Always address **failure** (timeouts, idempotency, partial failure), **backpressure**,
  and **cleanup/leaks** — these separate strong answers from textbook ones.
- **Justify tradeoffs out loud** and avoid **over-engineering** (Ch. 31).

---

[← Previous](29-interview-debugging-performance.md) | [Back to ToC](../README.md) | [Next: Common Mistakes and Anti-Patterns →](31-common-mistakes.md)
