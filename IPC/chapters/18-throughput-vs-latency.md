# Chapter 18 — Throughput vs Latency

[← Back to Table of Contents](../README.md) | [← Previous](17-data-consistency-ordering.md)

---

## 18.1 Two Different Goals

- **Latency** — how long a *single* operation takes end to end (e.g., request→response
  time). Measured in µs/ms. Lower is better.
- **Throughput** — how *much* work completes per unit time (messages/sec, GB/s). Higher
  is better.

They are **related but distinct**, and optimizing one often **hurts** the other. Knowing
which one your system needs is the most important IPC design decision.

```
 Latency:    |--- one message: 50 µs ---|
 Throughput: ||||||||||||||  (many messages packed per second)
```

---

## 18.2 Why They Conflict

The classic tension is **batching**:

- **Batching** many small messages into one transfer **amortizes** per-operation cost
  (syscalls, context switches, headers) → **higher throughput**.
- But a message may **wait** for the batch to fill → **higher latency** for that message.

```
 No batching:  send each msg now    → low latency, low throughput (overhead-bound)
 Batching:     wait, then send 100  → high throughput, higher per-msg latency
```

Examples of the same tradeoff:
- **TCP Nagle's algorithm** coalesces small writes (throughput) → adds latency;
  `TCP_NODELAY` disables it for latency.
- **Interrupt coalescing** on NICs batches IRQs (throughput) → adds latency.
- **Group commit** in databases batches fsyncs (throughput) → adds commit latency.

---

## 18.3 Little's Law (the quantitative link)

$$ L = \lambda \times W $$

- $L$ = average number of concurrent requests *in flight*
- $\lambda$ = throughput (arrivals/completions per second)
- $W$ = average latency (time in system)

Implications:
- To get **high throughput with high latency**, you need **many concurrent** requests in
  flight ($L$ large) — e.g., pipelining/async I/O.
- If you can only have **one outstanding** request (synchronous RPC), throughput is
  capped at $1/W$ — latency directly limits throughput. **Pipeline** to break this.

---

## 18.4 Latency Components in IPC

```
 total latency ≈ syscall + copy(S) + context_switch + scheduling_delay
                 (+ network RTT for distributed)
```

- **Small messages** → syscall + context switch + scheduling **jitter** dominate.
- **Large messages** → copy/serialization (bandwidth) dominates.
- **Distributed** → **network RTT** usually dwarfs everything; minimize round trips.

### Tail latency (p99/p99.9)
Averages lie. A system can have a great **median** but terrible **tail** due to:
context-switch jitter, GC pauses, lock contention, page faults, queueing under load.
For user-facing systems, **tail latency** is often what matters — design to bound it
(timeouts, load shedding, avoiding unbounded queues).

---

## 18.5 Throughput Limiters

| Limiter | Symptom | Mitigation |
|---------|---------|------------|
| Per-op overhead (syscalls) | CPU high, small msgs | **Batch**, shared memory, io_uring |
| Memory-copy bandwidth | CPU in `memcpy`, large msgs | **Zero-copy** (Ch. 24), shared memory |
| Context-switch rate | High involuntary switches | Batch wakeups, busy-poll, fewer threads |
| Lock contention | Threads serialized on a lock | Sharding, lock-free, finer locks |
| Single connection / no pipelining | Underutilized link | **Pipeline**, multiple connections |
| Serialization (JSON) | CPU in encode/decode | Binary formats (Protobuf, Cap'n Proto) |

---

## 18.6 Techniques and Which Goal They Serve

| Technique | Helps Latency | Helps Throughput |
|-----------|:------------:|:----------------:|
| Batching / coalescing | ✗ (worse) | ✅ |
| Pipelining (many in flight) | ~ | ✅ |
| Shared memory / zero-copy | ✅ | ✅ |
| Busy-polling (spin) | ✅ | ~ (CPU cost) |
| `TCP_NODELAY` | ✅ | ✗ (more packets) |
| Larger buffers | ~ | ✅ (fewer blocks) |
| Compression | ✗ (CPU) | ✅ (if network-bound) |
| Async / event loop | ~ | ✅ |

(✅ helps, ✗ hurts, ~ depends)

---

## 18.7 A Decision Framework

1. **Classify the requirement:** Is this path **latency-critical** (trading, RPC for a
   user click) or **throughput-critical** (log ingestion, batch ETL, streaming)?
2. **Latency-critical:** minimize round trips and switches — shared memory + spin, or
   `TCP_NODELAY`, small/no batching, pre-faulting, pin threads to cores, bound the tail.
3. **Throughput-critical:** batch, pipeline, use zero-copy/large buffers, binary
   serialization, and keep many operations in flight (Little's Law).
4. **Both?** Use **adaptive batching** (batch under load, flush immediately when idle) —
   the standard compromise (e.g., Nagle-like timers, `io_uring` with small linger).

---

## 18.8 Text Diagram — Adaptive Batching

```
 if queue has ≥ K msgs  OR  timer ≥ T elapsed:
       flush batch
 else:
       wait (briefly)
   → low latency when idle (flush small), high throughput under load (big batches)
```

---

## 18.9 Key Takeaways

- **Latency** (per-op time) and **throughput** (work/time) are distinct and often
  **conflict**, chiefly via **batching**.
- **Little's Law** ($L = \lambda W$): without concurrency, latency caps throughput —
  **pipeline** to scale.
- Small-message latency is **switch/overhead bound**; large-message is **copy bound**;
  distributed is **RTT bound**.
- Watch **tail latency (p99+)**, not just averages, for user-facing systems.
- Use **adaptive batching** to get good throughput *and* latency across load levels.

---

[← Previous](17-data-consistency-ordering.md) | [Back to ToC](../README.md) | [Next: Mechanism Comparison →](19-mechanism-comparison.md)
