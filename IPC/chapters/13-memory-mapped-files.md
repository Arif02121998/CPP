# Chapter 13 — Memory-Mapped Files

[← Back to Table of Contents](../README.md) | [← Previous](12-rpc.md)

---

## 13.1 Definition

A **memory-mapped file** maps the contents of a file (or a shared anonymous region)
directly into a process's virtual address space via **`mmap`**. Reads and writes to that
memory are transparently backed by the file's pages. When **two processes map the same
file with `MAP_SHARED`**, they share those pages — giving **file-backed shared memory**.

Memory-mapped files sit at the intersection of **file I/O** and **shared memory**:
you get shared-memory-style access *and* optional persistence to disk.

---

## 13.2 How It Works Conceptually

```
   Process A                         Process B
   mmap(file, MAP_SHARED)            mmap(file, MAP_SHARED)
        │                                  │
        ▼                                  ▼
   ┌──────────── page cache (kernel) ─────────────┐
   │  file pages cached in RAM (shared frames)    │
   └───────────────────────┬──────────────────────┘
                           │  (kernel flushes dirty pages)
                           ▼
                    ┌─────────────┐
                    │  disk file  │   ← persistence
                    └─────────────┘
```

- Accessing mapped memory the first time triggers a **page fault**; the kernel loads
  that page from the file into the **page cache** (demand paging).
- With `MAP_SHARED`, writes go to the page cache (shared with other mappers) and are
  **written back** to disk by the kernel (lazily, or on `msync`).
- With `MAP_PRIVATE` (copy-on-write), writes are **private** to the process and **not**
  shared or persisted.
- **No explicit `read`/`write` syscalls** for data access — it's just memory.

---

## 13.3 Why It Exists (Problem It Solves)

Two problems, one mechanism:

1. **Efficient file access** — Traditional `read`/`write` copies data between the page
   cache and your buffer (a copy + a syscall per call). `mmap` lets you access file data
   **in place**, avoiding that copy and enabling random access like an array. Great for
   **large files**, databases, and loading executables/shared libraries.

2. **Persistent / large shared memory** — When two processes map the same file
   `MAP_SHARED`, they share memory **and** get durability and a filesystem name for free.
   Useful when shared state must **survive restarts** or be larger than convenient to
   manage with anonymous shared memory.

---

## 13.4 Performance Characteristics

- **Latency (access):** very low — direct memory access after the page is faulted in.
- **Copies:** **zero** between page cache and user space (the mapping *is* the page
  cache). Contrast with `read()`, which copies cache→user buffer.
- **First-touch cost:** page faults + disk reads on initial access (demand paging).
  **Pre-faulting** (`MAP_POPULATE`) or **`madvise(WILLNEED)`** can mitigate.
- **Write-back:** dirty pages flush asynchronously; `msync` forces it. I/O latency is
  hidden until you need durability.
- **Random access:** excellent — no `lseek`; just index memory.

---

## 13.5 Tradeoffs

| Dimension | Memory-Mapped File |
|-----------|--------------------|
| **Speed** | Fast, zero-copy access; random access friendly |
| **Complexity** | Moderate — faults, flushing, sizing, error handling |
| **Safety** | Page faults can become **`SIGBUS`** (e.g., file truncated/I/O error) |
| **Persistence** | Yes (MAP_SHARED to a real file) — survives process exit |
| **Synchronization** | None for inter-process writes — you must add it (like shared mem) |
| **Scope** | Same host (shared pages); file may live on shared storage but mmap-sharing is local |

---

## 13.6 When to Use / When Not to Use

**Use memory-mapped files when:**
- Accessing **large files** with **random access** (databases, indexes, search engines).
- You want **shared memory that persists** to disk or has a filesystem name.
- **Loading** executables/shared libraries or large read-only datasets (demand-paged,
  shared across processes automatically).
- Zero-copy file access matters for throughput.

**Avoid when:**
- **Small or streaming sequential** I/O → plain `read`/`write` is simpler and may be
  faster (readahead, no fault overhead).
- You can't handle **`SIGBUS`/I/O errors** surfacing as faults.
- **Append-heavy / frequently growing** files → resizing a mapping (`mremap`, re-`mmap`)
  is awkward.
- Networked filesystems where `mmap` coherency/semantics are weak.

---

## 13.7 Relationship to Plain Shared Memory

| | Anonymous shared memory | Memory-mapped file (MAP_SHARED) |
|--|-------------------------|----------------------------------|
| **Backing** | RAM only (or swap) | A real file (page cache → disk) |
| **Persistence** | Gone on last unmap/reboot | Survives; durable via msync |
| **Naming** | shm name / inheritance | Filesystem path |
| **Use case** | Pure fast IPC | IPC + persistence, or fast file access |

POSIX `shm_open` actually returns an fd you `mmap` — under the hood, shared memory and
memory-mapped files use the **same `mmap` machinery**.

---

## 13.8 Common Pitfalls

- **No synchronization between processes** — same hazards as shared memory; add
  semaphores/mutexes/atomics for concurrent writers.
- **`SIGBUS` on access** if the file is **truncated** below the accessed offset, or on
  I/O error. Size the file (`ftruncate`) before mapping and handle errors.
- **Assuming immediate durability** — writes sit in the page cache; call **`msync`**
  (and possibly `fsync`) when you need data on disk.
- **Mapping a growing file** — the mapping doesn't auto-extend; manage size explicitly.
- **Storing absolute pointers** in a shared mapping — use **offsets** (same as shared
  memory).
- **Page-fault latency spikes** in latency-critical paths — pre-fault or `mlock`.

---

## 13.9 Key Takeaways

- `mmap` maps a file into memory: **zero-copy, random-access** file I/O and, with
  `MAP_SHARED`, **file-backed shared memory** that can **persist**.
- It unifies **file access** and **shared memory** — POSIX shared memory is literally
  `mmap` under the hood.
- You still own **synchronization** for concurrent writers (just like shared memory).
- Watch for **`SIGBUS` on truncation**, **deferred durability** (use `msync`), and
  **first-touch page-fault** costs.
- Ideal for **databases, large files, persistent shared state**; overkill for small
  sequential I/O.

---

[← Previous](12-rpc.md) | [Back to ToC](../README.md) | [Next: Kernel Involvement and Data Paths →](14-kernel-involvement-data-paths.md)
