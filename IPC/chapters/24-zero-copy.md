# Chapter 24 — Zero-Copy Mechanisms

[← Back to Table of Contents](../README.md) | [← Previous](23-high-performance-systems.md)

---

## 24.1 What "Zero-Copy" Means

**Zero-copy** refers to techniques that move data between a source and destination
**without the CPU copying the bytes through user-space buffers**. The data may still be
moved by hardware (DMA) or remapped, but the expensive **CPU `memcpy`** steps are
eliminated.

Why it matters: for large transfers, **memory-copy bandwidth** — not syscalls — is the
bottleneck (Ch. 4, 18). Removing copies directly raises throughput and frees CPU.

---

## 24.2 The Problem: the Traditional Copy Path

Sending a file over a socket the naive way involves **four copies and four context
switches**:

```
 1. read():  disk ─DMA─► kernel page cache ─copy─► user buffer    (copy 1)
 2. send():  user buffer ─copy─► kernel socket buffer             (copy 2)
 3.          socket buffer ─DMA─► NIC                             (DMA)
   plus 2 read+2 write mode switches
```

Copies 1 and 2 are pure CPU overhead — the data didn't need to visit user space at all.

---

## 24.3 Technique 1 — `sendfile()`

`sendfile(out_fd, in_fd, ...)` tells the kernel to copy data **from a file to a socket
entirely within the kernel**:

```
 disk ─DMA─► page cache ─► (kernel) ─► socket/NIC
   • no copy into user space
   • with scatter-gather DMA, the page-cache→socket copy is also avoided
```

- Cuts copies from 4 to **1 or 0** and halves context switches.
- Used by web servers (nginx, Apache), file servers — anywhere you ship file bytes to a
  socket unmodified.
- **Limitation:** you can't *transform* the data (it never enters your process).

---

## 24.4 Technique 2 — `splice()` / `vmsplice()` / `tee()`

`splice()` moves data between **two file descriptors via a kernel pipe buffer** without
copying to user space — more general than `sendfile` (works between pipes, sockets,
files).

```
 fd_in ─► [kernel pipe] ─► fd_out      (pages moved by reference, not copied)
```

- `vmsplice()` maps user pages **into** a pipe (gift pages to the kernel) — zero-copy
  from user memory.
- `tee()` duplicates a pipe's contents to another pipe without consuming it.
- Powerful for building zero-copy proxies and pipelines.

---

## 24.5 Technique 3 — `MSG_ZEROCOPY` (sockets)

For TCP/UDP sends, the `MSG_ZEROCOPY` flag lets the kernel transmit **directly from the
user buffer** via DMA, skipping the copy into the socket buffer:

```
 send(..., MSG_ZEROCOPY): user pages ─DMA─► NIC   (no socket-buffer copy)
   • completion is notified asynchronously (you must not modify the buffer until then)
```

- Big win for **large** sends; overhead (pinning pages, async completion bookkeeping)
  makes it *not* worth it for small messages.

---

## 24.6 Technique 4 — Memory-Mapped I/O (`mmap`)

`mmap` a file and the **page cache pages are the user-visible pages** — accessing them
copies **nothing** between cache and user space (Ch. 13).

```
 read():  page cache ─copy─► user buffer
 mmap():  page cache  ═(mapped)═ user accesses directly   (zero copy)
```

Combine `mmap` (read side) with `write`/`send` for partial zero-copy, or just access
file data in place.

---

## 24.7 Technique 5 — Shared Memory (the ultimate zero-copy)

The most complete form: with shared memory, there is **no transfer at all** — producer
and consumer touch the **same physical pages** (Ch. 9). Zero copies, zero syscalls on the
data path. Everything else in this chapter is about *approaching* what shared memory gives
natively (but shared memory is same-host only).

---

## 24.8 Technique 6 — Scatter-Gather (Vectored) I/O

`readv()`/`writev()` and `sendmsg()` with multiple buffers let you send **non-contiguous**
buffers (e.g., a header struct + a payload) in **one syscall without concatenating** them
into a temporary buffer first:

```
 writev(fd, [ {header, 16}, {payload, 64KB} ])   // one call, no merge copy
```

Saves both a copy (the concatenation) and a syscall.

---

## 24.9 Technique 7 — RDMA (zero-copy across machines)

**Remote Direct Memory Access** lets one machine's NIC read/write another machine's
memory **without involving the remote CPU or kernel** — zero-copy *and* kernel-bypass
across the network (Ch. 23). The closest thing to distributed shared memory.

---

## 24.10 When Zero-Copy Helps — and When It Doesn't

| Situation | Zero-copy worth it? |
|-----------|--------------------|
| Large file/data transfers | ✅ Big win (bandwidth-bound) |
| High-throughput servers | ✅ Frees CPU, raises ceiling |
| Small messages | ✗ Setup overhead > copy saved |
| Data must be transformed in user space | ✗ (sendfile/splice can't transform) |
| Same-host, max performance | ✅ Use shared memory directly |

> **Rule of thumb:** zero-copy pays off when **copy cost dominates** (large payloads).
> For small messages, the bookkeeping (page pinning, async completion) can be *slower*.

---

## 24.11 Costs and Caveats

- **Page pinning / lifetime:** the buffer must stay valid and unmodified until the async
  send completes (`MSG_ZEROCOPY`); managing this adds complexity.
- **Reduced flexibility:** kernel-internal moves (`sendfile`) mean your app can't inspect
  or transform the data.
- **Alignment / page granularity:** zero-copy often works in **page units**; unaligned or
  tiny transfers don't benefit.
- **Complexity:** more failure modes; only adopt with a measured bottleneck.

---

## 24.12 Key Takeaways

- Zero-copy eliminates **CPU copies** (and often syscalls), crucial when **bandwidth**
  is the bottleneck for large transfers.
- Toolbox: **`sendfile`**, **`splice`/`vmsplice`**, **`MSG_ZEROCOPY`**, **`mmap`**,
  **scatter-gather I/O**, **shared memory** (same host), **RDMA** (cross host).
- **Shared memory** is the ultimate zero-copy locally; the syscalls just *approach* it.
- Zero-copy **helps large payloads, hurts small ones** (setup overhead) and **limits
  data transformation** — measure before adopting.

---

[← Previous](23-high-performance-systems.md) | [Back to ToC](../README.md) | [Next: Lock-Free Communication →](25-lock-free-communication.md)
