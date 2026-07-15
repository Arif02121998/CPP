# Chapter 26 — Event-Driven Systems

[← Back to Table of Contents](../README.md) | [← Previous](25-lock-free-communication.md)

---

## 26.1 The Core Idea

An **event-driven** system reacts to **events** (a socket became readable, a timer fired,
a message arrived) instead of dedicating a thread to **wait** for each one. A single
thread (or a few) runs an **event loop** that asks the kernel "what's ready?" and dispatches
handlers. This is how you handle **tens of thousands of connections** without tens of
thousands of threads.

It's the architectural payoff of the **readiness/non-blocking** model from Chapter 5.

---

## 26.2 Why Event-Driven? The C10K Problem

The classic **thread-per-connection** model breaks at scale:

```
 10,000 connections × (1 thread + ~1 MB stack + context-switch overhead)
   = thousands of threads, gigabytes of stacks, scheduler thrashing
```

The **C10K problem** (handling 10,000+ concurrent connections) drove the shift to
event loops:

```
 Thread-per-conn:   N threads, N stacks, O(N) context switches
 Event loop:        1 thread,  1 stack,  O(ready) work per iteration
```

Most connections are **idle** at any instant; an event loop spends effort only on the
**few that are ready**.

---

## 26.3 Anatomy of an Event Loop

```
 setup: register fds of interest with the readiness API (epoll/kqueue)

 loop forever:
     ready = epoll_wait(timeout)        // sleeps efficiently until something happens
     for each fd in ready:
         if readable:  data = nonblocking_read(fd); dispatch_handler(data)
         if writable:  flush_pending_writes(fd)
     run_due_timers()
```

Key properties:
- **One blocking point** (`epoll_wait`) for *all* fds — efficient sleep, no busy-wait.
- All I/O is **non-blocking**; handlers must **never block** the loop.
- Each connection is a **state machine** (you can't use the call stack to hold per-request
  state across awaits).

---

## 26.4 The Readiness APIs

| API | Platform | Scaling | Notes |
|-----|----------|---------|-------|
| `select` | POSIX | O(N) per call, fd limit | Legacy, simple, portable |
| `poll` | POSIX | O(N) per call | No fd-number limit |
| **`epoll`** | Linux | **O(ready)** | Edge/level-triggered; the standard |
| **`kqueue`** | BSD/macOS | O(ready) | Also handles timers, signals, file events |
| **IOCP** | Windows | Completion-based | Async completion, not readiness |
| **`io_uring`** | Linux | Completion-based | Batched submit/complete; very high perf |

`select`/`poll` rescan **all** fds each call (O(N)); `epoll`/`kqueue` return only the
**ready** ones (O(ready)) — the key to scaling.

---

## 26.5 Level-Triggered vs Edge-Triggered

A crucial `epoll` subtlety:

- **Level-triggered (LT):** notified **as long as** data remains. Simpler; if you read
  part of the data, you'll be told again. (Default.)
- **Edge-triggered (ET):** notified **only on the transition** (new data arrived). You
  **must drain** the fd fully (read until `EAGAIN`) or you'll **miss** data until the next
  edge. Fewer wakeups, higher performance, easier to get wrong.

```
 LT: "there is data"        → notified repeatedly until you consume it all
 ET: "data just arrived"    → notified once; drain completely or lose the event
```

---

## 26.6 The Reactor and Proactor Patterns

- **Reactor** (readiness): the loop waits for "ready," then **the handler performs** the
  I/O. Built on `epoll`/`kqueue`. (nginx, Redis, Netty, libevent, libuv.)
- **Proactor** (completion): you **submit** an operation; the OS performs it and delivers
  a **completion** event. Built on IOCP/`io_uring`. The handler processes the **result**.

```
 Reactor:  "socket readable" → you call read()
 Proactor: "your read of 4KB is DONE, here's the buffer"
```

---

## 26.7 Scaling Across Cores

A single event loop uses **one core**. To use all cores:

- **Multiple event loops** (one per core), each with its own set of connections —
  **nginx workers**, **Netty event-loop groups**.
- **`SO_REUSEPORT`**: multiple processes/threads listen on the **same port**; the kernel
  load-balances incoming connections across them.

```
 [accept connections] ─► distribute ─► loop0(core0) loop1(core1) ... loopN(coreN)
```

This is the **multi-reactor** architecture.

---

## 26.8 Don't Block the Loop! (the cardinal rule)

A single slow handler stalls **every** connection. Threats and fixes:

| Threat | Fix |
|--------|-----|
| CPU-heavy work in a handler | Offload to a **worker thread pool** |
| Blocking syscall (DNS, disk, DB) | Use async variants or a thread pool |
| Large synchronous loops | Chunk the work, yield back to the loop |
| Unbounded write buffering | Apply **backpressure**; pause reads |

The event loop must stay **responsive**; treat any blocking operation as a bug.

---

## 26.9 Where IPC Fits

Event-driven systems are how processes **consume IPC at scale**:
- A server multiplexes thousands of **socket** connections in one loop.
- **Signals** are folded into the loop via `signalfd`/self-pipe (Ch. 11).
- **Pipes/eventfd** wake the loop from other threads (the classic cross-thread
  notification: write 1 byte to an `eventfd` the loop watches).
- **Timers** (`timerfd`) become just another readable fd.

> "Everything is an fd" lets one `epoll` loop unify **sockets, signals, timers, and
> cross-thread wakeups** — the elegance behind libuv/Node.js.

---

## 26.10 Tradeoffs

| | Thread-per-connection | Event loop |
|--|----------------------|-----------|
| **Mental model** | Simple, linear code | State machines / callbacks / async-await |
| **Scaling (conns)** | Limited by threads/RAM | Tens of thousands+ |
| **CPU use** | Context-switch heavy | Efficient, few switches |
| **Multi-core** | Automatic | Needs multiple loops |
| **Debugging** | Stack traces are clear | Harder (callbacks, async stacks) |
| **Blocking risk** | Per-thread, isolated | One block stalls everything |

Modern languages soften the complexity with **async/await** (coroutines) over an event
loop — linear-looking code on an event-driven core (Go goroutines, Rust async, Node,
Python asyncio).

---

## 26.11 Key Takeaways

- Event-driven systems use an **event loop** + **readiness API** (`epoll`/`kqueue`) to
  handle **many connections on few threads**, solving the **C10K** problem.
- Built on **non-blocking I/O**; the **cardinal rule** is *never block the loop* — offload
  CPU/blocking work.
- Know **level- vs edge-triggered** (`epoll`) and the **Reactor (readiness) vs Proactor
  (completion)** patterns.
- Scale across cores with **multiple loops** + **`SO_REUSEPORT`** (multi-reactor).
- **"Everything is an fd"** lets one loop unify sockets, **signals (signalfd)**, **timers
  (timerfd)**, and **cross-thread wakeups (eventfd)**; **async/await** hides the
  complexity.

---

[← Previous](25-lock-free-communication.md) | [Back to ToC](../README.md) | [Next: Interview — Beginner → Advanced →](27-interview-beginner-advanced.md)
