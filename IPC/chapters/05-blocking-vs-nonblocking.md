# Chapter 5 — Blocking vs Non-Blocking Communication

[← Back to Table of Contents](../README.md) | [← Previous](04-kernel-boundary-context-switching.md)

---

## 5.1 The Core Question

When a process performs an IPC operation that *can't complete right now* — reading an
empty pipe, writing to a full buffer — **what should happen?** The answer defines the
IPC's *blocking semantics*, and it shapes your whole program structure.

---

## 5.2 The Four Models

### 1. Blocking (synchronous)
The call **waits** until it can complete. Simple to reason about.

```
 read(fd, buf, n):
   if no data → sleep on wait queue → wake when data → return
```
- ✅ Simple, linear code.
- ❌ The thread can't do anything else while waiting; needs one thread per concurrent
  conversation.

### 2. Non-Blocking (polling)
The call **returns immediately**; if it couldn't proceed it returns an error
(`EAGAIN`/`EWOULDBLOCK`). You retry later.

```
 set O_NONBLOCK
 loop:
   n = read(fd, buf, ...)
   if n == -1 && errno == EAGAIN: do_other_work()  // don't sleep
```
- ✅ One thread can juggle many tasks.
- ❌ Naive polling **busy-waits** and wastes CPU. Needs a readiness mechanism to be
  efficient (see #3).

### 3. I/O Multiplexing (readiness notification)
Ask the kernel to watch **many** fds and tell you which are ready. Then do
non-blocking ops only on ready ones. APIs: `select`, `poll`, **`epoll`** (Linux),
`kqueue` (BSD/macOS), IOCP (Windows).

```
 epoll_wait() ── blocks once for ALL fds ──► returns ready set
   for each ready fd: non-blocking read/write
```
- ✅ One thread handles thousands of connections (the **event loop**).
- ✅ Sleeps efficiently when nothing is ready (no busy-wait).
- ❌ More complex; you manage state machines per connection. (See [Ch. 26](26-event-driven-systems.md).)

### 4. Asynchronous I/O (completion notification)
You *submit* an operation and get notified when it's **done** (not just "ready").
APIs: POSIX AIO, Windows IOCP, Linux **`io_uring`**.

```
 submit(read request) ──► kernel performs it ──► completion event delivered
```
- ✅ Highest throughput; minimal syscalls (`io_uring` batches submissions/completions).
- ❌ Most complex programming model.

---

## 5.3 Readiness vs Completion (a crucial distinction)

- **Readiness model** (`epoll`): "the fd *can* be read now without blocking" — *you*
  still perform the read.
- **Completion model** (`io_uring`, IOCP): "the read you asked for is *already done*,
  here's the data" — the kernel did it for you.

Completion models shine for disk I/O and very high connection counts; readiness models
dominate classic network servers.

---

## 5.4 Sender Side: What "Full" Means

Blocking applies to writes too. A pipe/socket has a finite buffer:

```
 Buffer full ──► blocking write() sleeps until space frees
            └──► non-blocking write() returns EAGAIN (or short write)
```

This is **backpressure**: a slow consumer eventually slows the producer. Designing how
backpressure propagates is a key part of robust IPC (drop? block? buffer? error?).

---

## 5.5 Timeouts and Cancellation

Real systems rarely want to block *forever*:
- `select`/`poll`/`epoll_wait` take a **timeout**.
- Sockets support `SO_RCVTIMEO`/`SO_SNDTIMEO`.
- A common pattern: block with a deadline, then handle timeout as a recoverable event.

Without timeouts, a dead peer can hang your process indefinitely.

---

## 5.6 Choosing a Model — Decision Guide

| Situation | Recommended model |
|-----------|-------------------|
| A few processes, simple flow | **Blocking** (one thread each) |
| Thousands of connections | **epoll/kqueue event loop** |
| Highest throughput disk + net | **io_uring / IOCP (async)** |
| CPU-bound work between messages | **Non-blocking + your own scheduling** |
| Ultra-low latency, CPU to spare | **Busy-poll / spin** (Ch. 23, 25) |

---

## 5.7 Concurrency Strategies That Wrap These Models

- **Thread-per-connection** — blocking I/O, simple, limited by thread count/memory.
- **Thread pool + blocking** — bounded threads pull work; good middle ground.
- **Single-threaded event loop** — non-blocking + epoll (Node.js, nginx, Redis).
- **Multi-reactor** — one event loop per core (nginx workers, Netty).

---

## 5.8 Text Diagram — Blocking vs Event Loop

```
 Blocking, thread-per-conn:        Event loop, single thread:

   T1: read ─ wait ─ handle          ┌───────── epoll_wait ─────────┐
   T2: read ─ wait ─ handle          │ ready: c3,c7,c9              │
   T3: read ─ wait ─ handle          │ handle c3; handle c7; c9     │
   ... N threads, N stacks           └── loop ──────────────────────┘
                                        1 thread, N connections
```

---

## 5.9 Key Takeaways

- The four models: **blocking**, **non-blocking polling**, **readiness
  multiplexing (epoll)**, **async completion (io_uring/IOCP)**.
- **Readiness ≠ completion**: epoll tells you *you can*, io_uring tells you *it's done*.
- Blocking is simplest but needs a thread per concurrent wait; event loops scale to
  many connections on one thread.
- **Backpressure** and **timeouts** are first-class design concerns, not afterthoughts.

---

[← Previous](04-kernel-boundary-context-switching.md) | [Back to ToC](../README.md) | [Next: Naming, Addressing, and Lifetime →](06-naming-addressing-lifetime.md)
