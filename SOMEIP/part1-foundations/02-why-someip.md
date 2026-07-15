# Chapter 2: Why SOME/IP? The Limits of CAN, LIN & FlexRay

[← Previous: Chapter 1](01-introduction.md) | [Back to TOC](../README.md) | [Next: Chapter 3 →](03-shift-to-soa.md)

---

## 2.1 The classic automotive bus world

For decades, in-vehicle networks were built on **signal-oriented**,
**broadcast** bus systems:

| Bus | Speed | Typical use | Model |
|-----|-------|-------------|-------|
| **LIN** | 20 kbit/s | Mirrors, windows, sensors | Master/slave, signal |
| **CAN** | 0.5–1 Mbit/s | Powertrain, body, chassis | Broadcast, signal |
| **CAN-FD** | up to ~8 Mbit/s | Higher-bandwidth CAN | Broadcast, signal |
| **FlexRay** | 10 Mbit/s | X-by-wire, safety | TDMA, signal |
| **MOST** | 25–150 Mbit/s | Legacy infotainment | Ring, streaming |

These work by putting **fixed signals** into **fixed frames** at **fixed IDs**.
Every node statically knows: *"CAN ID 0x120, bits 0–7 = engine RPM."*

## 2.2 Why this breaks down

**Definition of the problem:** Signal-oriented buses are *statically wired
contracts*. They are excellent for small, fixed, periodic data — and terrible
for everything modern cars now need.

Concrete limitations:

1. **Bandwidth ceiling.** A camera produces tens of Mbit/s. CAN tops out around
   1 Mbit/s. The physics simply don't fit.
2. **Static configuration.** Adding a new signal means re-generating and
   re-flashing the communication matrix (the "K-Matrix") across many ECUs.
3. **No dynamic discovery.** Nodes can't appear/disappear at runtime. Everything
   must be known at design time.
4. **Tight coupling.** Sender and receiver share a byte-level layout. Change one
   and you must change the other.
5. **No request/response.** CAN is broadcast; there is no native
   "call a function and get an answer" mechanism.
6. **Poor scalability.** More features → exponential signal/matrix complexity.

## 2.3 Why Ethernet + SOME/IP

Automotive Ethernet provides the **bandwidth** (100 Mbit/s, 1 Gbit/s, now
10 Gbit/s + TSN for determinism). But raw Ethernet is just pipes — you still
need a **middleware** to give it meaning. That's SOME/IP.

| Pain (CAN world) | SOME/IP answer |
|------------------|----------------|
| Bandwidth ceiling | Runs on 100M/1G Ethernet |
| Static comm matrix | Runtime **Service Discovery** |
| Broadcast only | Request/Response **methods** |
| Send always-on signals | **Subscribe** only when needed |
| Byte-level coupling | **Serialization** of typed data |
| Fixed topology | Services can move between ECUs |

## 2.4 Tradeoffs (be honest)

SOME/IP is **not** universally better — it trades simplicity for flexibility:

- **More overhead:** IP/UDP/SOME/IP headers vs an 8-byte CAN frame.
- **More CPU/RAM:** serialization, SD state machines, socket handling.
- **Less inherent determinism:** Ethernet needs **TSN** to match FlexRay-class
  timing guarantees.
- **More complexity:** more moving parts to configure, test, and debug.

> **Rule of thumb:** keep ultra-cheap, hard-real-time, low-bandwidth signals on
> CAN/FlexRay; move high-bandwidth, dynamic, service-style comms to SOME/IP.

## 2.5 Real-world analogy

CAN is like a **factory PA system**: fixed loudspeakers, everyone hears every
announcement, you can't ask a question. SOME/IP is like giving everyone a
**phone with a directory**: you look up who offers what, call them directly, get
an answer, or subscribe to their updates.

---

[← Previous: Chapter 1](01-introduction.md) | [Back to TOC](../README.md) | [Next: Chapter 3 →](03-shift-to-soa.md)
