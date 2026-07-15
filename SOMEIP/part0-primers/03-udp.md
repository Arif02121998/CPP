# Primer P3: UDP — User Datagram Protocol

[← Previous: P2 — IP & Addressing](02-ip-addressing.md) | [Primers Index](README.md) | [Next: P4 — TCP →](04-tcp.md)

---

## P3.1 Definition

**UDP (User Datagram Protocol)** is a **connectionless**, **best-effort**
transport protocol. It adds just four things to raw IP: **source port**,
**destination port**, **length**, and **checksum** — an 8-byte header. It sends
independent packets called **datagrams** with no setup and no guarantees.

```
   UDP header (8 bytes)
   ┌────────────┬────────────┬──────────┬──────────┐
   │ Src Port   │ Dst Port   │ Length   │ Checksum │
   │ (2 bytes)  │ (2 bytes)  │ (2 bytes)│ (2 bytes)│
   └────────────┴────────────┴──────────┴──────────┘
   └──────────────── then your data ────────────────┘
```

## P3.2 Why it is needed

Many real-time messages — a sensor reading, a cyclic status event — need to go out
**fast** and **often**. They don't need retransmission; if one update is lost,
the next one (milliseconds later) makes it irrelevant. UDP delivers exactly this:
minimum overhead, minimum delay.

## P3.3 Problem it solves

- **Low latency** — no handshake, no waiting for ACKs.
- **Low overhead** — tiny header, no connection state to maintain.
- **Multicast capability** — one datagram can reach many receivers at once
  (TCP cannot do this). This is what makes SOME/IP **pub/sub events** scale.

## P3.4 What UDP does NOT do

| Missing guarantee | Consequence | Who handles it if you care |
|-------------------|-------------|----------------------------|
| Reliability | Packets can be lost silently | App logic / E2E protection |
| Ordering | Packets can arrive out of order | App logic / sequence numbers |
| Duplicate protection | Same packet can arrive twice | App logic |
| Flow/congestion control | Can overwhelm a slow receiver | App design / rate limiting |
| Large messages | Limited by MTU (~1500 B) | **SOME/IP-TP** segmentation |

## P3.5 Why SOME/IP loves UDP

SOME/IP uses **UDP by default** because automotive traffic is dominated by small,
frequent, latency-sensitive messages and one-to-many events:

- Cyclic events (speed, RPM) → fire-and-forget, multicast to subscribers.
- Service Discovery → multicast offers/finds.
- Small request/response methods → fast round trips.

For payloads larger than a datagram, **SOME/IP-TP** segments the message across
multiple UDP datagrams instead of switching to TCP.

## P3.6 Tradeoffs

- ✅ Fast, lightweight, multicast-friendly, predictable timing.
- ❌ You inherit responsibility for loss, ordering, and large-message handling.
- ❌ No built-in security or congestion control.

## P3.7 Real-world analogy

UDP is **shouting an announcement across a room** (or over a PA system):

- You speak once; you don't wait for each person to confirm they heard.
- If someone missed it, that's okay — you'll announce the next update shortly.
- With a PA system, **everyone** hears it at once (multicast).

Fast and efficient, but if a single critical word *must* be heard, shouting is
the wrong tool — you'd want TCP.

---

[← Previous: P2 — IP & Addressing](02-ip-addressing.md) | [Primers Index](README.md) | [Next: P4 — TCP →](04-tcp.md)
