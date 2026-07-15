# Chapter 17: Performance & Real-Time Constraints

[← Previous: Chapter 16](16-comparison.md) | [Back to TOC](../README.md) | [Next: Chapter 18 →](18-common-mistakes.md)

---

## 17.1 What "performance" means here

In automotive distributed systems, performance is not just throughput. It is a
balance of four often-competing properties:

| Property | Question it answers |
|----------|---------------------|
| **Latency** | How long from send to receive? |
| **Jitter** | How *consistent* is that latency? |
| **Bandwidth** | How many bytes/sec can the link carry? |
| **Determinism** | Is worst-case timing *bounded* and guaranteed? |

For control data, **bounded worst-case latency** often matters more than raw
average speed.

## 17.2 Sources of latency in a SOME/IP call

```
   App serialize → SOME/IP header → UDP/TCP → IP → Ethernet/switch
        → wire → switch → IP → UDP/TCP → SOME/IP parse → App deserialize
```

| Stage | Latency contributor | Mitigation |
|-------|---------------------|------------|
| Serialization | CPU time to encode payload | Compact layout, fixed types |
| Transport | TCP handshake/ACKs, retransmit | Use UDP for cyclic/small data |
| Network | Switch queuing, contention | **TSN** scheduling/priority |
| Reassembly | SOME/IP-TP segmentation | Keep payloads ≤ MTU when possible |
| OS scheduling | Task wakeups, copies | RT OS, zero-copy, priorities |

## 17.3 Bandwidth & overhead

Every message carries protocol overhead. For a small payload, headers can dominate:

```
   Ethernet(14+) + IP(20) + UDP(8) + SOME/IP(16) = ~58 bytes of header
   For an 8-byte payload → overhead is >85% of the packet.
```

**Implications:**
- **Batch** related data into one message where sensible (fewer, larger messages).
- Avoid needlessly high event frequencies — a 1 kHz event that changes rarely
  wastes bandwidth; consider **on-change** instead of cyclic.
- Multicast events once instead of N unicast copies.

## 17.4 Serialization overhead

SOME/IP's schema-driven, mostly-positional format is deliberately **compact** and
**fast to parse** (no field-name tags like JSON). Still:

- Complex nested structures and variable-length arrays cost CPU and bytes.
- Excessive optional/dynamic fields slow parsing and bloat size.
- Keep frequently-sent event payloads **small and flat**.

## 17.5 UDP vs TCP — the real-time lens

| | UDP | TCP |
|---|-----|-----|
| Latency | Low, predictable | Higher, variable |
| Jitter | Low | **High** (retransmit, HOL blocking) |
| Multicast | Yes | No |
| Real-time events | ✅ Preferred | ❌ Avoid for cyclic data |
| Large reliable transfer | Needs TP | ✅ Natural fit |

> **Head-of-line blocking** is the classic TCP real-time killer: a lost packet
> stalls *fresh* data behind *stale* data in the receive buffer.

## 17.6 Achieving determinism: TSN

Plain Ethernet is **best-effort** — a burst of infotainment traffic could delay a
safety event. **TSN (Time-Sensitive Networking)** restores bounded latency:

| TSN standard | Role |
|--------------|------|
| 802.1AS | Time synchronization across ECUs |
| 802.1Qbv | Time-aware scheduling (gated traffic windows) |
| 802.1Qbu / 802.3br | Frame preemption for urgent traffic |
| 802.1Qav | Credit-based traffic shaping |

Result: high-priority SOME/IP traffic gets **guaranteed time windows**,
approaching FlexRay-like determinism while keeping Ethernet's bandwidth.

## 17.7 Real-time design checklist

- ✅ Pick **UDP** for cyclic/small/multicast; **TCP** only for large/reliable.
- ✅ Keep hot-path payloads **small, flat, ≤ MTU**.
- ✅ Prefer **on-change** events over high-rate cyclic where possible.
- ✅ Use **multicast** for fan-out, not N unicasts.
- ✅ Use **TSN** + traffic priorities for safety-critical streams.
- ✅ Budget **worst-case**, not average, latency.
- ✅ Apply **E2E protection** where data integrity is safety-relevant.

## 17.8 Tradeoffs

- Lower latency (UDP) ↔ you handle loss/ordering yourself.
- Determinism (TSN) ↔ added network configuration and cost.
- Compact serialization ↔ less self-describing, stricter version discipline.
- Fewer/larger messages ↔ potentially higher per-message latency.

## 17.9 Real-world analogy

Think of a **highway system**:

- **Bandwidth** = number of lanes.
- **Latency** = time to cross town.
- **Jitter** = how much that time varies by traffic.
- **TSN** = dedicated bus/emergency lanes with synchronized green lights, so
  ambulances (safety data) always arrive on a guaranteed schedule no matter how
  congested the regular lanes get.

---

[← Previous: Chapter 16](16-comparison.md) | [Back to TOC](../README.md) | [Next: Chapter 18 →](18-common-mistakes.md)
