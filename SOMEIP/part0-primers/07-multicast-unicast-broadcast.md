# Primer P7: Unicast, Multicast & Broadcast

[← Previous: P6 — Ethernet](06-ethernet-automotive.md) | [Primers Index](README.md) | [Next: P8 — Serialization & Endianness →](08-serialization-endianness.md)

---

## P7.1 Definition

These are the three **delivery modes** describing *how many* receivers a packet
reaches:

| Mode | Receivers | Picture |
|------|-----------|---------|
| **Unicast** | Exactly one | One-to-one |
| **Multicast** | A specific group that subscribed | One-to-many (selective) |
| **Broadcast** | Everyone on the subnet | One-to-all |

```
   UNICAST            MULTICAST              BROADCAST
   A ──▶ B            A ──▶ {B, C}           A ──▶ {everyone}
       (C ignored)       (D not subscribed)     (B, C, D, ... all)
```

## P7.2 Why it is needed

Different communication shapes need different delivery:

- A **method call** (request/response) targets **one** server → unicast.
- An **event** (e.g., speed update) may interest **many** consumers → multicast.
- **Discovery** ("who offers service X?") may need to reach **many** potential
  providers → multicast (broadcast is avoided as wasteful).

## P7.3 Problem each solves

| Mode | Problem it solves | Cost |
|------|-------------------|------|
| Unicast | Precise one-to-one delivery | Doesn't scale for fan-out (N copies for N receivers) |
| Multicast | Efficient fan-out to interested parties only | Needs IGMP + switch configuration |
| Broadcast | Reach all without knowing who | Wastes bandwidth/CPU on uninterested nodes |

## P7.4 Why multicast is central to SOME/IP

SOME/IP **events** use the **publish–subscribe** pattern. A publisher sends **one**
datagram to a multicast group; the network duplicates it only toward subscribers.
Without multicast, the publisher would have to send **N separate unicast copies** —
wasting CPU and bandwidth and limiting scalability.

- **Service Discovery** uses multicast for offers/finds.
- **Events/Eventgroups** can be delivered via multicast to all subscribers.
- **Methods (request/response)** use unicast (one specific server).

## P7.5 How multicast is managed

- **IP multicast group addresses** identify a logical group (not a single host).
- **IGMP (IPv4) / MLD (IPv6)** let a node tell the switch *"I want this group."*
- The **switch** then forwards group traffic only to ports that joined — so
  uninterested ECUs aren't bothered.

## P7.6 Tradeoffs

- **Unicast:** simple and reliable-friendly (works with TCP), but N copies for N
  receivers — poor fan-out scaling.
- **Multicast:** excellent fan-out efficiency, but more configuration, harder to
  secure, and typically UDP-only (no TCP reliability).
- **Broadcast:** simplest reach-all, but noisy and unscalable — generally avoided.

## P7.7 Real-world analogy

- **Unicast** = a phone call to one person.
- **Multicast** = a podcast or newsletter — only subscribers receive each episode,
  and the publisher produces it **once**.
- **Broadcast** = a fire alarm in the building — *everyone* hears it whether they
  care or not.

---

[← Previous: P6 — Ethernet](06-ethernet-automotive.md) | [Primers Index](README.md) | [Next: P8 — Serialization & Endianness →](08-serialization-endianness.md)
