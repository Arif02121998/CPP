# Chapter 6: Client–Server Model

[← Previous: Chapter 5](05-service-oriented-model.md) | [Back to TOC](../README.md) | [Next: Chapter 7 →](07-methods-vs-events.md)

---

## 6.1 Definition

In SOME/IP, the **server (provider)** is the entity that *offers* a service
instance; the **client (consumer)** is the entity that *uses* it. These are
**roles per service**, not fixed machine identities — one ECU can be a server
for one service and a client for another at the same time.

```
   ECU-A                         ECU-B
 ┌────────┐  client of Nav    ┌────────┐
 │  App1  │ ───────────────▶  │ NavSvc │  (B is server here)
 │        │                   │        │
 │ DoorSvc│ ◀───────────────  │  App2  │  (A is server here)
 └────────┘   client of Door  └────────┘
```

## 6.2 Why it is needed

- Establishes a clear **ownership** model: exactly one provider owns the
  authoritative state/logic of a service instance.
- Enables **directed** request/response semantics impossible on a pure broadcast
  bus.
- Maps cleanly onto **Service Discovery**: servers *offer*, clients *find* and
  *subscribe*.

## 6.3 Problem it solves

Broadcast buses have no notion of "who is responsible." Anyone can put a frame on
CAN; there's no authoritative responder. The client–server model gives every
service an **owner** and gives clients a **deterministic counterpart** to call.

## 6.4 Identity & addressing

A client reaches a specific server instance through:

```
(Service ID, Instance ID, Interface Version) → resolved by SD to → (IP, Port, UDP/TCP)
```

Each request carries a **Request ID** = `Client ID (16b) + Session ID (16b)` so
the client can match the asynchronous response to the original call and detect
loss/retransmission.

## 6.5 Tradeoffs

- **Pros:** clear ownership, directed comms, supports RPC semantics.
- **Cons:** the provider can become a **single point of failure** / bottleneck;
  may need redundancy or multiple instances; lifecycle (startup ordering, server
  unavailability) must be handled by clients.

## 6.6 Real-world analogy

A **help desk**. The desk (server) owns the ability to answer tickets; callers
(clients) dial in. Each caller gets a **ticket number** (Request ID) so the reply
is correlated to their specific question — even if many people called at once.

---

[← Previous: Chapter 5](05-service-oriented-model.md) | [Back to TOC](../README.md) | [Next: Chapter 7 →](07-methods-vs-events.md)
