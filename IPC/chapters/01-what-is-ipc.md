# Chapter 1 — What Is IPC?

[← Back to Table of Contents](../README.md)

---

## 1.1 Definition

**Inter-Process Communication (IPC)** is the set of mechanisms an operating system
provides to let two or more **processes** exchange data and coordinate their actions.

A *process* is an instance of a running program with its **own private virtual address
space**. By design, one process cannot reach into another process's memory. That
isolation is a feature (it gives safety and stability), but it also creates a wall.
IPC is the controlled set of "doors" the kernel places in that wall.

> **One-line mental model:** IPC = *kernel-mediated channels that let isolated
> processes share data or signal events without breaking memory protection.*

---

## 1.2 Two Things IPC Actually Does

IPC always provides one or both of:

1. **Data transfer** — moving bytes from process A to process B
   (a pipe, a message, a shared buffer).
2. **Synchronization / signaling** — telling another process *when* something
   happened (a semaphore post, a signal, an event flag), often with little or no data.

Many real systems combine both: shared memory (data) + a semaphore (synchronization).

---

## 1.3 A First Picture

```
   Process A (PID 1001)              Process B (PID 1002)
 ┌───────────────────┐            ┌───────────────────┐
 │  private address  │            │  private address  │
 │  space            │            │  space            │
 │                   │            │                   │
 │   [write] ──────┐ │            │ ┌────── [read]    │
 └─────────────────│─┘            └─│─────────────────┘
                   │                │
                   ▼                ▼
            ┌──────────────────────────────┐
            │            KERNEL             │
            │   pipe / queue / socket /     │
            │   shared mapping / signal     │
            └──────────────────────────────┘
```

The kernel sits in the middle as a trusted broker. Even "shared memory" — where the
two processes seem to talk directly — is *set up* by the kernel, which maps the same
physical pages into both address spaces.

---

## 1.4 IPC vs Related Terms

| Term | What it means | Relationship to IPC |
|------|---------------|---------------------|
| **Threads** | Multiple execution flows *inside one process* sharing memory | Threads share memory directly, so they use **intra**-process synchronization, not IPC. IPC is *between* processes. |
| **Concurrency** | Multiple tasks making progress over time | IPC is how concurrent *processes* coordinate. |
| **Networking** | Communication across machines | Distributed IPC (sockets, RPC) is a subset; networking generalizes it across hosts. |
| **Serialization** | Turning structures into bytes | Often needed *before* IPC when sharing structured data. |

---

## 1.5 Local vs Distributed IPC (preview)

- **Local IPC** — both processes are on the *same machine*. The kernel can move data
  via memory or fast in-kernel buffers. Examples: pipes, shared memory, Unix sockets.
- **Distributed IPC** — processes are on *different machines*. Communication crosses a
  network and must handle latency, partial failure, and serialization. Examples:
  TCP sockets, RPC, message brokers.

This distinction drives almost every design decision later in the book.
(See [Ch. 20 — Local vs Distributed](20-local-vs-distributed.md).)

---

## 1.6 Key Takeaways

- IPC exists because processes are **isolated by default**; it provides controlled
  channels through that isolation.
- Every IPC mechanism does **data transfer**, **synchronization**, or both.
- The **kernel is always involved** in setup, and usually in transfer too (shared
  memory is the main exception for the data path).
- "Local vs distributed" is the highest-level fork in the design space.

---

[← Back to Table of Contents](../README.md) | [Next: Why IPC Is Needed →](02-why-ipc-is-needed.md)
