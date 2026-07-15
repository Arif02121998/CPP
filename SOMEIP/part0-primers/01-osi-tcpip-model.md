# Primer P1: The OSI & TCP/IP Models

[Back to Primers Index](README.md) | [Back to Main TOC](../README.md) | [Next: P2 — IP & Addressing →](02-ip-addressing.md)

---

## P1.1 Definition

A **layered networking model** breaks communication into stacked layers, where
each layer has one job and talks only to the layers directly above and below it.
Two models dominate:

- **OSI model** — 7 layers, a conceptual teaching reference.
- **TCP/IP model** — 4 practical layers, what the real internet (and automotive
  Ethernet) actually uses.

| OSI Layer | TCP/IP Layer | Job | Examples |
|-----------|--------------|-----|----------|
| 7 Application | Application | App-level data & semantics | **SOME/IP**, HTTP, DNS |
| 6 Presentation | Application | Encoding, serialization | **SOME/IP serialization**, TLS |
| 5 Session | Application | Dialog/state management | (in SOME/IP: SD, sessions) |
| 4 Transport | Transport | End-to-end delivery | **TCP**, **UDP** |
| 3 Network | Internet | Addressing & routing | **IP**, ICMP |
| 2 Data Link | Link | Framing on a local link | **Ethernet**, MAC, VLAN |
| 1 Physical | Link | Bits on the wire | 100BASE-T1, copper |

## P1.2 Why it is needed

Without layering, every application would have to re-implement routing,
retransmission, addressing, and physical signaling. Layering lets each concern be
solved **once** and **reused**. SOME/IP can focus purely on *services* because IP
handles addressing and Ethernet handles the wire.

## P1.3 Problem it solves

- **Separation of concerns** — change Wi‑Fi to Ethernet without touching the app.
- **Interoperability** — vendors implement to a layer's interface, not the whole stack.
- **Reusability** — TCP/UDP serve thousands of application protocols.

## P1.4 Encapsulation: how data actually travels

Each layer **wraps** the layer above it with its own header (like nested envelopes):

```
[ Ethernet header [ IP header [ UDP header [ SOME/IP header + payload ] ] ] ]
   └ Link layer     └ Network    └ Transport   └ Application
```

On send, headers are **added** top-to-bottom. On receive, headers are **stripped**
bottom-to-top until your application sees just the SOME/IP payload.

## P1.5 Tradeoffs

- **Overhead** — every layer adds a header (bytes on the wire).
- **Abstraction cost** — a problem can hide in any layer, making debugging require
  cross-layer thinking (e.g., a "SOME/IP" bug that is really an IP multicast issue).
- **Benefit outweighs cost** — modularity is why the internet scales at all.

## P1.6 Real-world analogy

Sending a letter through a postal system:

- **Application** = the letter you wrote (the meaning).
- **Transport** = choosing registered vs regular mail (reliable vs best-effort).
- **Network** = the postal address & routing between cities.
- **Link/Physical** = the trucks, planes, and roads carrying the envelope.

You write the letter; you don't personally drive it. Each layer trusts the one
below to do its job.

---

[Back to Primers Index](README.md) | [Back to Main TOC](../README.md) | [Next: P2 — IP & Addressing →](02-ip-addressing.md)
