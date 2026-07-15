# Primer P9: Communication Patterns — Client–Server, RPC & Pub/Sub

[← Previous: P8 — Serialization & Endianness](08-serialization-endianness.md) | [Primers Index](README.md) | [Back to Main TOC](../README.md)

---

## P9.1 Definition

A **communication pattern** is a reusable shape for how two or more parties
exchange information. The three that underpin SOME/IP are:

- **Client–Server** — one side requests, the other provides.
- **RPC (Remote Procedure Call)** — calling a function on another machine as if it
  were local (request → response).
- **Publish–Subscribe (Pub/Sub)** — producers publish data; interested consumers
  subscribe and receive it, without the producer knowing each consumer.

## P9.2 Client–Server

```
   CLIENT ──── request ───▶ SERVER
   CLIENT ◀─── response ─── SERVER
```

- The **client** initiates; the **server** owns the resource/logic and responds.
- **Why:** centralizes authority over data/behavior; clients stay simple.
- **SOME/IP mapping:** a service **provider** is the server; consumers are clients.

## P9.3 RPC (Request/Response)

RPC makes a remote call **look like** a local function call:

```
   result = getTemperature(zone)   // looks local…
   //  → serialize args → send request → server runs → response → deserialize
```

- **Why:** developer ergonomics — think in functions, not packets.
- **Tradeoff:** hides the network. A "function call" can now fail, time out, or be
  slow — you must design for latency and errors that local calls never have.
- **SOME/IP mapping:** **methods** are RPC. They can be:
  - **Request/Response** — expects a reply.
  - **Fire-and-forget** — no reply expected.

## P9.4 Publish–Subscribe

```
   PUBLISHER ──┬──▶ Subscriber A
               ├──▶ Subscriber B
               └──▶ Subscriber C
   (publisher doesn't know who/how many subscribers exist)
```

- **Why:** **decouples** producers from consumers in identity, number, and time.
  The publisher emits data once; the infrastructure delivers to all subscribers.
- **Tradeoff:** looser control and harder debugging — you can't easily tell who is
  listening; delivery (over UDP multicast) may be best-effort.
- **SOME/IP mapping:** **events** and **eventgroups** are pub/sub. Consumers
  **subscribe** via Service Discovery; publishers send notifications (often
  multicast) to all subscribers.

## P9.5 Choosing between them

| Need | Pattern | SOME/IP feature |
|------|---------|-----------------|
| "Do X and tell me the result" | RPC request/response | Method (R/R) |
| "Do X, I don't need a reply" | Fire-and-forget | Method (FF) |
| "Tell me whenever Y changes" | Pub/Sub | Event / Eventgroup |
| "Many consumers want the same stream" | Pub/Sub + multicast | Eventgroup over UDP |
| "One authoritative responder" | Client–Server | Service provider |

## P9.6 Coupling: the big idea

- **Tight coupling** — caller must know the callee's identity, location, and
  availability up front. Brittle; changing one side breaks the other.
- **Loose coupling** — parties interact through a discovered, well-defined
  interface. Either side can change location or be swapped, as long as the
  interface holds.

SOME/IP pushes toward **loose coupling** via **Service Discovery** (find services
at runtime) and **interfaces** (agreed methods/events), which is what makes
service-oriented architectures scalable and maintainable.

## P9.7 Real-world analogies

- **Client–Server / RPC** = ordering at a restaurant: you (client) ask the kitchen
  (server) for a dish and wait for it to come back.
- **Fire-and-forget** = dropping a suggestion in a suggestion box — you don't wait
  for a reply.
- **Pub/Sub** = subscribing to a magazine: the publisher prints once and every
  subscriber gets a copy; new subscribers can join without the publisher
  redesigning anything.

---

[← Previous: P8 — Serialization & Endianness](08-serialization-endianness.md) | [Primers Index](README.md) | [Back to Main TOC](../README.md)
