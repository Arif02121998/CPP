# Chapter 25 — Lock-Free Communication

[← Back to Table of Contents](../README.md) | [← Previous](24-zero-copy.md)

---

## 25.1 What "Lock-Free" Means

**Lock-free** communication coordinates concurrent processes/threads **without using
mutual-exclusion locks**, relying instead on **atomic hardware instructions** and
**memory ordering**. The goal: avoid the costs and hazards of locks — context switches on
contention, priority inversion, deadlock, and the convoy effect.

Precise definitions (progress guarantees):
- **Lock-free:** *at least one* thread always makes progress (system never stalls), even
  if individual threads starve.
- **Wait-free:** *every* thread makes progress in a bounded number of steps (strongest).
- **Obstruction-free:** a thread makes progress if it runs in isolation (weakest).

> "Lock-free" ≠ "no atomic operations." It means **no locks**, so no thread holding a
> lock can block all others.

---

## 25.2 Why Avoid Locks?

| Lock problem | Lock-free benefit |
|--------------|-------------------|
| Holder descheduled → everyone waits | No holder; system keeps moving |
| Deadlock (circular waits) | Impossible (no locks to cycle) |
| Priority inversion | Avoided |
| Context switch on contention | Stays in user space (atomics) |
| Convoy effect under load | No serialization point |

For low-latency IPC (Ch. 23), the killer feature is **no context switch and no kernel
call** on the fast path.

---

## 25.3 The Building Block — Atomic Compare-And-Swap (CAS)

Almost all lock-free algorithms are built on **CAS** (compare-and-swap):

```
 CAS(addr, expected, new):
   atomically: if *addr == expected { *addr = new; return true }
               else { return false }
```

The pattern: read a value, compute a new one, **CAS** it in; if someone changed it
meanwhile, **retry**.

```
 do {
     old = load(ptr);
     new = compute(old);
 } while (!CAS(ptr, old, new));   // retry on conflict
```

Other atomics: `fetch_add`, `exchange`, `load`/`store` with ordering.

---

## 25.4 Memory Ordering — the Subtle Heart

CPUs and compilers **reorder** memory operations. Lock-free code must specify ordering so
that when a consumer observes a published index, it also observes the **data written
before** it (Ch. 17).

```
 Producer: store data;  store-release(index, n);   // data becomes visible before index
 Consumer: load-acquire(index);  read data;         // sees data if it sees index
```

- **release** (store): prior writes can't move *after* it.
- **acquire** (load): later reads can't move *before* it.
- Getting this wrong yields bugs that appear only on **weakly-ordered CPUs** (ARM,
  POWER) under load — notoriously hard to reproduce. x86 is more forgiving (TSO), which
  ironically hides bugs until you deploy on ARM.

---

## 25.5 The Workhorse — SPSC Ring Buffer

The most common, *easiest-correct* lock-free IPC structure: **single-producer,
single-consumer** ring in shared memory.

```
 Shared:  buffer[N], head (producer owns), tail (consumer owns)

 Producer:                          Consumer:
   if (head+1)%N == load_acq(tail)    if load_acq(head) == tail
        → full, spin/return              → empty, spin/return
   buffer[head] = item                item = buffer[tail]
   store_rel(head, (head+1)%N)        store_rel(tail, (tail+1)%N)
```

- Only one writer per index → **no CAS needed**, just release/acquire. Very fast.
- This (plus batching) is the LMAX **Disruptor** / DPDK ring idea.
- **MPSC / MPMC** rings (multiple producers/consumers) need **CAS** on the shared index
  and are substantially harder.

---

## 25.6 Hard Problems in Lock-Free Design

### The ABA problem
A value reads `A`, changes to `B`, then back to `A`. A `CAS(expected=A)` succeeds even
though the state churned — corrupting structures (esp. with pointers/free lists). Fixes:
**tagged pointers** (version counter packed with pointer), **hazard pointers**, or
**epoch-based reclamation**.

### Safe memory reclamation
In lock-free structures, you can't just `free` a node — another thread might still be
reading it. You need **hazard pointers**, **RCU** (read-copy-update), or **epoch
reclamation** to know when it's safe.

### Correctness is genuinely hard
Lock-free algorithms are **easy to get subtly wrong** and **hard to test** (bugs are
timing- and CPU-dependent). Prefer **proven, reviewed implementations** over rolling your
own.

---

## 25.7 RCU — Read-Copy-Update (a key pattern)

**RCU** allows **lock-free reads** with no atomics on the read side, while writers create
a **new copy** and atomically swap a pointer; old versions are freed after a **grace
period** when no reader can still hold them.

```
 Readers:  follow pointer, read (no locks, no atomics) — extremely cheap
 Writer:   copy → modify → atomically publish new pointer → reclaim old after grace period
```

Used heavily in the **Linux kernel** for read-mostly shared data. Ideal when reads vastly
outnumber writes.

---

## 25.8 When to Use / When Not to Use

**Use lock-free when:**
- **Ultra-low latency** / no tolerance for blocking (HFT, audio, packet processing).
- **High contention** where locks cause convoys.
- A simple, well-understood structure fits (**SPSC ring** is the sweet spot).
- Reads dominate writes → **RCU**.

**Avoid lock-free when:**
- A mutex (especially uncontended, futex-backed) is **fast enough** — it usually is.
- The structure is complex (MPMC, arbitrary graphs) — risk of subtle bugs outweighs
  gains.
- Your team can't deeply review/test it. **Correctness debt** here is brutal.

> **Default to locks; reach for lock-free only with a measured need and a proven
> implementation.** Premature lock-free is a top source of heisenbugs.

---

## 25.9 Key Takeaways

- Lock-free coordinates via **atomics + memory ordering**, not locks — avoiding
  deadlock, priority inversion, and context switches on the fast path.
- **CAS** + **release/acquire** ordering are the core tools; wrong ordering = rare,
  CPU-specific bugs.
- The **SPSC ring buffer** is the practical, correct-by-construction workhorse; **MPMC**
  and pointer-based structures are much harder (**ABA**, **safe reclamation**).
- **RCU** gives near-free lock-free reads for read-mostly data (used in the Linux kernel).
- **Default to (futex-backed) locks**; use lock-free only when measured latency/contention
  demands it, and prefer **battle-tested libraries**.

---

[← Previous](24-zero-copy.md) | [Back to ToC](../README.md) | [Next: Event-Driven Systems →](26-event-driven-systems.md)
