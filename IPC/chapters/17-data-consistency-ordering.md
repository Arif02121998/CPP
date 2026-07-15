# Chapter 17 — Data Consistency and Ordering

[← Back to Table of Contents](../README.md) | [← Previous](16-deadlocks-race-conditions.md)

---

## 17.1 What "Consistency" Means in IPC

When data flows between processes, you must guarantee that a receiver sees a
**coherent, correctly-ordered** view — not a half-written struct, not messages out of
order, not stale data. Consistency spans three layers:

1. **Atomicity** — updates are seen all-or-nothing, never partial (no torn reads).
2. **Ordering** — events/messages are observed in a well-defined order.
3. **Visibility** — once written, an update is actually *seen* by the other side
   (memory/cache and buffering effects).

---

## 17.2 Atomicity: All-or-Nothing Updates

A reader must never observe a partially-updated structure.

- **Kernel-buffered IPC** (pipes, queues, sockets): the kernel gives atomicity at the
  syscall level. Pipe writes ≤ `PIPE_BUF` are atomic; a datagram is delivered whole.
- **Shared memory**: only single aligned word writes are naturally atomic. For larger
  structures you need a **lock** or an **atomic publish** pattern:

```
 Double-buffering / atomic pointer swap:
   build new version in buffer B (private)
   atomically publish: shared_ptr = &B   (single word, store-release)
   readers load shared_ptr (acquire) and read a fully-consistent snapshot
```

This gives readers a consistent snapshot without locking on the read path.

---

## 17.3 Ordering Guarantees by Mechanism

| Mechanism | Ordering guarantee |
|-----------|--------------------|
| Pipe / FIFO | **FIFO** byte order, strictly preserved |
| Stream socket (TCP/UDS) | **In-order**, reliable byte stream |
| Datagram (UDP) | **No ordering**, may reorder/drop/duplicate |
| Message queue | FIFO (or by **priority/type** if used) |
| Shared memory | **No ordering** unless you add barriers/locks |
| Signals (standard) | **No ordering**, may coalesce |
| RT signals | Delivered in order, queued |

> Lesson: if your mechanism doesn't guarantee ordering (UDP, shared memory), you must
> impose it yourself (sequence numbers, memory barriers, locks).

---

## 17.4 Memory Visibility and the Reordering Problem

In shared memory, **writing** a value doesn't guarantee another core **sees** it
promptly or in program order. CPUs use store buffers and caches; compilers reorder
instructions. Without barriers:

```
 Producer:  data = 42;        // (1)
            ready = true;      // (2)
 Consumer:  if (ready)         // might see ready=true
                use(data);     // ...but data still 0!  (saw (2) before (1))
```

Fix with **release/acquire** ordering: the producer does a **store-release** on `ready`
(all prior writes become visible first); the consumer does a **load-acquire** on `ready`
(subsequent reads see those writes). This is the foundation of correct lock-free IPC
(Ch. 25).

---

## 17.5 Idempotency and Duplicates (Distributed)

Over a network, messages can be **lost, duplicated, or reordered**. Delivery semantics:

- **At-most-once** — may lose messages, never duplicates.
- **At-least-once** — never loses, but may **duplicate** (retries).
- **Exactly-once** — the ideal; expensive/impossible in general, *approximated* via
  **idempotency keys + dedup** at the receiver.

**Design rule:** make operations **idempotent** (applying twice = applying once) so that
at-least-once delivery + retries are safe. Use sequence numbers / dedup tables.

---

## 17.6 Consistency Models (brief, for shared/distributed state)

- **Strong / linearizable** — every read sees the latest write; behaves like a single
  copy. Easiest to reason about, costly to maintain (locks, coordination).
- **Sequential** — all see the same order, not necessarily real-time latest.
- **Eventual** — replicas converge *eventually*; reads may be stale. Common in
  distributed caches/brokers for availability and throughput.

Choosing weaker consistency buys performance/availability but pushes complexity (conflict
resolution, staleness handling) onto the application.

---

## 17.7 Framing and Message Boundaries (stream IPC)

Stream channels (pipes, TCP) deliver **bytes**, not messages. To keep messages
consistent you must **frame** them:

- **Length prefix:** send a fixed-size length, then that many bytes. Simple, robust.
- **Delimiter:** terminate messages with a marker (e.g., `\n`); must escape the marker
  in payloads.
- **Self-describing formats:** Protobuf/length-delimited, HTTP `Content-Length`.

Without framing, a receiver can't tell where one message ends and the next begins →
corruption at the application layer even though the transport was "reliable."

---

## 17.8 Key Takeaways

- IPC consistency = **atomicity + ordering + visibility**.
- Kernel-buffered IPC provides atomicity/ordering; **shared memory provides neither** —
  add locks, **memory barriers (release/acquire)**, or atomic-publish patterns.
- Know each mechanism's **ordering guarantee**; impose your own (sequence numbers) when
  there is none (UDP, shared memory).
- Over networks, assume **loss/duplication/reordering**; prefer **idempotent** operations
  and explicit delivery semantics.
- On **stream** channels you must **frame** messages (length-prefix or delimiter).

---

[← Previous](16-deadlocks-race-conditions.md) | [Back to ToC](../README.md) | [Next: Throughput vs Latency →](18-throughput-vs-latency.md)
