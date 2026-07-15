# Chapter 8 — Message Queues

[← Back to Table of Contents](../README.md) | [← Previous](07-pipes.md)

---

## 8.1 Definition

A **message queue** is a kernel-managed list of **discrete messages**. Senders enqueue
whole messages; receivers dequeue whole messages. Unlike a pipe, **message boundaries
are preserved** — you get back exactly one message per receive, not a byte soup.

Two classic POSIX-world flavors:
- **System V message queues** (`msgget`, `msgsnd`, `msgrcv`) — older, integer keys/IDs.
- **POSIX message queues** (`mq_open`, `mq_send`, `mq_receive`) — name-based (`/name`),
  support **priorities** and **async notification**.

(Higher-level "message queues" like RabbitMQ/Kafka are *application-level brokers*,
covered in Ch. 21 — not the kernel primitive discussed here.)

---

## 8.2 How It Works Conceptually

```
  Sender(s)                     Kernel queue                 Receiver(s)
  ┌───────┐   msgsnd   ┌──────────────────────────┐  msgrcv  ┌────────┐
  │ "job1"│ ─────────► │ [job1][job2][job3] ...    │ ───────► │ worker │
  └───────┘            │  each entry = 1 message   │          └────────┘
  ┌───────┐            │  may carry type/priority  │
  │ "job2"│ ─────────► │                           │
  └───────┘            └──────────────────────────┘
```

Key properties:
- **Message-oriented**: each `send` → one record; each `receive` → one record.
- **Selective receive**: System V messages have a **type** (`long mtype`); a receiver
  can ask for a specific type. POSIX queues use **priority** ordering.
- **Kernel-persistent**: the queue (and its messages) survive even if the sender exits,
  until the queue is removed/`unlink`ed (POSIX) or `ipcrm`'d (System V).
- **Bounded**: there are limits on message size and total queue bytes; a full queue
  blocks senders (or returns `EAGAIN` if non-blocking).
- **Async notification (POSIX)**: `mq_notify` can signal a process when a message
  arrives on an empty queue — no polling.

---

## 8.3 Why It Exists (Problem It Solves)

Pipes give you a *stream* and force you to invent framing. Message queues solve:
- **Preserving record boundaries** so structured messages stay intact.
- **Decoupling sender and receiver in time** — the queue buffers messages; the sender
  doesn't need the receiver to be reading at that instant.
- **Prioritization / selective consumption** — pull the message *type* you want.
- **Many-to-many** drop-off without each pair needing its own pipe.

---

## 8.4 Performance Characteristics

- **Latency:** comparable to pipes — a `send`/`receive` is a syscall plus a copy in and
  a copy out; a wakeup may cause a context switch.
- **Throughput:** fine for small/medium control messages; **two copies** per message.
  Not designed for high-bandwidth bulk data (use shared memory for that).
- **Overhead per message:** higher relative cost for *tiny* messages (syscall dominates)
  → batch if you can.
- **Bounded buffering:** built-in backpressure when full.

---

## 8.5 Tradeoffs

| Dimension | Message Queue |
|-----------|---------------|
| **Speed** | Moderate; 2 copies/syscalls per message |
| **Complexity** | Low–moderate (System V API is clunky; POSIX cleaner) |
| **Safety** | High — kernel-managed, atomic per-message delivery |
| **Structure** | Preserves message boundaries; supports types/priorities |
| **Persistence** | Kernel-persistent → must clean up (leak risk) |
| **Scope** | Same host only |

---

## 8.6 When to Use / When Not to Use

**Use message queues when:**
- You exchange **discrete commands/events** and want boundaries preserved.
- You want **time decoupling** (sender and receiver run at different times).
- You need **priority** or **type-selective** consumption.
- A small number of cooperating processes on one host pass control messages.

**Avoid when:**
- You move **large payloads** at high rate → shared memory / zero-copy.
- You need **cross-machine** delivery → sockets or a real broker (Kafka/RabbitMQ).
- You need rich routing, durability, replay → use an application-level broker.

---

## 8.7 Common Pitfalls

- **Leaking queues.** Kernel-persistent; forgetting `mq_unlink`/`ipcrm` leaves stale
  queues consuming kernel limits. Check with `ipcs` (System V) / `/dev/mqueue` (POSIX).
- **Hitting small default limits.** Max message size and queue depth are often low by
  default (e.g., System V `msgmax`); large messages get rejected.
- **Blocking surprises.** A full queue blocks senders; an empty one blocks receivers —
  set `O_NONBLOCK`/`IPC_NOWAIT` when you need responsiveness.
- **Confusing System V "type" semantics.** `msgrcv` type matching rules (0 / >0 / <0)
  are subtle and a frequent bug source.

---

## 8.8 Key Takeaways

- Message queues deliver **whole messages**, preserving boundaries (vs pipe byte
  streams).
- They **decouple sender/receiver in time** and support **priorities/types**.
- **Kernel-persistent** → remember to unlink, or they leak.
- Good for **control/command messaging** on one host; not for high-bandwidth bulk
  (use shared memory) or cross-host delivery (use sockets/brokers).

---

[← Previous](07-pipes.md) | [Back to ToC](../README.md) | [Next: Shared Memory →](09-shared-memory.md)
