# Chapter 12 — Remote Procedure Call (RPC)

[← Back to Table of Contents](../README.md) | [← Previous](11-signals.md)

---

## 12.1 Definition

**Remote Procedure Call (RPC)** is a communication *abstraction* that lets a program
call a function that **executes in another process** (possibly on another machine) as
if it were a **local function call**. The messy details — serialization, transport,
waiting for the reply — are hidden behind generated **stub** code.

RPC is not a kernel primitive; it's a **higher-level pattern built on top of** sockets
(or pipes/UDS locally). Examples: gRPC, Apache Thrift, JSON-RPC, Cap'n Proto RPC,
SunRPC, D-Bus method calls.

---

## 12.2 How It Works Conceptually

```
 Client process                                  Server process
 ┌───────────────────┐                          ┌───────────────────┐
 │ result = add(2,3) │  (looks local)           │  int add(a,b){...} │
 │        │          │                          │        ▲          │
 │        ▼          │                          │        │          │
 │  client STUB      │                          │  server STUB      │
 │  marshal args ───►│  bytes over transport    │◄─ unmarshal args  │
 │                   │ ───────────────────────► │   call real add() │
 │  unmarshal result │◄─────────────────────────│   marshal result  │
 │        ▲          │                          │                   │
 └────────┼──────────┘                          └───────────────────┘
          └─ returns 5 to caller
```

Steps in a call:
1. **Marshal (serialize)** arguments into bytes (Protobuf, JSON, XDR, ...).
2. **Transport** the request (HTTP/2 for gRPC, TCP, UDS locally).
3. Server **unmarshals**, dispatches to the real function.
4. Result is **marshalled back** and returned to the caller.

An **Interface Definition Language (IDL)** (e.g., `.proto`) describes methods and types;
a compiler generates client/server **stubs**, giving type-safety across languages.

---

## 12.3 Why It Exists (Problem It Solves)

Hand-writing socket code means defining wire formats, framing, serialization, request
matching, and error handling for every service. RPC solves the
**"make distributed calls feel like local calls"** problem:
- **Abstraction**: developers think in **functions and types**, not bytes and packets.
- **Interoperability**: an IDL lets a Go client call a Java server safely.
- **Productivity**: stubs, versioning, and boilerplate are generated.

It's the backbone of **microservices** and **client/server** architectures.

---

## 12.4 Performance Characteristics

- **Latency:** dominated by **network round trip** + **(de)serialization**. Each call is
  at least one RTT. Binary formats (Protobuf, Cap'n Proto) are far faster than JSON/XML.
- **Throughput:** depends on transport and serialization efficiency; gRPC uses HTTP/2
  multiplexing and supports **streaming**.
- **Overhead vs raw sockets:** RPC adds serialization and framing cost but rarely changes
  the order of magnitude; the dominant cost is usually the network itself.
- **Local RPC** (over UDS) avoids the network but still pays (de)serialization.

---

## 12.5 Tradeoffs

| Dimension | RPC |
|-----------|-----|
| **Speed** | Network-bound; serialization adds overhead |
| **Complexity** | Hidden from callers, but infra is complex (discovery, retries, TLS) |
| **Safety/typing** | Strong with IDL-generated stubs |
| **Coupling** | Risk of **tight coupling**; schema versioning needed |
| **Failure model** | **Partial failure** is fundamental — see below |
| **Reach** | Cross-process and cross-machine |

---

## 12.6 The Hard Truth: RPC Is Not a Local Call

The "looks like a local function" abstraction is **leaky** — and pretending otherwise
causes real bugs. Unlike local calls, RPC has:

- **Network latency** — orders of magnitude slower; never call in a hot loop naively.
- **Partial failure** — the call may **fail or time out**, and you may not know whether
  the server actually executed it.
- **Idempotency concerns** — retrying after a timeout can **double-execute** a
  non-idempotent operation (e.g., "charge card"). Design idempotent operations / use
  dedup keys.
- **Serialization limits** — you can't pass pointers, file handles, or callbacks freely.

> The classic *"Fallacies of Distributed Computing"* warn: the network is **not**
> reliable, zero-latency, infinite-bandwidth, or secure. RPC hides the API, not reality.

---

## 12.7 When to Use / When Not to Use

**Use RPC when:**
- Building **microservices** / service-to-service calls with clear request/response.
- You want **cross-language**, type-safe interfaces (gRPC + Protobuf).
- You need **structured APIs** with evolving schemas and tooling (auth, tracing, retries).

**Avoid / reconsider when:**
- **Chatty, fine-grained** calls would multiply latency → batch, or co-locate, or use a
  coarser API.
- You need **fire-and-forget, decoupled** delivery → use a **message broker** (Ch. 21).
- **Same-host, ultra-low-latency, high-bandwidth** → shared memory.
- Strong autonomy/loose coupling is paramount → consider async messaging/events.

---

## 12.8 RPC vs Messaging (important contrast)

| | RPC (request/response) | Messaging (broker/queue) |
|--|------------------------|--------------------------|
| **Coupling** | Caller waits for callee (temporal coupling) | Decoupled in time |
| **Pattern** | Synchronous call/return | Async publish/consume |
| **Failure** | Caller sees errors directly | Broker buffers, retries, replays |
| **Best for** | Queries, commands needing a result | Events, work distribution, fan-out |

Many systems use **both**: RPC for synchronous queries, messaging for async events.

---

## 12.9 Common Pitfalls

- **Treating RPC as free/local** → chatty designs, latency blowups (N+1 calls).
- **No timeouts/retries/circuit breakers** → cascading failures when a service is slow.
- **Naive retries on non-idempotent ops** → duplicate side effects.
- **Schema incompatibility** → breaking changes without versioning crash clients.
- **Ignoring backpressure** in streaming RPC → memory blowups.

---

## 12.10 Key Takeaways

- RPC makes a **remote function call look local**, hiding marshalling and transport
  behind generated **stubs** driven by an **IDL**.
- It powers **microservices** and gives **cross-language, type-safe** APIs.
- The abstraction is **leaky**: account for **latency, partial failure, idempotency,
  timeouts, and retries** — RPC hides the API, not the network.
- Use **RPC for synchronous request/response**, **messaging for async/decoupled** flows;
  use **shared memory** for same-host high-performance needs.

---

[← Previous](11-signals.md) | [Back to ToC](../README.md) | [Next: Memory-Mapped Files →](13-memory-mapped-files.md)
