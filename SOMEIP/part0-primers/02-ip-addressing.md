# Primer P2: IP & Addressing (IPv4 / IPv6)

[← Previous: P1 — OSI & TCP/IP](01-osi-tcpip-model.md) | [Primers Index](README.md) | [Next: P3 — UDP →](03-udp.md)

---

## P2.1 Definition

**IP (Internet Protocol)** is the network-layer protocol that gives every device
an **address** and routes packets from a source address to a destination address
across one or more networks. It is the "where" of communication.

- **IPv4** — 32-bit addresses, written as `192.168.1.10` (~4.3 billion addresses).
- **IPv6** — 128-bit addresses, written as `2001:db8::1` (effectively unlimited).

## P2.2 Why it is needed

In an automotive Ethernet network, dozens of ECUs share the same physical medium.
IP gives each ECU a unique identity so a packet from the camera ECU can reach the
display ECU specifically — not everyone at once.

## P2.3 Problem it solves

- **Identity** — who is who on the network.
- **Routing** — how a packet reaches the right node, possibly across switches/subnets.
- **Independence from hardware** — IP addresses are logical, so software doesn't
  hardcode MAC addresses or physical ports.

## P2.4 Key concepts you'll meet in SOME/IP

| Concept | Meaning | SOME/IP relevance |
|---------|---------|-------------------|
| **Address** | Logical ID of a node | Each ECU/service endpoint has one |
| **Subnet / netmask** | Group of addresses on one logical network | Defines who is "local" |
| **Unicast** | One-to-one address | Methods / request-response |
| **Multicast** | One-to-many group address | Events, Service Discovery |
| **Broadcast** | One-to-all on a subnet | Rarely used; multicast preferred |
| **Static vs dynamic** | Fixed vs assigned address | Automotive often uses **static** for determinism |
| **MTU** | Max bytes per packet (~1500 on Ethernet) | Drives UDP fragmentation / SOME/IP-TP |
| **Fragmentation** | Splitting oversized packets | Undesirable in embedded; avoided via TP |

## P2.5 IP does NOT guarantee delivery

IP is **best-effort**: it tries to deliver packets but makes **no promise** about
arrival, order, or duplication. Those guarantees (if needed) come from the layer
above — **TCP** (reliable) or your application logic on top of **UDP**.

```
   IP packet
   ┌─────────────┬──────────────┬───────────────────────────┐
   │ Src IP      │ Dst IP       │ Payload (TCP or UDP segment)│
   └─────────────┴──────────────┴───────────────────────────┘
```

## P2.6 Tradeoffs

- **IPv4** is simpler and ubiquitous but address-limited.
- **IPv6** is future-proof and great for multicast, but addresses are longer and
  add header overhead.
- **Static addressing** gives determinism and easy debugging but less flexibility.

## P2.7 Real-world analogy

IP addresses are **postal addresses**. The postal service (IP) will *try* to
deliver your envelope to `12 Oak Street`, but on its own it won't tell you whether
it arrived. If you want a delivery confirmation, you pay extra for registered mail
(**TCP**) or you build your own follow-up system (app logic over **UDP**).

---

[← Previous: P1 — OSI & TCP/IP](01-osi-tcpip-model.md) | [Primers Index](README.md) | [Next: P3 — UDP →](03-udp.md)
