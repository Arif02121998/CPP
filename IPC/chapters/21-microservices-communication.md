# Chapter 21 — Microservices Communication

[← Back to Table of Contents](../README.md) | [← Previous](20-local-vs-distributed.md)

---

## 21.1 The Setting

A **microservice** architecture splits an application into many small, independently
deployable services. They are **separate processes on separate machines**, so *all*
their interaction is **distributed IPC**. The choices here are the practical application
of Chapters 10, 12, and 20.

```
   [API Gateway]
        │
   ┌────┼─────────────┬──────────────┐
   ▼    ▼             ▼              ▼
 [Auth][Orders]────►[Payments]    [Inventory]
   │      │  events     │              │
   └──────┴──────►[ Message Broker ]◄──┘
```

---

## 21.2 Two Communication Styles

### Synchronous (request/response) — RPC / HTTP
Service A **calls** service B and **waits** for a reply.

- **Technologies:** gRPC (HTTP/2 + Protobuf), REST/HTTP+JSON, GraphQL.
- **Good for:** queries needing an immediate answer, commands that must confirm.
- **Cost:** **temporal coupling** — A is blocked and *depends on B being up*. Failures
  and latency **propagate** (a slow B slows A).

### Asynchronous (event/message) — Broker
Service A **publishes** a message/event; B consumes it later. They never wait on each
other directly.

- **Technologies:** Kafka, RabbitMQ, NATS, AWS SQS/SNS, Pulsar.
- **Good for:** events ("OrderPlaced"), work distribution, fan-out, decoupling.
- **Benefit:** **temporal decoupling** — B can be down; the broker buffers. Better fault
  isolation and elasticity.
- **Cost:** eventual consistency, harder to trace, message ordering/dedup concerns.

---

## 21.3 Synchronous vs Asynchronous — Decision Table

| Question | Lean Synchronous (RPC) | Lean Asynchronous (Broker) |
|----------|------------------------|-----------------------------|
| Need an immediate result? | ✅ | ✗ |
| Caller can tolerate callee downtime? | ✗ | ✅ |
| Fan-out to many consumers? | ✗ | ✅ |
| Strong, simple consistency? | ✅ | harder (eventual) |
| Smooth load spikes / buffering? | ✗ | ✅ (queue absorbs) |
| Simple request/response semantics? | ✅ | more moving parts |

Most real systems use **both**: gRPC for queries/commands, a broker for events.

---

## 21.4 The Message Broker Pattern

A broker is a distributed, often durable, version of a message queue (Ch. 8) across hosts.

```
 Producer ─► [ Broker: topic/queue, persisted ] ─► Consumer group(s)
                     │ buffers, retries, replays
                     └─ decouples producer/consumer in time and rate
```

- **Queue (point-to-point):** each message consumed by **one** worker — work distribution.
- **Pub/Sub (topic):** each message delivered to **all** subscribers — event fan-out.
- **Log-based (Kafka):** an append-only, replayable log; consumers track an **offset**
  and can re-read history; great for event sourcing and high throughput.

**Delivery semantics** (Ch. 17): brokers typically give **at-least-once** → make
consumers **idempotent**.

---

## 21.5 Cross-Cutting Concerns (the hard 90%)

Distributed IPC needs infrastructure that local IPC never did:

| Concern | What it solves | Tooling |
|---------|----------------|---------|
| **Service discovery** | Find where B lives | DNS, Consul, Kubernetes Services |
| **Load balancing** | Spread calls across B's replicas | client-side LB, Envoy, k8s |
| **Timeouts & retries** | Don't hang on slow/dead peers | gRPC deadlines, retry policies |
| **Circuit breakers** | Stop hammering a failing service | resilience libs, mesh |
| **Backpressure** | Prevent overload cascades | bounded queues, rate limits |
| **Serialization/schema** | Cross-language data, evolution | Protobuf, Avro + schema registry |
| **Observability** | Trace a request across services | OpenTelemetry, distributed tracing |
| **Security** | Authn/z, encryption in transit | mTLS, JWT, service mesh |

A **service mesh** (Istio/Linkerd with Envoy sidecars) implements many of these
transparently, so application code stays focused on business logic.

---

## 21.6 Failure Patterns to Design For

- **Cascading failure:** B slows → A's threads block on B → A exhausts resources → A's
  callers fail. **Fix:** timeouts + circuit breakers + bulkheads (isolate resource pools).
- **Retry storms:** everyone retries a struggling service at once → makes it worse.
  **Fix:** exponential backoff + jitter, retry budgets.
- **Dual-write / consistency:** updating a DB *and* publishing an event isn't atomic.
  **Fix:** **transactional outbox** (write event to DB in same txn, relay to broker).
- **Lost/duplicated messages:** at-least-once delivery. **Fix:** idempotent consumers,
  dedup keys.

---

## 21.7 The Saga Pattern (distributed transactions)

You can't use a single ACID transaction across services. A **saga** is a sequence of
local transactions, each publishing an event that triggers the next, with
**compensating actions** to undo on failure.

```
 Order ─► reserve inventory ─► charge payment ─► ship
   on failure at payment: compensate → release inventory, cancel order
```

Trades strong consistency for **availability + eventual consistency**.

---

## 21.8 Common Mistakes

- **Too fine-grained / chatty services** → N network calls per request (latency blowup).
  Co-locate or coarsen APIs; batch.
- **Distributed monolith** → services so coupled they must deploy together; you got the
  costs of distribution without the benefits.
- **Synchronous chains** (A→B→C→D) → latency adds up and any link failing fails all.
  Prefer async events where possible.
- **No idempotency** with at-least-once delivery → duplicate side effects.
- **Premature microservices** → distribution overhead before you need scale (Ch. 31).

---

## 21.9 Key Takeaways

- Microservices make **all** communication **distributed IPC** — apply Ch. 20's rules.
- **Synchronous RPC** (gRPC) for immediate request/response; **async brokers** (Kafka,
  RabbitMQ) for events, decoupling, and fan-out. Most systems use both.
- Brokers give **time/rate decoupling** and durability but **at-least-once** delivery →
  build **idempotent consumers**.
- Invest in **discovery, timeouts, retries+backoff, circuit breakers, tracing, and
  security** — often via a **service mesh**.
- Beware **cascading failures, retry storms, chatty/over-fine services**, and the
  **distributed monolith**.

---

[← Previous](20-local-vs-distributed.md) | [Back to ToC](../README.md) | [Next: OS-Level Services →](22-os-level-services.md)
