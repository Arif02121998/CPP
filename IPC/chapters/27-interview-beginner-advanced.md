# Chapter 27 — Interview Questions: Beginner → Advanced

[← Back to Table of Contents](../README.md) | [← Previous](26-event-driven-systems.md)

---

> Questions are grouped by difficulty. Each has a **concise model answer**. Try to answer
> before reading. Cross-references point to the relevant chapter for depth.

---

## 27.1 Beginner

**Q1. What is IPC and why is it needed?**
IPC is the set of OS-provided mechanisms that let separate processes exchange data and
synchronize. It's needed because each process has a **private address space** (for
safety, security, stability) and therefore can't share variables directly; cooperating
processes need sanctioned channels. (Ch. 1–2)

**Q2. How is IPC different from inter-thread communication?**
Threads of one process **share** an address space, so they communicate via shared
variables + intra-process synchronization. Processes are **isolated**, so they need
kernel-mediated IPC. IPC is heavier but provides fault/security isolation. (Ch. 1)

**Q3. Name the main IPC mechanisms.**
Pipes (anonymous/named), message queues, shared memory, sockets (Unix/TCP/UDP), signals,
RPC, and memory-mapped files. (Ch. 7–13)

**Q4. What's the difference between a pipe and a named pipe (FIFO)?**
An anonymous pipe is unnamed and shared only via **fd inheritance** between related
processes (parent/child). A FIFO has a **filesystem path**, so **unrelated** processes can
open it by name. (Ch. 7)

**Q5. Which IPC mechanism is the fastest, and why?**
**Shared memory** — both processes map the **same physical pages**, so data transfer
involves **no copies and no syscalls** on the data path. The catch: you must add your own
synchronization. (Ch. 9)

**Q6. What is a signal?**
An asynchronous notification (a software interrupt) sent to a process, carrying
essentially just a number (e.g., `SIGTERM`). Used for control/lifecycle events, not data
transfer. (Ch. 11)

---

## 27.2 Intermediate

**Q7. Why does a pipe have no "message boundaries"?**
A pipe is a **byte stream** — the kernel buffers raw bytes with no record markers. Two
writes can merge or a read can return a partial message. You must add **framing**
(length prefix or delimiter). (Ch. 7, 17)

**Q8. Shared memory is fastest but rarely the default choice. Why?**
Because it provides **no synchronization, no message boundaries, and no built-in safety**.
You must implement locking/atomics, handle crash-with-held-lock, use offsets not pointers,
and manage lifecycle. The complexity and bug risk outweigh the speed unless you truly need
it. (Ch. 9)

**Q9. Difference between a mutex and a semaphore?**
A **mutex** is a lock with an **owner** — only the locker unlocks; for mutual exclusion.
A **semaphore** is an ownerless **counter** with `wait`/`post`; a binary semaphore signals
between roles, a counting semaphore tracks N resources. (Ch. 15)

**Q10. Unix domain socket vs TCP socket on localhost — which is faster and why?**
**Unix domain socket.** It skips the entire **TCP/IP stack** (no checksums, headers,
congestion control, routing) and just moves bytes through kernel socket buffers. It also
supports **fd passing** and **peer credentials**. (Ch. 10)

**Q11. What is the difference between blocking and non-blocking I/O?**
Blocking calls **wait** until they can complete (simple, one thread per wait).
Non-blocking calls **return immediately** with `EAGAIN` if they can't proceed, letting one
thread juggle many fds — usually combined with `epoll` for efficient sleeping. (Ch. 5)

**Q12. How do you handle thousands of concurrent connections efficiently?**
Use an **event loop** with `epoll`/`kqueue` and non-blocking sockets (the Reactor
pattern), scaling across cores with multiple loops + `SO_REUSEPORT`. Avoid
thread-per-connection (C10K). (Ch. 26)

**Q13. What happens if you write to a pipe whose read end is closed?**
The writer receives **`SIGPIPE`** (default action: terminate) and/or `write` returns
`EPIPE`. Ignore `SIGPIPE` and handle `EPIPE`. (Ch. 7)

---

## 27.3 Advanced

**Q14. Walk through the cost of sending a small message over a pipe.**
syscall (`write`, mode switch) → copy user→kernel → mark reader runnable → return →
**context switch** to reader → syscall (`read`) → copy kernel→user. For small messages,
**syscalls + the context switch dominate**, not the copy. (Ch. 4, 14)

**Q15. Why might a system use shared memory for data but a socket/pipe for signaling?**
Bulk data benefits from **zero-copy** shared memory, but you still need a way to say
"data ready." A small **eventfd/pipe/semaphore** provides cheap notification and
integrates with an event loop — "control plane over socket, data plane over shared
memory." (Ch. 22, 26)

**Q16. What is zero-copy and name three techniques.**
Moving data without CPU copies through user space. Techniques: `sendfile`,
`splice`/`vmsplice`, `MSG_ZEROCOPY`, `mmap`, scatter-gather I/O, shared memory (local),
RDMA (remote). Worth it for **large** payloads. (Ch. 24)

**Q17. Explain memory ordering and why it matters for shared-memory IPC.**
CPUs/compilers reorder memory ops. Without barriers, a consumer might see a "ready" flag
before the data it guards. Use **store-release** on publish and **load-acquire** on
consume so data written before the flag is visible when the flag is. Critical for
lock-free rings. (Ch. 17, 25)

**Q18. What is the difference between lock-free and wait-free?**
**Lock-free:** at least one thread always makes progress (system never stalls).
**Wait-free:** every thread finishes in a bounded number of steps (stronger, rarer). Both
avoid locks via atomics. (Ch. 25)

**Q19. Why is RPC "not just a local function call"?**
It crosses a network: **latency** (orders of magnitude slower), **partial failure** (may
run but lose the reply — indistinguishable from "never ran"), **idempotency** concerns on
retry, and **serialization** limits. The abstraction hides the API, not the network
realities. (Ch. 12, 20)

**Q20. Explain the ABA problem.**
In CAS-based lock-free code, a value goes A→B→A; a `CAS(expected=A)` succeeds though the
state churned, possibly corrupting structures. Fix with **tagged pointers (version
counters)**, **hazard pointers**, or **epoch/RCU** reclamation. (Ch. 25)

**Q21. Level-triggered vs edge-triggered epoll?**
**Level-triggered:** notified while data remains (simpler). **Edge-triggered:** notified
only on the arrival transition — you must **drain to `EAGAIN`** or miss data; fewer
wakeups, higher performance, easier to misuse. (Ch. 26)

---

## 27.4 Rapid-Fire One-Liners

- **Fastest local IPC?** Shared memory (zero-copy). (Ch. 9)
- **Only cross-machine mechanisms?** TCP/UDP sockets, RPC. (Ch. 10, 20)
- **Preserves message boundaries?** Message queues, datagram sockets, RPC. (Ch. 8, 19)
- **Carries no real data?** Signals. (Ch. 11)
- **Two copies on the data path?** Pipes, queues, sockets. (Ch. 14)
- **Needs DIY synchronization?** Shared memory, mmap. (Ch. 9, 15)
- **Kernel-persistent (can leak)?** System V / POSIX named shm, mq, sem. (Ch. 6)
- **Can pass file descriptors?** Unix domain sockets (`SCM_RIGHTS`). (Ch. 10, 22)
- **Solves C10K?** Event loop with epoll/kqueue. (Ch. 26)
- **Approximates exactly-once?** Idempotency keys + at-least-once + dedup. (Ch. 17)

---

[← Previous](26-event-driven-systems.md) | [Back to ToC](../README.md) | [Next: Scenario-Based Questions →](28-interview-scenario-based.md)
