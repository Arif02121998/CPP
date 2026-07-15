# Chapter 19: Interview Preparation

[← Previous: Chapter 18](18-common-mistakes.md) | [Back to TOC](../README.md) | [Next: Appendix A →](appendix-a-glossary.md)

---

## 19.1 How to use this chapter

Questions are grouped **Beginner → Intermediate → Advanced**, then **scenario**,
**design**, and **debugging**. Try to answer before reading the model answer.
Each answer is intentionally concise — expand with examples in a real interview.

---

## 19.2 Beginner questions

**Q1. What is SOME/IP?**
A service-oriented middleware that runs over IP (UDP/TCP) to enable ECUs to offer
and consume **services** (methods + events) in automotive Ethernet networks. It is
part of AUTOSAR (Classic & Adaptive).

**Q2. What problem does it solve over CAN/LIN?**
CAN/LIN are low-bandwidth, signal-based, statically wired, with no service concept.
SOME/IP brings **high bandwidth (Ethernet)**, **services**, **runtime discovery**,
and **large payloads** — enabling ADAS, cameras, infotainment, and OTA.

**Q3. What are the two main interaction types?**
- **Methods** (RPC): request/response or fire-and-forget.
- **Events** (pub/sub): notifications to subscribers, often via multicast.

**Q4. What is a service in SOME/IP?**
A logical unit identified by a **Service ID**, exposing **methods**, **events**,
and **fields**, offered by a provider as one or more **instances**.

**Q5. UDP or TCP — which does SOME/IP use?**
Both, configurable per element. **UDP by default** (low latency, multicast for
events); **TCP** for large or reliability-critical payloads.

---

## 19.3 Intermediate questions

**Q6. What is SOME/IP-SD and why is it needed?**
The Service Discovery sub-protocol. Providers **Offer** services; consumers **Find**
and **Subscribe** to eventgroups. It removes hardcoded IP:port, enabling **loose
coupling** and runtime availability handling.

**Q7. Explain eventgroups.**
A logical grouping of events a consumer subscribes to as a unit. Subscriptions are
managed via SD (Subscribe / SubscribeAck), and notifications are delivered
(often multicast) to subscribers.

**Q8. Walk through the SOME/IP message header fields.**
Message ID (Service ID + Method/Event ID), Length, Request ID (Client ID + Session
ID), Protocol Version, Interface Version, **Message Type** (request/response/
notification/error), **Return Code**, then payload.

**Q9. How does serialization work and why big-endian?**
Schema-driven, mostly **positional** (compact, no field tags), **big-endian**
(network byte order) so heterogeneous CPUs agree on byte order.

**Q10. What is SOME/IP-TP?**
A segmentation/reassembly mechanism for sending **large payloads over UDP**
(beyond MTU) without IP fragmentation and while keeping multicast capability.

**Q11. Difference between a field, an event, and a method?**
- **Method:** callable operation (RPC).
- **Event:** notification pushed on change/cyclically.
- **Field:** a value with optional **getter/setter** (methods) **and** a notifier
  (event) — i.e., state you can read, write, and subscribe to.

---

## 19.4 Advanced questions

**Q12. How do you achieve real-time determinism over Ethernet?**
Use **UDP** to avoid TCP jitter/HOL blocking, multicast for fan-out, and **TSN**
(802.1AS time sync, 802.1Qbv scheduling, frame preemption) for bounded latency.
Budget **worst-case**, apply traffic priorities.

**Q13. How does SOME/IP fit in the AUTOSAR Adaptive stack?**
It's the communication binding under `ara::com` — applications use service-oriented
APIs; the binding maps them to SOME/IP messages, with SD handling discovery.

**Q14. How do you handle interface evolution/versioning?**
Use **major/minor versions** (Interface Version in the header). Add fields
carefully; never reorder/repurpose existing positions (serialization is positional);
coordinate provider/consumer rollout; major bump for breaking changes.

**Q15. How is reliability handled if events use best-effort UDP multicast?**
Application-level measures: **E2E protection** (CRC + sequence counters per
AUTOSAR E2E), latest-value semantics, and using TCP where guaranteed delivery is
required.

**Q16. Security considerations?**
Authenticate/validate SD offers, segment with VLANs, integrity/authenticity via
**E2E**, and link-layer/network security (**MACsec/IPsec**) where applicable.
Multicast and open SD are attack surfaces.

---

## 19.5 Scenario-based questions

**S1. A client reports "service not found" intermittently. Causes?**
SD timing (offer/find intervals, initial delay), **multicast/IGMP** misconfig,
TTL expiry, VLAN isolation, or provider not yet in "available" state. Check SD
traffic first, not the method.

**S2. Events arrive late and with jitter under load. Why and fix?**
Likely TCP (HOL blocking) or best-effort Ethernet contention. Switch events to
**UDP multicast**, apply **TSN**/priorities, reduce payload size, send on-change.

**S3. A large 9 KB sensor payload must reach 4 ECUs efficiently. Approach?**
If multicast fan-out matters → **SOME/IP-TP over UDP multicast**. If strict
reliability per consumer matters more → TCP unicast per consumer (costlier). Weigh
loss tolerance vs bandwidth.

**S4. Two consumers need different subsets of a provider's data. Design?**
Split into multiple **eventgroups** so each consumer subscribes only to what it
needs — avoids over-subscription and wasted bandwidth.

---

## 19.6 Design questions

**D1. Design a vehicle communication system for an infotainment + ADAS domain.**
Key points to mention:
- **Ethernet backbone** with switches; VLAN separation (safety vs comfort).
- **SOME/IP services** for camera, navigation, media, climate; **CAN/FlexRay** for
  hard-real-time control, bridged via **gateway**.
- **SD** for runtime discovery; **eventgroups** for sensor streams.
- **UDP + multicast** for cyclic events; **TCP/TP** for large transfers/OTA.
- **TSN** + priorities for safety-critical streams; **E2E** for integrity.
- Define **service interfaces & versioning** strategy up front.

**D2. Design the service interface for a "Door Control" service.**
- **Methods:** `lockAll()`, `unlock(doorId)` (request/response, with return codes).
- **Events:** `DoorStateChanged` (eventgroup) — on-change.
- **Fields:** `ChildLockEnabled` (getter/setter + notifier).
- Keep it **consumer-agnostic**; version the interface; small flat payloads.

**D3. How would you scale to 50 ECUs and 200 services?**
Hierarchical/zonal Ethernet topology, multicast eventgroups to limit fan-out cost,
careful SD timing to avoid storms, VLAN segmentation, and gateways to legacy buses.
Emphasize **loose coupling** and stable interfaces.

---

## 19.7 Debugging questions

**DB1. Tools you'd use?**
**Wireshark** (with SOME/IP & SD dissectors), ECU/middleware logs, network switch
counters/IGMP tables, and E2E/sequence-counter diagnostics.

**DB2. Method calls work but events never arrive. First checks?**
This points to **pub/sub**, not RPC: verify **Subscribe/SubscribeAck**, eventgroup
IDs, **multicast group + IGMP join**, switch multicast snooping, and TTLs.

**DB3. Payload arrives but values are garbage. Likely cause?**
**Serialization mismatch** — interface version skew, wrong field order/size, or
endianness/alignment misconfig. Compare provider/consumer interface definitions.

**DB4. Intermittent message loss on UDP events under high load. Approach?**
Expected (best-effort). Add E2E sequence counters to detect, reduce rate/size,
prioritize via TSN, or move critical data to TCP. Check switch buffer drops.

---

## 19.8 Rapid-fire one-liners

| Question | Answer |
|----------|--------|
| Default transport? | UDP |
| Multicast for? | Events / SD |
| Byte order? | Big-endian |
| Discovery protocol? | SOME/IP-SD |
| Large UDP payloads? | SOME/IP-TP |
| Real-time determinism? | TSN |
| Reliability on UDP? | App-level E2E |
| Loose coupling enabler? | SD + interfaces |
| AUTOSAR Adaptive API? | ara::com |
| Field = ? | getter/setter + notifier |

---

## 19.9 Final preparation tips

- Be able to **draw** the request/response and subscribe/notify flows from memory.
- Always frame answers around **why** (the problem solved), not just **what**.
- Mention **tradeoffs** — interviewers reward balanced thinking.
- Tie concepts back to **real automotive scenarios** (ADAS, infotainment, OTA).
- Know where SOME/IP **doesn't** fit (hard-real-time control → CAN/FlexRay).

---

[← Previous: Chapter 18](18-common-mistakes.md) | [Back to TOC](../README.md) | [Next: Appendix A →](appendix-a-glossary.md)
