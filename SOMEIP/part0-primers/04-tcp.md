# Primer P4: TCP — Transmission Control Protocol

[← Previous: P3 — UDP](03-udp.md) | [Primers Index](README.md) | [Next: P5 — Ports & Sockets →](05-ports-sockets.md)

---

## P4.1 Definition

**TCP (Transmission Control Protocol)** is a **connection-oriented**, **reliable**,
**ordered**, byte-stream transport protocol. Before any data flows, two endpoints
establish a connection; TCP then guarantees that bytes arrive **completely**,
**in order**, and **without duplicates**, retransmitting anything lost.

## P4.2 Why it is needed

Some data must arrive intact: a firmware update, a diagnostic log, a large
configuration blob. A single missing byte could corrupt the whole thing. TCP
handles all the bookkeeping — acknowledgments, retransmission, ordering — so the
application just reads a clean stream.

## P4.3 The 3-way handshake

```
   Client                         Server
     │ ───────── SYN ──────────────▶ │   "Let's talk"
     │ ◀────── SYN-ACK ───────────── │   "OK, and you?"
     │ ───────── ACK ──────────────▶ │   "Confirmed"
     │ ====== connection open ====== │
```

This setup costs a round trip **before** any real data — fine for long-lived or
large transfers, wasteful for a single tiny message.

## P4.4 What TCP guarantees

| Guarantee | How | Benefit |
|-----------|-----|---------|
| Reliability | ACKs + retransmission | No silent data loss |
| Ordering | Sequence numbers | Bytes reassembled in order |
| Flow control | Receiver advertises window | Won't overwhelm a slow receiver |
| Congestion control | Adapts send rate | Plays nicely with shared networks |
| No duplicates | Sequence tracking | Each byte delivered once |

## P4.5 The cost: latency & jitter

- **Connection setup** adds a round trip.
- **Head-of-line blocking** — if packet #3 is lost, packets #4, #5 wait in the
  buffer until #3 is retransmitted. For a periodic real-time event, this stalls
  *fresh* data behind *stale* data — bad for control loops.
- **No multicast** — TCP is strictly one-to-one, so it can't fan out events.

## P4.6 When SOME/IP uses TCP

SOME/IP uses TCP **selectively**, configured per service/method, when:

- Payloads are **large** (and you'd rather not segment over UDP).
- Data **must not be lost** and ordering matters (e.g., diagnostics, bulk transfer).
- The communication is more **streamy** than cyclic.

For cyclic events and small RPCs, UDP remains the default.

## P4.7 UDP vs TCP at a glance

| | UDP | TCP |
|---|-----|-----|
| Connection | None | Handshake required |
| Reliability | Best-effort | Guaranteed |
| Ordering | No | Yes |
| Multicast | **Yes** | No |
| Latency | Lower, predictable | Higher, variable (jitter) |
| Header | 8 bytes | 20+ bytes |
| Best for | Events, small RPCs | Large/critical transfers |

## P4.8 Real-world analogy

TCP is **registered mail with tracking and guaranteed delivery**:

- You first confirm the recipient is ready to receive (handshake).
- Each item is numbered; the carrier re-sends anything that gets lost.
- Everything arrives in the order you sent it.

Reliable and orderly — but slower, more paperwork, and you can only address it to
**one** recipient at a time.

---

[← Previous: P3 — UDP](03-udp.md) | [Primers Index](README.md) | [Next: P5 — Ports & Sockets →](05-ports-sockets.md)
