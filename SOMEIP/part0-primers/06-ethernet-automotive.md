# Primer P6: Ethernet & Automotive Ethernet

[← Previous: P5 — Ports & Sockets](05-ports-sockets.md) | [Primers Index](README.md) | [Next: P7 — Unicast/Multicast/Broadcast →](07-multicast-unicast-broadcast.md)

---

## P6.1 Definition

**Ethernet** is the dominant **link-layer** technology for wired networks. It
defines how devices are addressed locally (**MAC addresses**), how data is framed,
and how bits move on the physical medium. **Automotive Ethernet** is a family of
variants (e.g., **100BASE-T1**, **1000BASE-T1**) designed for the cost, weight,
EMC, and reliability demands of vehicles — notably running over a **single twisted
pair**.

## P6.2 Why it is needed

Traditional automotive buses (CAN ~1 Mbit/s, LIN ~20 kbit/s) cannot carry camera
feeds, radar data, or software updates. Modern vehicles need **megabits to
gigabits** of bandwidth. Ethernet provides it while enabling **IP**, and therefore
**SOME/IP**, to run on top.

## P6.3 The Ethernet frame

```
   ┌──────────┬──────────┬──────┬─────────────────┬─────┐
   │ Dst MAC  │ Src MAC  │ Type │ Payload (IP pkt)│ FCS │
   │ 6 bytes  │ 6 bytes  │ 2 B  │ 46–1500 bytes   │ 4 B │
   └──────────┴──────────┴──────┴─────────────────┴─────┘
```

- **MAC address** — a 48-bit hardware address identifying a network interface
  *locally* (within one link). IP routes globally; MAC delivers on the local wire.
- **Payload** carries the IP packet (which carries UDP/TCP, which carries SOME/IP).
- **MTU** — the standard 1500-byte payload limit drives why large UDP messages
  need fragmentation or **SOME/IP-TP**.

## P6.4 Automotive Ethernet specifics

| Feature | Why it matters in a car |
|---------|-------------------------|
| **Single twisted pair (xBASE-T1)** | Less weight, lower cost than 4-pair |
| **Switched topology** | Each ECU on its own link; switch forwards selectively |
| **VLANs (802.1Q)** | Logically separate traffic domains for safety/security |
| **TSN (802.1)** | Adds time sync + bounded latency for real-time traffic |
| **PoDL** | Power over data line for simple peripherals |

## P6.5 Switches, not shared wire

Modern Ethernet is **switched**: each ECU connects point-to-point to a switch.
The switch forwards a frame only to the port(s) that need it (using MAC tables and
IGMP for multicast). This avoids collisions and lets multiple conversations happen
simultaneously — essential for scaling many ECUs.

## P6.6 Best-effort by default

Plain Ethernet makes **no timing guarantee** — a burst of traffic can delay a
critical frame (jitter). **TSN** adds scheduling, priority, and preemption so
high-priority SOME/IP traffic gets **bounded latency**, approaching FlexRay-like
determinism while keeping Ethernet's huge bandwidth advantage.

## P6.7 Tradeoffs

- ✅ High bandwidth, IP-capable, mature ecosystem, scalable via switches.
- ❌ Plain Ethernet is non-deterministic → needs TSN for hard real-time.
- ❌ More complex (switches, VLANs, IP config) than a simple CAN bus.

## P6.8 Real-world analogy

Ethernet is the **road network** of the vehicle's data world:

- **MAC addresses** are like license plates — meaningful on the local road.
- **The switch** is a smart intersection that sends each car only down the road it
  needs, instead of every car flooding every street.
- **TSN** adds traffic lights and express lanes so ambulances (critical data)
  always get through on time.

---

[← Previous: P5 — Ports & Sockets](05-ports-sockets.md) | [Primers Index](README.md) | [Next: P7 — Unicast/Multicast/Broadcast →](07-multicast-unicast-broadcast.md)
