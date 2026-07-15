# Chapter 11 — Signals

[← Back to Table of Contents](../README.md) | [← Previous](10-sockets.md)

---

## 11.1 Definition

A **signal** is an asynchronous **notification** delivered to a process to inform it
that an event occurred. A signal carries almost **no data** — essentially just its
**number** (e.g., `SIGINT`, `SIGTERM`, `SIGCHLD`, `SIGSEGV`). It's the OS's
**software interrupt** for processes.

Signals are a **control/notification** mechanism, not a data-transfer mechanism.

---

## 11.2 How It Works Conceptually

```
 Sender (or kernel)              Target process
  kill(pid, SIGTERM) ──► kernel sets "pending" bit in target's PCB
                              │
                              ▼  (next time target is scheduled / returns to user mode)
                         deliver signal:
                           • default action (term/ignore/stop/core), OR
                           • run registered handler function, OR
                           • remains blocked if masked
```

Mechanics:
- The kernel keeps a **pending mask** and a **blocked mask** per process.
- A process registers a **handler** with `sigaction` (preferred) or `signal`.
- Default actions: **Terminate**, **Terminate+core**, **Ignore**, **Stop**, **Continue**.
- Delivery is **asynchronous**: it interrupts normal flow whenever the process next runs.

### Standard vs real-time signals
- **Standard signals** (1–31): **not queued** — multiple pending instances of the same
  signal collapse into **one**. No guaranteed ordering, no payload.
- **Real-time signals** (`SIGRTMIN`..`SIGRTMAX`): **queued**, delivered in order, and can
  carry a small **payload** (`sigqueue` with `sigval`). Better when you can't lose events.

---

## 11.3 Why It Exists (Problem It Solves)

Signals solve **asynchronous notification** without a data channel:
- The kernel needs to tell a process "you faulted" (`SIGSEGV`), "your child died"
  (`SIGCHLD`), "terminal interrupt" (`SIGINT` from Ctrl-C).
- One process needs to tell another "please reload config" (`SIGHUP`), "shut down
  gracefully" (`SIGTERM`), or "die now" (`SIGKILL`).

They're lightweight, always-available, and need no pre-established channel — just a PID
and permission.

---

## 11.4 Performance Characteristics

- **Latency:** low to deliver, but **delivery timing is not precise** — it happens when
  the target is next scheduled / returns to user mode.
- **Throughput:** essentially **not a data mechanism**; standard signals can be
  *coalesced/lost*, so don't use them to count events.
- **Cost:** minimal kernel bookkeeping; handler invocation involves a small
  stack/context manipulation.

---

## 11.5 Tradeoffs

| Dimension | Signals |
|-----------|---------|
| **Speed** | Fast to send; imprecise delivery timing |
| **Complexity** | Deceptively tricky — async-signal-safety, reentrancy |
| **Safety** | Hazardous in handlers (very limited safe operations) |
| **Data capacity** | ~none (number only; RT signals: tiny payload) |
| **Reliability** | Standard signals can be **coalesced/lost**; RT signals queue |
| **Scope** | Same host; addressed by PID/process group |

---

## 11.6 When to Use / When Not to Use

**Use signals when:**
- **Lifecycle control**: `SIGTERM`/`SIGINT` (graceful shutdown), `SIGHUP` (reload),
  `SIGKILL`/`SIGSTOP` (force).
- **Kernel-originated events**: child exit (`SIGCHLD`), faults, timers (`SIGALRM`).
- You need a **simple, channel-free poke** to a known PID.

**Avoid signals when:**
- You need to **transfer data** → use pipes/sockets/queues.
- You need **reliable, ordered, countable** events → use a pipe/eventfd or RT signals.
- You're tempted to do **real work inside a handler** → don't; handlers must be tiny.

---

## 11.7 The Async-Signal-Safety Trap

A signal handler can interrupt your program **anywhere** — even in the middle of
`malloc`. So a handler may only call **async-signal-safe** functions (a short
POSIX-defined list: `write`, `_exit`, `sigaction`, etc.). Calling `printf`, `malloc`,
or most library functions in a handler can **deadlock or corrupt** state.

**Safe patterns:**
- **Set a `volatile sig_atomic_t` flag** and handle the real work in the main loop.
- **The self-pipe trick / `signalfd` / `eventfd`**: the handler writes 1 byte to a pipe;
  your event loop (`epoll`) treats signals as just another readable fd — turning async
  signals into synchronous, safe events.

```
 handler:  write(self_pipe_w, "x", 1);   // async-signal-safe
 main loop: epoll_wait() ──► self_pipe readable ──► handle signal safely
```

---

## 11.8 Common Pitfalls

- **Doing too much in a handler** → undefined behavior, deadlocks. Keep it minimal.
- **Relying on standard signals to count events** → they coalesce; you'll miss some.
- **Race conditions** with shared state touched by both handler and main code → use
  `sig_atomic_t`, block signals around critical sections (`sigprocmask`).
- **`SIGKILL`/`SIGSTOP` can't be caught/ignored** — don't design around catching them.
- **EINTR**: signals can interrupt blocking syscalls (`read` returns `EINTR`); retry or
  use `SA_RESTART`.
- **Forgetting `SIGPIPE`** when doing pipe/socket I/O → unexpected termination.

---

## 11.9 Key Takeaways

- A signal is an **asynchronous, near-dataless notification** — the OS's software
  interrupt for processes.
- Great for **lifecycle/control** (`SIGTERM`, `SIGHUP`, `SIGCHLD`); **not** for data.
- **Standard signals coalesce/lose**; **real-time signals queue** and carry a tiny payload.
- Handlers must be **async-signal-safe** — prefer the **self-pipe / `signalfd`** pattern
  to handle signals in your normal event loop.
- Mind **EINTR**, reentrancy, and that **SIGKILL/SIGSTOP** are uncatchable.

---

[← Previous](10-sockets.md) | [Back to ToC](../README.md) | [Next: Remote Procedure Call (RPC) →](12-rpc.md)
