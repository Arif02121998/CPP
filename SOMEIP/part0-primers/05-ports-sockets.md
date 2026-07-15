# Primer P5: Ports & Sockets

[← Previous: P4 — TCP](04-tcp.md) | [Primers Index](README.md) | [Next: P6 — Ethernet →](06-ethernet-automotive.md)

---

## P5.1 Definition

- A **port** is a 16-bit number (0–65535) that identifies a specific *endpoint*
  (a service or application) on a host. IP gets a packet to the right **machine**;
  the port gets it to the right **program** on that machine.
- A **socket** is the combination of **IP address + port + protocol**, e.g.
  `192.168.1.10 : 30509 / UDP`. It is the concrete handle an application opens to
  send and receive data.

```
   IP address  →  WHICH machine
   Port        →  WHICH program on that machine
   Protocol    →  UDP or TCP
   ───────────────────────────────────────
   Socket = (IP, Port, Protocol)  → a unique communication endpoint
```

## P5.2 Why it is needed

A single ECU may run many services at once — a diagnostics service, a camera
service, a climate service. They all share one IP address. **Ports** let the OS
deliver each incoming packet to the correct service instead of mixing them up.

## P5.3 Problem it solves

- **Multiplexing** — many independent conversations over one IP address.
- **Addressing services, not just hosts** — "send to the camera service" becomes
  "send to port 30490 on that ECU."
- **Separation** — request/response on one socket, events on another.

## P5.4 How this maps to SOME/IP

In SOME/IP, services are reached at specific **(IP, port, protocol)** endpoints.
**Service Discovery** exists precisely so clients don't have to hardcode these:
the SD message tells a client *"this service instance lives at IP X, port Y, over
UDP/TCP."* The client then opens a socket to that endpoint.

| Without SD | With SD |
|------------|---------|
| Hardcode IP:port in every client | Learn IP:port dynamically at runtime |
| Re-flash clients if endpoint changes | Endpoint can change; clients adapt |
| Tight coupling | Loose coupling |

## P5.5 Port number ranges

| Range | Name | Typical use |
|-------|------|-------------|
| 0–1023 | Well-known | Standard services (HTTP 80, etc.) |
| 1024–49151 | Registered | Assigned applications |
| 49152–65535 | Dynamic/ephemeral | Temporary client-side ports |

Automotive stacks typically use **statically configured** ports for determinism,
often in the registered range.

## P5.6 Tradeoffs

- **Static ports** → predictable, easy to firewall and debug; less flexible.
- **Dynamic ports** → flexible; harder to secure and trace.
- More sockets → more OS resources and configuration to manage.

## P5.7 Real-world analogy

Think of a large office building:

- The **street address** (IP) gets mail to the building.
- The **apartment/suite number** (port) gets it to the right office inside.
- A **socket** is an open mailbox at a specific suite, ready to send and receive.

You need both the street address *and* the suite number — an address alone isn't
enough to reach the right desk.

---

[← Previous: P4 — TCP](04-tcp.md) | [Primers Index](README.md) | [Next: P6 — Ethernet →](06-ethernet-automotive.md)
