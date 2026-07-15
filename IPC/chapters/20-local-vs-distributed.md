# Chapter 20 — Local IPC vs Distributed Communication

[← Back to Table of Contents](../README.md) | [← Previous](19-mechanism-comparison.md)

---

## 20.1 The Fundamental Divide

The single biggest architectural fork in IPC: are the communicating processes on the
**same machine** (local) or on **different machines** (distributed)?

```
 LOCAL                              DISTRIBUTED
 ┌────────── one host ──────────┐   ┌── host A ──┐        ┌── host B ──┐
 │  P1  ◄── shared RAM ──►  P2  │   │    P1   ───┼─network─┼──►  P2    │
 │  (kernel, memory bus)        │   │            │  (NIC)  │           │
 └──────────────────────────────┘   └────────────┘        └────────────┘
   ns–µs, reliable, shared clock      µs–ms, can fail, no shared memory/clock
```

This divide changes **performance, failure modes, and the entire programming model**.

---

## 20.2 What Changes When You Cross the Network

| Aspect | Local IPC | Distributed |
|--------|-----------|-------------|
| **Latency** | ns (shm) to µs | µs (LAN) to 100s ms (WAN) |
| **Bandwidth** | Memory bus (10s GB/s) | Link speed (often far less) |
| **Shared memory** | Yes (shm, mmap) | **Impossible** |
| **Failure model** | Peer crashes (detectable) | **Partial failure** — peer or *network* fails, often **indistinguishable** |
| **Clocks** | One clock | **No shared clock** (must sync, still skewed) |
| **Serialization** | Often unnecessary (shared structs) | **Always required** (bytes on the wire) |
| **Security** | Process/OS boundaries | **Untrusted network** — need auth + encryption |
| **Ordering/delivery** | Strong (kernel) | Weak — loss, reorder, duplicate possible |

---

## 20.3 The Eight Fallacies of Distributed Computing

Designers repeatedly make false assumptions. The classic fallacies:

1. The network is **reliable**.
2. Latency is **zero**.
3. Bandwidth is **infinite**.
4. The network is **secure**.
5. Topology **doesn't change**.
6. There is **one administrator**.
7. Transport cost is **zero**.
8. The network is **homogeneous**.

Every one is false. Local IPC lets you *ignore* most of them; distributed IPC **forces**
you to design for them (timeouts, retries, encryption, versioning, backpressure).

---

## 20.4 Partial Failure: The Defining Challenge

Locally, a function call either runs or the process crashes — binary and observable.
Across a network, a request can result in:

```
 • success (reply received)
 • failure before execution (never ran)
 • failure after execution (ran, but reply lost)  ← you can't tell this from "never ran"
 • slow (still running? or dead? — timeout ambiguity)
```

The killer: **you often cannot distinguish "it didn't happen" from "it happened but I
didn't hear back."** This is why distributed systems need **idempotency, timeouts,
retries with dedup, and reconciliation** — concepts irrelevant to local IPC.

---

## 20.5 Latency Numbers Every Engineer Should Know (approx.)

```
 L1 cache reference                  ~1 ns
 Main memory reference               ~100 ns
 Shared-memory IPC (same host)       ~0.1–1 µs
 Context switch                      ~1–5 µs
 Pipe / UDS round trip               ~2–10 µs
 TCP loopback round trip             ~10–30 µs
 Same-datacenter network RTT         ~0.5 ms (500 µs)
 Cross-continent network RTT         ~100–150 ms
```

Takeaway: a network hop can be **100–100,000×** slower than local IPC. **Minimize round
trips** in distributed designs; co-locate latency-sensitive components.

---

## 20.6 When to Stay Local vs Go Distributed

**Stay local (same host) when:**
- Components are tightly coupled and **latency-sensitive**.
- You can deploy/scale them together.
- Shared memory / UDS gives you the performance you need.

**Go distributed when:**
- You must **scale beyond one machine** (CPU, memory, storage).
- You need **independent deployment / fault isolation** across teams (microservices).
- Components run in different **trust/availability domains** or geographies.
- High availability requires **redundancy** across hosts.

> **Don't distribute prematurely.** A network boundary adds latency, failure modes, and
> operational cost. "You don't need microservices" until scaling/organizational pressure
> justifies them. (See Ch. 31 on overengineering.)

---

## 20.7 The Spectrum (it's not binary)

```
 Same process     Same host                         Different hosts
 (threads)   →   pipes/UDS/shm   →   loopback TCP   →   LAN RPC   →   WAN / Internet
 no IPC          fast local IPC      local network      µs–ms          10s–100s ms
                                     stack                              + unreliable
```

Many systems **start local and grow distributed**. Designing with a **transport-agnostic
API** (e.g., RPC over UDS now, TCP later) eases that transition — one reason RPC/sockets
are popular even for local use.

---

## 20.8 Bridging the Two Worlds

- **Sockets** deliberately use **one API** for UDS (local) and TCP (remote) so code ports
  easily.
- **Service meshes / sidecars** make remote calls look local to the app while handling
  retries, TLS, and discovery.
- **Message brokers** (Kafka, RabbitMQ) provide distributed, durable queuing analogous to
  local message queues but across hosts.

---

## 20.9 Key Takeaways

- **Local vs distributed** is the top-level IPC decision; it changes performance by
  **orders of magnitude** and introduces **partial failure** and **no shared clock**.
- Distributed IPC must respect the **eight fallacies**: design for unreliability,
  latency, security, and change.
- **Partial failure** (can't tell "didn't run" from "ran but reply lost") forces
  **idempotency, timeouts, retries, and reconciliation**.
- A network hop is **100–100,000× slower** than local IPC — **minimize round trips**.
- **Don't distribute prematurely**; use a **transport-agnostic API** to keep the door
  open.

---

[← Previous](19-mechanism-comparison.md) | [Back to ToC](../README.md) | [Next: Microservices Communication →](21-microservices-communication.md)
