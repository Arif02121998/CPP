# Chapter 19 — Mechanism Comparison Tables

[← Back to Table of Contents](../README.md) | [← Previous](18-throughput-vs-latency.md)

---

## 19.1 How to Read These Tables

This chapter is the **at-a-glance reference** that ties Part III together. Use it to pick
a mechanism quickly, then read that mechanism's chapter for depth.

---

## 19.2 Master Comparison Table

| Mechanism | Direction | Data unit | Copies | Speed | Cross-host? | Built-in sync? | Persistence |
|-----------|-----------|-----------|:------:|-------|:-----------:|:--------------:|-------------|
| **Anonymous pipe** | One-way | Byte stream | 2 | Moderate | No | Yes (kernel) | Process |
| **Named pipe (FIFO)** | One-way | Byte stream | 2 | Moderate | No | Yes | Filesystem node |
| **Message queue** | One/many-way | Message | 2 | Moderate | No | Yes | Kernel |
| **Shared memory** | Any | Raw bytes | **0** | **Fastest** | No | **No (DIY)** | Kernel/named |
| **Memory-mapped file** | Any | Raw bytes | **0** | Fastest | No | **No (DIY)** | Disk (durable) |
| **Unix domain socket** | Two-way | Stream/dgram | 2 | Fast | No | Yes | Filesystem node |
| **TCP/IP socket** | Two-way | Stream | 2 + stack | Network-bound | **Yes** | Yes | Connection |
| **UDP socket** | Two-way | Datagram | 2 + stack | Network-bound | **Yes** | Yes (unreliable) | Connectionless |
| **Signals** | One-way | ~Number only | n/a | Fast (no data) | No | n/a | Transient |
| **RPC** | Two-way | Typed call | 2+ (serialize) | Network-bound | **Yes** | Yes | Per call |

---

## 19.3 Speed Ranking (same host, typical)

```
 Fastest ───────────────────────────────────────────► Slowest
 Shared memory ≈ mmap   >   Unix domain socket ≈ pipe ≈ msg queue   >   TCP loopback   >   TCP/RPC over network
 (zero-copy)               (2 copies, no IP stack)                    (full IP stack)     (+ network RTT)
```

> Caveat: "speed" depends on message size and pattern. Shared memory wins on **raw data
> path**, but you pay in **synchronization complexity**. For small control messages, the
> difference between pipe and UDS is often negligible.

---

## 19.4 Complexity Ranking (effort to use correctly)

```
 Simplest ──────────────────────────────────────────► Hardest
 Signals(send) < Pipe < FIFO < Message queue < Unix socket < TCP socket < RPC < Shared memory(+sync)
```

- **Signals**: trivial to send, but **handlers** are deceptively dangerous.
- **Shared memory**: simplest *data path*, but **hardest to get correct** (you own all
  synchronization, lifecycle, and crash recovery).

---

## 19.5 Decision Matrix — "I need to..."

| Goal | Best fit | Why |
|------|----------|-----|
| Stream bytes parent→child | **Anonymous pipe** | Simplest, inherited |
| Connect unrelated local procs simply | **FIFO / Unix socket** | Named, easy |
| Pass discrete commands with priorities | **Message queue** | Boundaries + types |
| Share large/high-rate data, lowest latency | **Shared memory** | Zero-copy |
| Persist shared state / map big file | **Memory-mapped file** | Shared + durable |
| Local client/server, fd passing | **Unix domain socket** | Fast + features |
| Talk across machines | **TCP socket / RPC** | Only cross-host options |
| Typed cross-language service calls | **RPC (gRPC)** | IDL + stubs |
| Notify "shut down / reload" | **Signal** | Channel-free poke |
| Decouple producers/consumers in time | **Message queue / broker** | Buffering |
| Lossy real-time (video/games) | **UDP** | Low latency, no retransmit stalls |

---

## 19.6 Capability Matrix

| Capability | Pipe | FIFO | Msg Q | Shm | mmap | UDS | TCP | UDP | Signal | RPC |
|------------|:----:|:----:|:-----:|:---:|:----:|:---:|:---:|:---:|:------:|:---:|
| Preserves message boundaries | ✗ | ✗ | ✅ | n/a | n/a | dgram | ✗ | ✅ | n/a | ✅ |
| Bidirectional | ✗ | ✗ | ~ | ✅ | ✅ | ✅ | ✅ | ✅ | ✗ | ✅ |
| Reliable/ordered | ✅ | ✅ | ✅ | DIY | DIY | ✅ | ✅ | ✗ | ✗ | ✅ |
| Cross-machine | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✅ | ✅ | ✗ | ✅ |
| Carries bulk data | ~ | ~ | ~ | ✅ | ✅ | ✅ | ✅ | ~ | ✗ | ✅ |
| Zero-copy data path | ✗ | ✗ | ✗ | ✅ | ✅ | ✗ | ✗* | ✗* | n/a | ✗ |
| Unrelated processes | ✗ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ | ✅ |
| Pass file descriptors | ✗ | ✗ | ✗ | ✗ | ✗ | ✅ | ✗ | ✗ | ✗ | ✗ |

(✅ yes, ✗ no, ~ partial/with effort, * unless using zero-copy syscalls; DIY = you add it)

---

## 19.7 Quick "Gotcha" Reminders per Mechanism

- **Pipe/FIFO:** no message framing; close unused ends or never get EOF; SIGPIPE.
- **Message queue:** kernel-persistent (leaks); small default size limits.
- **Shared memory / mmap:** no sync (races); use offsets not pointers; SIGBUS; unlink.
- **UDS:** stream has no boundaries; supports fd passing/peer creds.
- **TCP:** framing yourself; `TCP_NODELAY`; handle resets/partial I/O.
- **UDP:** unreliable/unordered; handle loss and reordering.
- **Signals:** async-signal-safety; standard signals coalesce.
- **RPC:** not a local call — latency, partial failure, idempotency.

---

## 19.8 Key Takeaways

- **Shared memory/mmap** = fastest (zero-copy) but you own synchronization.
- **Pipes/queues/UDS** = simple, safe, kernel-synced, ~2 copies, same-host.
- **TCP/RPC** = the only cross-machine options; pay network + serialization costs.
- Match the mechanism to **direction, data unit, host scope, and sync needs** — use the
  decision matrix (19.5) as your starting point.

---

[← Previous](18-throughput-vs-latency.md) | [Back to ToC](../README.md) | [Next: Local vs Distributed →](20-local-vs-distributed.md)
