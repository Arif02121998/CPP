# Chapter 16 — Deadlocks and Race Conditions

[← Back to Table of Contents](../README.md) | [← Previous](15-synchronization-primitives.md)

---

## 16.1 The Two Great Hazards of Concurrent IPC

Once multiple processes share state or wait on each other, two failure families appear:

- **Race conditions** — outcome depends on *unlucky timing*; data ends up corrupted or
  inconsistent.
- **Deadlocks** (and cousins: livelock, starvation) — processes wait **forever**, making
  no progress.

Both are **non-deterministic** — they may pass tests a thousand times and fail in
production. Understanding their conditions is how you design them out.

---

## 16.2 Race Conditions

A **race** occurs when two parties access shared data concurrently, at least one writes,
and there's no synchronization ordering the accesses.

### Example — lost update (shared counter)
```
 counter = 0 in shared memory
 A: read counter (0) ─┐         B: read counter (0)
                       ├ both compute 0+1
 A: write 1            │         B: write 1
 Result: 1  (should be 2 — one update lost)
```

### Torn read/write
A reader sees a **partially updated** structure (e.g., a 16-byte struct updated
non-atomically) → garbage. Only naturally-atomic word writes are safe without locks.

### Fixes
- Protect the access with a **mutex/semaphore**, or
- Use **atomic** operations (`fetch_add`) for simple counters, or
- Use a **lock-free** protocol with proper **memory barriers** (Ch. 25).

### TOCTOU (time-of-check to time-of-use)
A security-sensitive race: you *check* a condition (file perms, buffer space) then *act*,
but state changed in between. Common in file-based IPC; fix with atomic ops
(`O_CREAT|O_EXCL`, `openat`) and holding locks across check+use.

---

## 16.3 Deadlock — the Four Coffman Conditions

A deadlock requires **all four** simultaneously:

1. **Mutual exclusion** — resources held exclusively.
2. **Hold and wait** — a party holds one resource while waiting for another.
3. **No preemption** — resources can't be forcibly taken.
4. **Circular wait** — a cycle: A waits for B's resource, B waits for A's.

```
   A holds Lock1, wants Lock2          ┌──────┐ wants  ┌──────┐
   B holds Lock2, wants Lock1          │  A   │───────►│Lock2 │
   → neither proceeds                  │holds │        │held  │
                                       │Lock1 │◄───────│ by B │
                                       └──────┘ wants  └──────┘
```

**Break any one condition → no deadlock.**

### IPC-specific deadlock examples
- **Two pipes, full buffers:** A blocks writing to pipe1 (full) while B blocks writing to
  pipe2 (full); neither reads. (Classic parent/child pipe deadlock.)
- **Lock ordering:** two processes acquire two shared-memory mutexes in opposite orders.
- **Request/response with no timeout:** A waits for B's reply; B waits for A's — and a
  message was lost.

---

## 16.4 Deadlock Prevention Strategies

| Strategy | How | Tradeoff |
|----------|-----|----------|
| **Lock ordering** | Always acquire locks in a fixed global order | Must know all locks; discipline required |
| **Lock timeout / try-lock** | `trylock`; back off and retry on failure | Possible livelock; needs backoff |
| **No hold-and-wait** | Acquire all needed locks at once, or none | Lower concurrency |
| **Coarser locking** | One lock instead of many | Less parallelism |
| **Avoid nested locks** | Keep critical sections flat | Not always possible |
| **Timeouts on IPC waits** | Never block forever on a peer | Must handle timeout path |
| **Robust mutexes** | Recover if a lock holder dies | Recovery code complexity |

For pipe/socket deadlocks specifically: **drain both directions** (use non-blocking I/O
or separate reader/writer threads) so a full buffer never blocks you indefinitely.

---

## 16.5 Livelock and Starvation

- **Livelock:** processes keep *changing state in response to each other* but make no
  progress (e.g., both back off and retry in lockstep forever). Fix with **randomized
  backoff**.
- **Starvation:** a process never gets a resource because others keep winning (unfair
  scheduling/locks). Fix with **fair locks / queuing**, priority aging.

---

## 16.6 Detection and Debugging

- **Deadlock detection:** look for a **cycle** in the "who-waits-for-whom" graph. Tools:
  thread/process dumps, `gdb` backtraces of all threads, `pstack`, lock analyzers
  (helgrind, TSan), kernel `hung task` warnings.
- **Race detection:** **ThreadSanitizer (TSan)**, **Helgrind/DRD** (Valgrind), stress
  testing with randomized timing, assertions/invariants on shared state.
- **Reproduce** by adding artificial delays (`sleep`) near critical sections to widen the
  race window during testing.

---

## 16.7 Defensive Design Principles

- **Minimize shared mutable state** — the less you share, the fewer races.
- **Keep critical sections short** — hold locks briefly; do I/O outside them.
- **One consistent lock order**, documented.
- **Always use timeouts** on cross-process waits; assume peers can die.
- **Prefer higher-level, race-safe mechanisms** (queues, channels) over raw shared memory
  unless you need the performance.
- **Make invalid states unrepresentable** where possible (atomic publish of an immutable
  snapshot, double-buffering).

---

## 16.8 Text Diagram — Producer/Consumer Done Right

```
 Shared ring buffer + 2 semaphores (empty=N, full=0) + mutex
 Producer: wait(empty) → lock → enqueue → unlock → post(full)
 Consumer: wait(full)  → lock → dequeue → unlock → post(empty)
   • empty/full prevent overflow/underflow (no busy wait)
   • mutex prevents torn enqueue/dequeue (no race)
   • consistent order (wait sem, then mutex) prevents deadlock
```

---

## 16.9 Key Takeaways

- **Races** come from unsynchronized concurrent access; fix with locks, atomics, or
  lock-free + barriers. Watch for **lost updates, torn reads, TOCTOU**.
- **Deadlock** needs all four Coffman conditions — **break one** (lock ordering,
  timeouts, no hold-and-wait) to prevent it.
- **Livelock/starvation** are progress failures; fix with **randomized backoff** and
  **fairness**.
- Use **TSan/Helgrind**, thread dumps, and stress tests to find these
  non-deterministic bugs.
- Best defense: **minimize shared state, short critical sections, consistent lock order,
  and always use timeouts.**

---

[← Previous](15-synchronization-primitives.md) | [Back to ToC](../README.md) | [Next: Data Consistency and Ordering →](17-data-consistency-ordering.md)
