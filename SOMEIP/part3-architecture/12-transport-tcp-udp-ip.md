# Chapter 12: Transport — TCP, UDP & IP Integration

[← Previous: Chapter 11](11-protocol-stack.md) | [Back to TOC](../README.md) | [Next: Chapter 13 →](13-middleware.md)

---

## 12.1 Definition

SOME/IP rides on standard **IP** transport. It can use **UDP** (default,
connectionless) or **TCP** (connection-oriented, reliable). The choice is made
**per service/method/event** in configuration, based on payload size and
reliability needs.

## 12.2 UDP vs TCP for SOME/IP

| Aspect | UDP | TCP |
|--------|-----|-----|
| Connection | Connectionless | Connection (handshake) |
| Reliability | None (app/E2E handles) | Reliable, ordered |
| Latency | Lower, predictable | Higher (ACKs, retransmit) |
| Overhead | Small (8-byte header) | Larger (state, ACKs) |
| Multicast | **Yes** (events to many) | No |
| Best for | Small, frequent, real-time msgs; events | Large/streamed payloads, diagnostics |
| Max practical payload | ≤ ~1400 B (avoid IP frag) or SOME/IP-TP | large streams |

> **Default stance:** UDP for cyclic events and small RPCs; TCP where payloads are
> large or must not be lost.

## 12.3 SOME/IP Transport Protocol (SOME/IP-TP)

UDP datagrams are limited (fragmenting at IP level is undesirable in embedded
networks). For large payloads over UDP, **SOME/IP-TP** **segments** a big message
into multiple UDP packets with offset/more-segments flags and **reassembles** at
the receiver — giving "large message" capability without TCP.

```
 Big payload (e.g., 9 KB)
   └─ TP segment 1 (offset 0,   more=1)
   └─ TP segment 2 (offset 1392,more=1)
   └─ ...
   └─ TP segment N (offset X,   more=0)  → receiver reassembles
```

## 12.4 IP layer concerns

- **Addressing:** IPv4 or IPv6; each ECU has IP(s); services bind to **ports**.
- **Multicast:** events to many subscribers use IP multicast groups (managed via
  IGMP/MLD and the Ethernet switch).
- **Static vs dynamic IP:** addresses may be statically configured or assigned;
  SD removes the need to hardcode them in the application.

## 12.5 Why this matters

- **Right transport per data type** minimizes latency and bandwidth.
- **Multicast on UDP** is what makes pub/sub to many consumers scalable.
- **SOME/IP-TP** avoids forcing TCP just because a payload is big.

## 12.6 Determinism: TSN

Plain Ethernet is **best-effort**. **TSN (Time-Sensitive Networking, IEEE 802.1)**
adds time synchronization (802.1AS), traffic shaping/scheduling (802.1Qbv), and
preemption (802.1Qbu) so high-priority SOME/IP traffic gets **bounded latency** —
approaching the determinism of FlexRay while keeping Ethernet's bandwidth.

## 12.7 Tradeoffs

- **UDP:** fast and multicast-capable, but you must handle loss/ordering at the
  application or via E2E protection.
- **TCP:** reliable, but head-of-line blocking and jitter hurt real-time events.
- **Multicast:** scales fan-out, but needs switch/IGMP configuration and is
  harder to secure and debug.

## 12.8 Real-world analogy

- **UDP** = sending postcards: cheap, fast, no guarantee each arrives.
- **TCP** = registered mail with tracking and re-delivery: reliable but slower.
- **SOME/IP-TP** = shipping a big item as numbered boxes the receiver
  reassembles.
- **TSN** = a dedicated express lane that guarantees priority mail arrives on
  time.

---

[← Previous: Chapter 11](11-protocol-stack.md) | [Back to TOC](../README.md) | [Next: Chapter 13 →](13-middleware.md)
