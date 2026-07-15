# Appendix B: Quick Reference Cheat Sheet

[← Previous: Appendix A](appendix-a-glossary.md) | [Back to TOC](../README.md)

---

## B.1 SOME/IP in one paragraph

SOME/IP is an **automotive service-oriented middleware over IP**. ECUs **offer**
and **consume services** (methods + events + fields) over **UDP/TCP/Ethernet**,
find each other at runtime via **SOME/IP-SD**, exchange **compact big-endian**
serialized messages, fan out **events via multicast**, send **large payloads via
SOME/IP-TP**, and achieve **real-time determinism with TSN**.

## B.2 Message header (field order)

```
   ┌────────────────────────────────────────────────┐
   │ Message ID   = Service ID (16) + Method/Event ID(16) │
   │ Length        (32)                                   │
   │ Request ID   = Client ID (16) + Session ID (16)      │
   │ Protocol Version (8) | Interface Version (8)         │
   │ Message Type (8) | Return Code (8)                   │
   │ Payload …                                            │
   └────────────────────────────────────────────────┘
```

## B.3 Message types

| Type | Meaning |
|------|---------|
| REQUEST | Method call expecting a response |
| REQUEST_NO_RETURN | Fire-and-forget method |
| RESPONSE | Reply to a request |
| NOTIFICATION | Event/eventgroup notification |
| ERROR | Error response |

## B.4 Interaction patterns

| Pattern | Transport | Cast | Use |
|---------|-----------|------|-----|
| Request/Response | UDP or TCP | Unicast | Query/command with reply |
| Fire-and-forget | UDP or TCP | Unicast | Command, no reply |
| Event (pub/sub) | UDP (often) | Multicast | State change / cyclic data |
| Field getter/setter | UDP or TCP | Unicast | Read/write state |
| Field notifier | UDP | Multicast | Subscribe to state changes |

## B.5 Transport decision

```
   Small, frequent, real-time, one-to-many? ──▶ UDP (+ multicast)
   Large, must-not-lose, ordered/streamy?   ──▶ TCP
   Large but event-like / needs multicast?  ──▶ SOME/IP-TP over UDP
   Hard real-time bounded latency required? ──▶ add TSN
```

## B.6 Service Discovery flow

```
   Provider ── Offer (multicast) ──▶ network
   Consumer ── Find  (multicast) ──▶ network
   Consumer ── SubscribeEventgroup ─▶ Provider
   Provider ── SubscribeAck ────────▶ Consumer
   Provider ── Notification(s) ─────▶ Consumer(s)   (cyclic/on-change)
```

## B.7 Request/Response flow

```
   Client: build request (Msg ID, Request ID, REQUEST) → serialize → send
   Server: receive → deserialize → execute method → build RESPONSE
           (echo Request ID, set Return Code) → serialize → send
   Client: match Session ID → deserialize → use result
```

## B.8 Protocol stack

```
   Application (services)
   SOME/IP  +  SOME/IP-SD
   UDP / TCP
   IP (IPv4/IPv6)
   (Automotive) Ethernet
   Physical (100/1000BASE-T1)
```

## B.9 Versus other tech (one-liners)

| Tech | Pick it when… |
|------|---------------|
| **SOME/IP** | Automotive service comms over Ethernet (AUTOSAR) |
| **DDS** | Rich-QoS data-centric pub/sub (robotics/aero) |
| **gRPC** | Cloud microservices, many languages |
| **REST** | Open, human-readable web APIs |
| **CAN/FlexRay** | Hard-real-time low-bandwidth control |

## B.10 Common mistakes → fixes

| Mistake | Fix |
|---------|-----|
| Hardcoding IP:port | Use SD |
| TCP for cyclic events | UDP + multicast |
| IP fragmentation for big msgs | SOME/IP-TP |
| Assuming events never drop | E2E sequence counters |
| Reordering serialized fields | Version interfaces |
| Service coupled to one client | Consumer-agnostic interface |
| Oversized cyclic payloads | On-change, small flat messages |

## B.11 Debug checklist (layer order)

```
   App        → IDs, payload, return codes, interface version
   SOME/IP    → header fields, message type, serialization match
   SD         → offer/subscribe, eventgroup IDs, TTL, timings
   Transport  → UDP/TCP config, ports
   IP/MCast   → multicast group, IGMP join, switch snooping
   Ethernet   → VLAN, link, MTU, TSN
```

## B.12 Numbers worth remembering

| Item | Value |
|------|-------|
| UDP header | 8 bytes |
| TCP header | 20+ bytes |
| SOME/IP header | 16 bytes |
| Typical Ethernet MTU | ~1500 bytes |
| Byte order | Big-endian |
| CAN bandwidth | ~1 Mbit/s |
| Automotive Ethernet | 100 Mbit/s – 1 Gbit/s+ |

---

[← Previous: Appendix A](appendix-a-glossary.md) | [Back to TOC](../README.md)

---

*End of the SOME/IP Study Guide. Return to the [Main TOC](../README.md) to revisit any chapter.*
