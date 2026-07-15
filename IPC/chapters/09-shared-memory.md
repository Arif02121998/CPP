# Chapter 9 — Shared Memory

[← Back to Table of Contents](../README.md) | [← Previous](08-message-queues.md)

---

## 9.1 Definition

**Shared memory** is an IPC mechanism where the kernel maps the **same physical memory
pages** into the address spaces of two or more processes. Once set up, processes read
and write that memory **directly**, with **no kernel involvement on the data path** —
making it the **fastest** IPC available.

APIs:
- **POSIX**: `shm_open` + `mmap` (file-descriptor based, name like `/myshm`).
- **System V**: `shmget` + `shmat` (integer key/ID based).
- **Anonymous**: `mmap(MAP_SHARED|MAP_ANONYMOUS)` shared across `fork()`.

---

## 9.2 How It Works Conceptually

```
   Process A virtual space        Process B virtual space
   ┌───────────────┐              ┌───────────────┐
   │ ...           │              │ ...           │
   │ 0x7f00: shm ──┼───┐      ┌───┼─ shm :0x5a00   │
   │ ...           │   │      │   │ ...           │
   └───────────────┘   ▼      ▼   └───────────────┘
                  ┌───────────────────────┐
                  │  SAME physical frames │   ← one copy of the data
                  │  (RAM pages)          │
                  └───────────────────────┘
```

- Each process may map the region at a **different virtual address**, but both point to
  the **same physical frames**. (So store **offsets**, not absolute pointers, inside it.)
- Reads/writes are ordinary memory accesses — **no syscall per access**.
- The kernel is involved only at **setup** (`mmap`) and **teardown** (`munmap`), plus
  page faults on first touch.

**Critical caveat:** raw shared memory provides **no synchronization**. Concurrent
access *will* race. You must pair it with a synchronization primitive — a semaphore,
mutex (in shared memory, `PTHREAD_PROCESS_SHARED`), or lock-free atomics (Ch. 25).

---

## 9.3 Why It Exists (Problem It Solves)

Every other IPC mechanism **copies** data through the kernel (often twice). For large
or high-frequency data, that copying dominates cost. Shared memory solves the
**"don't copy gigabytes" problem**: producers write once into shared pages; consumers
read in place. It's the only mechanism with a **zero-copy data path by construction**.

Typical use: a ring buffer in shared memory for a high-throughput producer/consumer,
or a large read-mostly cache shared by many workers.

---

## 9.4 Performance Characteristics

- **Latency:** lowest of all mechanisms — just memory accesses (nanoseconds), no
  syscall per transfer. Synchronization (a futex wake) is the only kernel cost, and
  lock-free designs avoid even that.
- **Throughput:** highest — limited by **memory bandwidth**, not copy/syscall overhead.
- **Scalability concern:** **cache coherence** traffic. Multiple cores writing the same
  cache lines cause "false sharing" and coherence stalls. Lay out data to avoid it.
- **Setup cost:** page faults on first access; large mappings benefit from **huge pages**.

---

## 9.5 Tradeoffs

| Dimension | Shared Memory |
|-----------|---------------|
| **Speed** | Fastest — zero-copy, no per-access syscalls |
| **Complexity** | **High** — you implement synchronization, layout, lifecycle |
| **Safety** | **Low by default** — races, corruption, no boundaries |
| **Structure** | Whatever you define in the region (you own the layout) |
| **Failure handling** | Hard — a crash mid-update can leave inconsistent state / held locks |
| **Scope** | Same host only (shares physical RAM) |

---

## 9.6 When to Use / When Not to Use

**Use shared memory when:**
- You move **large** data or **very high message rates** on one host.
- Latency must be **minimal** (HFT, audio/video pipelines, databases, game engines).
- Multiple processes need a **shared, mutable dataset** (frame buffer, cache).

**Avoid when:**
- You only pass small/occasional messages → a pipe/queue/socket is simpler and safe.
- You can't invest in **correct synchronization** → the bugs are brutal.
- Processes are on **different machines** → impossible; use sockets/RPC.
- You need **strong isolation** between mutually-distrusting processes → shared memory
  widens the attack/corruption surface.

---

## 9.7 Synchronization Patterns (you MUST pick one)

1. **Semaphore-guarded** — a POSIX/System V semaphore protects access or signals
   "data ready." Simple, kernel-backed.
2. **Process-shared mutex/condvar** — a pthread mutex placed *inside* the shared region
   with `PTHREAD_PROCESS_SHARED`. Use **robust mutexes** so a crash doesn't deadlock peers.
3. **Lock-free ring buffer** — atomic head/tail indices, memory barriers; no kernel
   calls in the fast path (Ch. 25). Highest performance, hardest to get right.

```
 Producer:  write data to slot ──► (barrier) ──► advance head atomically
 Consumer:  read tail < head ? ──► read slot ──► advance tail atomically
```

---

## 9.8 Common Pitfalls

- **No/incorrect synchronization** → torn reads, lost updates, corruption. The #1 bug.
- **Storing raw pointers** in the region → invalid in the other process (different
  virtual address). Use **offsets**.
- **Forgetting to `shm_unlink`** → kernel-persistent leak (see `ipcs`).
- **Crash with a held lock** → peers deadlock. Use **robust** mutexes or a recovery
  protocol.
- **False sharing** → two cores hammering the same cache line tanks performance.
  Pad/align hot fields to cache-line boundaries.
- **Assuming write ordering** → CPUs/compilers reorder; insert proper **memory barriers**.

---

## 9.9 Key Takeaways

- Shared memory maps the **same physical pages** into multiple processes →
  **zero-copy, fastest** IPC.
- The kernel is involved **only at setup/teardown**, not on each access.
- It provides **no synchronization** — you **must** add semaphores, process-shared
  mutexes, or lock-free atomics.
- Use **offsets not pointers**, watch **false sharing**, handle **crash-with-lock**, and
  remember to **unlink** named segments.
- Ideal for **large/high-rate, low-latency, same-host** data sharing; overkill and
  dangerous for simple messaging.

---

[← Previous](08-message-queues.md) | [Back to ToC](../README.md) | [Next: Sockets →](10-sockets.md)
