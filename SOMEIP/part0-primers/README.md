# Part 0 — Prerequisite Primers

> **Standalone background topics.** SOME/IP is built on top of decades of
> networking technology. If any term in the main book feels unfamiliar, read the
> relevant primer here first. Each primer is self-contained and uses the same
> teaching template as the main chapters: **Definition → Why needed → Problem
> solved → Tradeoffs → Analogy**.

These are **not** SOME/IP-specific. They are the foundation the protocol assumes
you already know. Skim what you know; study what you don't.

---

## Contents

| # | Primer | Read this before… |
|---|--------|-------------------|
| P1 | [The OSI & TCP/IP Models](01-osi-tcpip-model.md) | Chapter 11 (Protocol Stack) |
| P2 | [IP & Addressing (IPv4/IPv6)](02-ip-addressing.md) | Chapter 12 (Transport) |
| P3 | [UDP — User Datagram Protocol](03-udp.md) | Chapters 8, 12 |
| P4 | [TCP — Transmission Control Protocol](04-tcp.md) | Chapter 12 |
| P5 | [Ports & Sockets](05-ports-sockets.md) | Chapters 6, 12 |
| P6 | [Ethernet & Automotive Ethernet](06-ethernet-automotive.md) | Chapters 2, 12 |
| P7 | [Unicast, Multicast & Broadcast](07-multicast-unicast-broadcast.md) | Chapters 8, 9 |
| P8 | [Serialization, Bytes & Endianness](08-serialization-endianness.md) | Chapter 10 |
| P9 | [Communication Patterns: Client–Server, RPC & Pub/Sub](09-comm-patterns.md) | Chapters 5–8 |

---

## How the layers stack up

```
   APPLICATION LOGIC  (your ECU software, services)
   ────────────────────────────────────────────────
   SOME/IP  +  SOME/IP-SD        ← the main book
   ────────────────────────────────────────────────
   TCP   /   UDP                 ← Primers P3, P4
   ────────────────────────────────────────────────
   IP  (IPv4 / IPv6)             ← Primer P2
   ────────────────────────────────────────────────
   (Automotive) Ethernet         ← Primer P6
   ────────────────────────────────────────────────
   Physical wire (100/1000BASE-T1)
```

Read bottom-up if you want to build intuition from the wire upward, or top-down
if you want to start from SOME/IP and drill into what it relies on.

---

[Back to Main TOC](../README.md) | [Start: Primer P1 →](01-osi-tcpip-model.md)
