# Chapter 18: Common Mistakes & Anti-Patterns

[← Previous: Chapter 17](17-performance.md) | [Back to TOC](../README.md) | [Next: Chapter 19 →](19-interview-prep.md)

---

## 18.1 Why study anti-patterns?

Most production SOME/IP problems aren't protocol bugs — they're **design and
configuration mistakes**. Recognizing these patterns saves days of debugging and
is a frequent interview topic ("what would you do differently?").

## 18.2 Service Discovery misuse

| Anti-pattern | Why it hurts | Better approach |
|--------------|--------------|-----------------|
| Too-frequent Offer/Find cycles | Floods network, wastes bandwidth/CPU | Tune SD timings (initial delay, repetition, cyclic offer) |
| Hardcoding IP:port, skipping SD | Tight coupling; breaks on relocation | Let SD resolve endpoints at runtime |
| Ignoring SD state machine | Subscriptions never establish/recover | Honor offer/subscribe/ack flow & TTLs |
| No TTL / wrong TTL | Stale entries or constant re-offering | Set sensible TTLs for availability |

> **Symptom:** "Client can't find the service" → usually SD timing, multicast/IGMP
> config, or TTL — not the method itself.

## 18.3 Tight coupling in service design

- **Mistake:** designing services around *one specific consumer* or embedding
  consumer-specific logic in the provider.
- **Why it hurts:** adding a second consumer forces provider changes; defeats SOA.
- **Fix:** design **consumer-agnostic** interfaces; expose data/capabilities, not
  client-specific workflows. Keep the interface stable; evolve via versioning.

## 18.4 Wrong transport choice

| Mistake | Consequence | Fix |
|---------|-------------|-----|
| TCP for high-frequency cyclic events | Jitter, HOL blocking, latency spikes | UDP (multicast) for events |
| UDP for large critical bulk transfer | Loss, manual reassembly pain | TCP, or SOME/IP-TP if multicast needed |
| Relying on IP fragmentation | One lost fragment drops whole message | SOME/IP-TP segmentation |

## 18.5 Inefficient message handling

- **Oversized cyclic messages** — sending everything every cycle. Fix: send
  **on-change** or split hot vs cold data.
- **Chatty fine-grained methods** — many tiny calls where one batched call would
  do. Fix: coarser interfaces for remote calls (network ≠ local function cost).
- **Ignoring serialization cost** — deep nested types on the hot path. Fix: keep
  frequent messages small and flat.
- **No rate limiting** — a fast publisher overwhelming a slow subscriber. Fix:
  sensible cycle times / event throttling.

## 18.6 Treating RPC like a local call

- **Mistake:** assuming a method call always returns quickly and never fails.
- **Reality:** it crosses the network — it can **time out, fail, or lag**.
- **Fix:** always handle timeouts and error responses; prefer **fire-and-forget**
  where a reply isn't needed; design idempotent operations where possible.

## 18.7 Pub/Sub pitfalls

- **Assuming guaranteed delivery** over UDP multicast events. Events are
  best-effort — add **E2E protection / sequence counters** if loss matters.
- **No handling for missed updates** — design consumers to tolerate gaps (use
  latest-value semantics, not "every event is sacred").
- **Subscribing to everything** — wastes bandwidth/CPU. Subscribe only to needed
  eventgroups.

## 18.8 Versioning & interface evolution

- **Mistake:** changing field order/size without versioning → silent
  misinterpretation (serialization is positional, not self-describing).
- **Fix:** use **major/minor service versions**; add fields carefully; never
  repurpose existing field positions; coordinate provider/consumer rollout.

## 18.9 Security afterthoughts

- **Mistake:** open multicast, no authentication, trusting all SD offers.
- **Risks:** spoofed services, rogue subscribers, traffic injection.
- **Fix:** network segmentation (VLANs), authentication/integrity (E2E, IPsec/
  MACsec where applicable), and validating SD sources.

## 18.10 Debugging mindset (layer-by-layer)

When something "doesn't work," isolate the layer:

```
   App logic?  → check service/method IDs, payload, return codes
   SOME/IP?    → header fields, message type, session ID, serialization
   SD?         → offers/subscribes, TTL, eventgroup IDs, timings
   Transport?  → UDP vs TCP config, ports
   IP/Multicast→ IGMP joins, multicast group, switch snooping
   Ethernet?   → VLAN, link up, MTU, TSN config
```

Most "SOME/IP bugs" live in **SD timing** or **multicast/IGMP** configuration, not
the application code.

## 18.11 Quick do/don't summary

| Do | Don't |
|----|-------|
| Use SD for endpoint resolution | Hardcode IP:port |
| UDP+multicast for events | Use TCP for cyclic events |
| Design consumer-agnostic interfaces | Couple a service to one client |
| Send on-change, keep messages small | Blast oversized cyclic data |
| Handle timeouts/loss explicitly | Assume RPC/events always succeed |
| Version interfaces | Reorder fields silently |

---

[← Previous: Chapter 17](17-performance.md) | [Back to TOC](../README.md) | [Next: Chapter 19 →](19-interview-prep.md)
