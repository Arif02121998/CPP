# Chapter 1: Introduction to SOME/IP

[← Back to Table of Contents](../README.md) | [Next: Chapter 2 →](02-why-someip.md)

---

## 1.1 What is SOME/IP?

**SOME/IP** (Scalable service-Oriented MiddlewarE over IP) is an automotive
communication **middleware** that lets software components (running on different
ECUs) talk to each other using a **service-oriented** model over standard
**Ethernet/IP** networks.

Instead of thinking in terms of *"signal X is on the bus at byte 3"*, SOME/IP
lets you think in terms of *"the Braking **service** offers a `getSpeed()`
**method** and a `SpeedChanged` **event**."*

> **One-line definition:** SOME/IP is the RPC + pub/sub middleware of Automotive
> Ethernet — it turns ECUs into providers and consumers of *services*.

It was developed by BMW around 2011–2013 and later standardized within
**AUTOSAR** (both Classic and Adaptive Platforms). It is the on-wire protocol
that AUTOSAR's `ara::com` and Classic AUTOSAR's `SOME/IP Transformer` use.

| Property | Value |
|----------|-------|
| Full name | Scalable service-Oriented MiddlewarE over IP |
| Layer | Application/Middleware (on top of TCP/UDP/IP) |
| Transport | UDP (default), TCP (large/streamed payloads) |
| Paradigm | Service-Oriented (RPC + Publish/Subscribe) |
| Standardized by | AUTOSAR |
| Typical use | In-vehicle ECU ↔ ECU communication over Automotive Ethernet |

## 1.2 What problem does it address (in one paragraph)?

Modern cars contain 70–150 ECUs and increasingly powerful domain controllers
(ADAS, infotainment, connectivity). These need to exchange **large, dynamic,
bidirectional** data — camera frames, navigation data, diagnostics — which
**does not fit** the rigid, low-bandwidth, statically-configured world of CAN.
SOME/IP brings the **flexibility of IT-world service architectures** (think
microservices) to the **deterministic, embedded** automotive domain.

## 1.3 Where SOME/IP sits (mental picture)

```
   Application logic  (ara::com / RTE)
 ┌─────────────────────────────────────┐
 │         SOME/IP  (this book)         │  ← serialization + RPC + pub/sub
 │      SOME/IP-SD (service discovery)  │
 ├─────────────────────────────────────┤
 │            UDP   /   TCP             │  ← transport
 ├─────────────────────────────────────┤
 │                IP                    │  ← addressing/routing
 ├─────────────────────────────────────┤
 │     Ethernet (Automotive 100/1000)   │  ← physical/data-link
 └─────────────────────────────────────┘
```

## 1.4 The five-part template (used throughout this book)

| Element | SOME/IP answer |
|---------|----------------|
| **Definition** | Service-oriented middleware over IP |
| **Why needed** | CAN/LIN/FlexRay can't scale to high-bandwidth, dynamic comms |
| **Problem solved** | Decouples *what* data means from *where/how* it's transported |
| **Tradeoffs** | More CPU, memory, and protocol complexity than raw signals |
| **Analogy** | Like REST/microservices, but tuned for embedded real-time cars |

---

[← Back to Table of Contents](../README.md) | [Next: Chapter 2 →](02-why-someip.md)
