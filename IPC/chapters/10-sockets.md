# Chapter 10 — Sockets (Unix Domain and TCP/IP)

[← Back to Table of Contents](../README.md) | [← Previous](09-shared-memory.md)

---

## 10.1 Definition

A **socket** is a bidirectional communication endpoint. The **same API** (`socket`,
`bind`, `listen`, `accept`, `connect`, `send`, `recv`) works for:

- **Unix domain sockets (UDS, `AF_UNIX`)** — IPC between processes on the **same host**.
- **Internet sockets (`AF_INET`/`AF_INET6`)** — communication **across the network**
  using TCP or UDP.

This uniformity is sockets' superpower: write to the socket API and you can run locally
(UDS) or across machines (TCP) with minimal change.

---

## 10.2 How It Works Conceptually

### Connection-oriented (TCP / UDS stream)

```
 Server                                   Client
 socket()                                 socket()
 bind(addr)                               
 listen()  ──── waiting ────              
 accept() ◄──────────────────  connect(addr)
   │   (new connected socket)             │
   ▼                                       ▼
 recv/send  ◄────── byte stream ──────► send/recv
```

- **Stream sockets** (TCP, UDS-stream): reliable, ordered, **byte stream** — like a
  bidirectional pipe. No message boundaries (you frame messages yourself).
- **Datagram sockets** (UDP, UDS-dgram): message-oriented, **preserve boundaries**,
  but (for UDP) **unreliable/unordered**.

### Unix domain vs TCP loopback
Both can connect two local processes. **UDS is faster**: it skips the TCP/IP stack
(no checksums, no protocol headers, no routing) and just moves bytes through kernel
socket buffers. UDS also supports special tricks: **passing file descriptors** and
**peer credentials** (`SCM_RIGHTS`, `SO_PEERCRED`).

---

## 10.3 Why It Exists (Problem It Solves)

- **TCP/IP sockets** solve **communication across machines**: reliable, ordered byte
  delivery over an unreliable network, with addressing (`IP:port`) and flow control.
- **Unix domain sockets** solve **local client/server IPC** with the *same programming
  model* as network code — so a service can serve local and remote clients uniformly,
  and you can develop locally then scale out.

The unifying value: **one API spanning local and distributed** communication.

---

## 10.4 Performance Characteristics

| | Unix domain socket | TCP loopback | TCP over network |
|--|--------------------|--------------|------------------|
| **Latency** | Low (kernel buffers only) | Higher (full TCP stack) | + network RTT |
| **Throughput** | High | High but more CPU | Limited by link/RTT |
| **Copies** | 2 (user↔kernel) | 2 + protocol processing | 2 + NIC DMA |
| **Overhead** | Minimal | Checksums, headers, congestion ctrl | + retransmits, routing |

- All socket transfers normally **copy twice**. **Zero-copy** options exist
  (`sendfile`, `splice`, `MSG_ZEROCOPY`) — see Ch. 24.
- TCP adds **connection setup** (3-way handshake), **congestion/flow control**, and
  retransmission — great for correctness over networks, pure overhead on loopback.

---

## 10.5 Tradeoffs

| Dimension | Sockets |
|-----------|---------|
| **Speed** | UDS: fast; TCP local: moderate; TCP remote: network-bound |
| **Complexity** | Moderate — connection lifecycle, framing, error handling |
| **Safety** | High — isolated, kernel-managed; TCP adds reliability |
| **Reach** | **Only mechanism that spans machines** (TCP) |
| **Structure** | Stream (TCP/UDS): frame yourself; Datagram (UDP): boundaries kept |
| **Features** | UDS: fd passing, peer creds; TCP: reliability, ordering |

---

## 10.6 When to Use / When Not to Use

**Use sockets when:**
- Processes may be on **different machines** (TCP) — the default for distributed systems.
- You want a **client/server** model with many connections (event loop + epoll).
- Local IPC where you value the **portable socket API** or features like **fd passing**
  (UDS) — common for daemons, container runtimes, databases.

**Avoid / reconsider when:**
- You need the **absolute lowest latency / highest bandwidth** locally → shared memory.
- Tiny related-process streaming → a pipe is simpler.
- You're using TCP **loopback** when UDS would be faster for same-host comms.

---

## 10.7 Common Pitfalls

- **Assuming message boundaries on TCP/stream sockets.** `recv` returns *whatever is
  available*; you must implement framing (length prefix or delimiter). Short reads/writes
  are normal.
- **Ignoring partial writes.** `send` may accept fewer bytes than requested under
  backpressure; loop until done.
- **`SIGPIPE` on write to a closed peer.** Ignore `SIGPIPE` / use `MSG_NOSIGNAL`.
- **Nagle's algorithm latency.** TCP may delay small packets; set `TCP_NODELAY` for
  latency-sensitive request/response.
- **Not handling `ECONNRESET`/EOF.** Peers die; detect and clean up connections.
- **Address-already-in-use** after restart → set `SO_REUSEADDR`.
- **Unbounded connections** without an event loop → use `epoll`/`kqueue` to scale.

---

## 10.8 Text Diagram — Local Path Comparison

```
 Unix domain socket:   A ─► [kernel socket buffer] ─► B        (no IP stack)

 TCP loopback:         A ─► [TCP][IP][loopback dev][IP][TCP] ─► B   (full stack)

 Shared memory:        A ─► [same RAM page] ◄─ B                (zero copy, no kernel)
```

---

## 10.9 Key Takeaways

- Sockets give **one bidirectional API for both local (UDS) and remote (TCP/IP)** IPC.
- **Stream** sockets are byte streams (frame yourself); **datagram** sockets preserve
  message boundaries (UDP is unreliable).
- **UDS beats TCP loopback** locally (skips the IP stack) and adds **fd passing** and
  **peer credentials**.
- **TCP** provides reliability/ordering across networks at the cost of stack overhead.
- Mind **framing, partial I/O, SIGPIPE, TCP_NODELAY**, and use **epoll** to scale.

---

[← Previous](09-shared-memory.md) | [Back to ToC](../README.md) | [Next: Signals →](11-signals.md)
