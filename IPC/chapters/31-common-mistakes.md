# Chapter 31 — Common Mistakes and Anti-Patterns

[← Back to Table of Contents](../README.md) | [← Previous](30-interview-system-design.md)

---

> A catalog of the mistakes that bite real systems. For each: the **mistake**, **why it
> happens**, the **symptom**, and the **fix**.

---

## 31.1 Misusing Shared Memory

### Mistake: shared memory with no (or wrong) synchronization
- **Why:** It "works" in testing because races are timing-dependent.
- **Symptom:** Intermittent garbage, torn reads, lost updates, corruption under load.
- **Fix:** Always pair shared memory with a **mutex/semaphore** or **atomics +
  release/acquire** barriers. Validate with **ThreadSanitizer**. (Ch. 9, 15, 17)

### Mistake: storing raw pointers in shared memory
- **Why:** Each process may map the segment at a **different virtual address**.
- **Symptom:** Wild pointers, segfaults in the *other* process only.
- **Fix:** Store **offsets** from the segment base, not absolute pointers. (Ch. 9)

### Mistake: crash while holding a lock in shared memory
- **Symptom:** Surviving peers **deadlock** forever on the orphaned lock.
- **Fix:** Use **robust mutexes** (`EOWNERDEAD` recovery) or a lock-free design with a
  recovery protocol. (Ch. 15, 16)

### Mistake: using shared memory when you don't need it
- **Symptom:** Weeks spent debugging races for a moderate-rate message flow.
- **Fix:** Use a **pipe/queue/socket** unless you've **measured** that copy/syscall cost
  is the bottleneck. (Ch. 31.6)

---

## 31.2 Deadlocks

### Mistake: inconsistent lock ordering
- **Symptom:** Two processes freeze; CPU idle; both stuck acquiring locks in opposite
  order.
- **Fix:** Define and document a **global lock order**; always acquire in that order; or
  use **try-lock + backoff**. (Ch. 16)

### Mistake: blocking on a peer with no timeout
- **Symptom:** A dead/slow peer hangs your process indefinitely.
- **Fix:** **Always use timeouts** on cross-process waits; assume peers can die. (Ch. 5,
  16)

### Mistake: two full pipes, neither drained
- **Symptom:** Classic parent/child deadlock — both block writing while neither reads.
- **Fix:** **Drain both directions** (non-blocking I/O or separate reader/writer threads).
  (Ch. 7, 16)

---

## 31.3 Forgetting Resource Cleanup (Leaks)

### Mistake: not unlinking kernel-persistent IPC
- **Why:** System V/POSIX **shm, mq, sem** survive process exit.
- **Symptom:** Stale segments accumulate (`ipcs`), hit kernel limits, "already exists"
  errors after restart.
- **Fix:** `shm_unlink`/`mq_unlink`/`sem_unlink` (or `ipcrm`); clean up on startup *and*
  shutdown. (Ch. 6)

### Mistake: leaking file descriptors
- **Symptom:** Pipe readers never see **EOF** (a forgotten write end stays open); `EMFILE`
  "too many open files."
- **Fix:** Close unused ends after `fork`; use **`FD_CLOEXEC`**; audit with `lsof`. (Ch. 6,
  7)

### Mistake: address reuse after restart
- **Symptom:** `bind: Address already in use` due to `TIME_WAIT`.
- **Fix:** Set **`SO_REUSEADDR`** (and understand its semantics). (Ch. 10)

---

## 31.4 Stream vs Message Confusion

### Mistake: assuming message boundaries on a stream
- **Why:** Pipes and TCP are **byte streams**, not message channels.
- **Symptom:** Two messages merge, or a "message" arrives split; parser corrupts.
- **Fix:** Implement **framing** — length prefix or delimiter. Handle **partial
  reads/writes**. (Ch. 7, 17)

---

## 31.5 Signal Handling Mistakes

### Mistake: doing real work in a signal handler
- **Symptom:** Rare deadlocks/corruption (handler called non-async-signal-safe code while
  a lock was held).
- **Fix:** Handler sets a `volatile sig_atomic_t` flag or writes to a **self-pipe/
  `signalfd`**; do the work in the main loop. (Ch. 11)

### Mistake: counting events with standard signals
- **Symptom:** Missed events — standard signals **coalesce** when multiple are pending.
- **Fix:** Use a **pipe/eventfd** for counted notifications, or **real-time signals**.
  (Ch. 11)

### Mistake: ignoring `EINTR`
- **Symptom:** `read`/`write` mysteriously "fail" when a signal arrives.
- **Fix:** Retry on `EINTR` or use `SA_RESTART`. (Ch. 11)

---

## 31.6 Overengineering IPC

### Mistake: premature microservices ("distributed monolith")
- **Symptom:** Network latency, partial failures, and ops complexity — without the scaling
  or team-autonomy benefits; services still deploy together.
- **Fix:** Start with a **modular monolith**; split only when scale/organization demands.
  (Ch. 20, 21)

### Mistake: reaching for kernel bypass / lock-free / zero-copy too early
- **Symptom:** Heisenbugs, burned CPU cores, unmaintainable code — for a system that a
  plain socket would serve fine.
- **Fix:** **Measure first.** Use the simplest mechanism that meets the requirement; add
  complexity only against a proven bottleneck. (Ch. 23, 24, 25)

### Mistake: chatty fine-grained RPC (N+1 calls)
- **Symptom:** A single user action triggers dozens of sequential network round trips →
  latency blowup.
- **Fix:** **Batch** calls, coarsen the API, or co-locate. Remember RPC is **not** a local
  call. (Ch. 12, 21)

### Mistake: unbounded queues/buffers
- **Symptom:** Memory grows under load until **OOM**; backpressure never applied.
- **Fix:** Use **bounded** buffers; decide block/drop/shed policy explicitly. (Ch. 5, 18)

---

## 31.7 Distributed-Specific Mistakes

### Mistake: naive retries on non-idempotent operations
- **Symptom:** Duplicate side effects (double charge) after a timeout-triggered retry.
- **Fix:** **Idempotency keys** + dedup; design idempotent operations. (Ch. 17, 20)

### Mistake: trusting the network (the fallacies)
- **Symptom:** No timeouts, no encryption, assuming zero latency/infinite bandwidth →
  outages and breaches.
- **Fix:** Design for **unreliability, latency, and untrusted networks**: timeouts,
  retries with backoff, circuit breakers, **mTLS/TLS**. (Ch. 20)

### Mistake: no backpressure → cascading failure
- **Symptom:** One slow service blocks callers → resource exhaustion ripples outward.
- **Fix:** **Timeouts, circuit breakers, bulkheads**, bounded concurrency. (Ch. 21)

---

## 31.8 The Meta-Lessons

```
 1. Use the SIMPLEST mechanism that meets the requirement.
 2. MEASURE before optimizing — don't guess the bottleneck.
 3. SHARED MEMORY = power + responsibility (you own all the safety).
 4. ALWAYS handle: peer death, timeouts, backpressure, and cleanup.
 5. STREAMS need FRAMING; DISTRIBUTED needs IDEMPOTENCY.
 6. SIGNAL HANDLERS must be tiny and async-signal-safe.
 7. Don't DISTRIBUTE or go LOCK-FREE/ZERO-COPY prematurely.
```

---

## 31.9 Key Takeaways

- The most common IPC bugs cluster around **missing synchronization (shared memory)**,
  **deadlocks**, **resource leaks (unclosed fds / un-unlinked objects)**, **stream/message
  confusion**, **unsafe signal handlers**, and **overengineering**.
- Distributed adds **non-idempotent retries**, **trusting the network**, and **missing
  backpressure**.
- The cure is almost always: **simplicity, measurement, explicit failure handling, and
  cleanup** — not more cleverness.

---

[← Previous](30-interview-system-design.md) | [Back to ToC](../README.md) | [Next: Appendix A — Glossary →](appendix-a-glossary.md)
