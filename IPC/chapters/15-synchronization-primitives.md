# Chapter 15 — Synchronization Primitives

[← Back to Table of Contents](../README.md) | [← Previous](14-kernel-involvement-data-paths.md)

---

## 15.1 Why Synchronization Is Inseparable from IPC

Mechanisms that **copy** data through the kernel (pipes, queues, sockets) get
synchronization "for free" — the kernel serializes access and signals readiness.
But **shared memory** and **memory-mapped files** give you a raw shared buffer with
**no coordination**. There, *you* must add synchronization, or you get races and
corruption. This chapter covers the primitives that do that job.

---

## 15.2 The Two Things Sync Primitives Provide

1. **Mutual exclusion** — only one party touches a critical section at a time.
2. **Coordination / signaling** — "data is ready," "buffer has space," "your turn."

Different primitives emphasize one or both.

---

## 15.3 Mutex (Mutual Exclusion Lock)

A lock with two states: **locked/unlocked**, with an **owner**. Only the owner unlocks.

```
 lock(m);            // enter critical section (block if held)
   // touch shared data exclusively
 unlock(m);          // leave
```

- **Process-shared mutex:** place a `pthread_mutex_t` *inside* the shared region and
  init with `PTHREAD_PROCESS_SHARED` to use it across processes.
- **Robust mutex** (`PTHREAD_MUTEX_ROBUST`): if the owner dies while holding it, the next
  locker gets `EOWNERDEAD` and can **recover** — avoiding permanent deadlock. Essential
  for shared-memory IPC where a peer may crash.
- Implemented with **futexes**: no syscall when uncontended (fast), syscall only to
  sleep/wake on contention.

---

## 15.4 Semaphore (Counting / Signaling)

A semaphore is an **integer counter** with atomic `wait` (P, decrement; block if 0) and
`post` (V, increment; wake a waiter).

```
 wait(s): while s==0 block; s--      // acquire a unit / wait for signal
 post(s): s++; wake one waiter       // release a unit / send signal
```

- **Binary semaphore** (0/1) ≈ a lock, but **any** process can post (no ownership) — good
  for **signaling** ("data ready") between distinct producer/consumer roles.
- **Counting semaphore** tracks **N available resources** (e.g., free slots in a ring
  buffer).
- POSIX **named semaphores** (`sem_open("/name")`) work across unrelated processes;
  **unnamed** ones (`sem_init` with `pshared`) live in shared memory.

**Classic pairing:** shared memory (data) + two semaphores (`empty`, `full`) for a
bounded producer/consumer buffer.

```
 Producer: wait(empty); lock; put item; unlock; post(full);
 Consumer: wait(full);  lock; take item; unlock; post(empty);
```

---

## 15.5 Condition Variables

A condition variable lets a thread/process **wait for a predicate** to become true while
**releasing a mutex**, and be woken when another party signals.

```
 lock(m);
 while (!ready) cond_wait(cv, m);   // atomically unlock+sleep; relock on wake
 // proceed
 unlock(m);
 ...
 lock(m); ready = true; cond_signal(cv); unlock(m);
```

- Must be used **with a mutex** and a **predicate loop** (guards against spurious
  wakeups and lost signals).
- Process-shared variants exist for shared memory IPC.

---

## 15.6 Spinlocks

A lock that **busy-waits** (spins) instead of sleeping.

```
 while (test_and_set(lock)) { /* spin */ }   // burns CPU until acquired
```

- ✅ **Lowest latency** when the critical section is *tiny* and contention is brief —
  no context switch.
- ❌ **Wastes CPU** while spinning; terrible if the holder is descheduled or the section
  is long. Never spin on a single-core system for cross-process locks.
- Used in **low-latency** shared-memory designs (Ch. 23) and inside the kernel.

---

## 15.7 Futex — the Building Block (Linux)

A **futex** ("fast userspace mutex") is the kernel primitive most user-space locks are
built on:

```
 Fast path (uncontended): atomic CAS in user space — NO syscall.
 Slow path (contended):   futex(WAIT)  → kernel sleeps the thread
                          futex(WAKE)  → kernel wakes a waiter
```

This is why a well-implemented mutex/semaphore is nearly free when there's no
contention, and only pays kernel cost when it must actually block — the key to fast
shared-memory synchronization.

---

## 15.8 Atomic Operations & Memory Barriers

For **lock-free** coordination (Ch. 25), you use hardware atomics directly:

- **Atomic read-modify-write**: `CAS` (compare-and-swap), `fetch_add`, `exchange` —
  indivisible operations on a word.
- **Memory barriers / ordering**: CPUs and compilers **reorder** memory operations. You
  insert acquire/release fences (or use C11/C++11 `memory_order`) so a consumer that sees
  an updated index also sees the data written *before* it.

```
 Producer: write data;  store-release(head, new);   // data visible before head
 Consumer: load-acquire(head);  read data;           // sees data if it sees head
```

Getting ordering wrong yields bugs that appear only under load on certain CPUs.

---

## 15.9 Choosing a Primitive

| Need | Use |
|------|-----|
| Exclusive access to shared struct | **Mutex** (robust + process-shared for IPC) |
| Count available resources / slots | **Counting semaphore** |
| Producer signals "data ready" | **Binary semaphore** or **condvar** |
| Wait for a complex condition | **Condition variable + mutex** |
| Ultra-short critical section, low latency | **Spinlock** (carefully) |
| Maximum performance, no blocking | **Atomics + barriers (lock-free)** |
| Cross-unrelated-process signaling | **Named semaphore** |

---

## 15.10 Key Takeaways

- Kernel-buffered IPC syncs itself; **shared memory needs explicit synchronization**.
- **Mutex** = exclusive ownership (use **robust + process-shared** for IPC);
  **semaphore** = counting/signaling with no owner; **condvar** = wait-for-predicate.
- **Spinlocks** trade CPU for latency on tiny sections; **futexes** make uncontended
  locks syscall-free.
- **Lock-free** designs use **atomics + memory barriers**; correct **ordering** is
  essential and subtle.
- Pick the primitive that matches your pattern; the classic IPC combo is
  **shared memory + (empty/full) semaphores**.

---

[← Previous](14-kernel-involvement-data-paths.md) | [Back to ToC](../README.md) | [Next: Deadlocks and Race Conditions →](16-deadlocks-race-conditions.md)
