# Chapter 11: The SOME/IP Protocol Stack

[← Previous: Chapter 10](../part2-core-concepts/10-message-serialization.md) | [Back to TOC](../README.md) | [Next: Chapter 12 →](12-transport-tcp-udp-ip.md)

---

## 11.1 The layered view

```
┌──────────────────────────────────────────────────────────┐
│  Application / Service logic                              │  business logic
│  (AUTOSAR Adaptive: ara::com  | Classic: RTE + SWCs)     │
├──────────────────────────────────────────────────────────┤
│  Binding / Middleware API                                 │  proxies & skeletons
│   • Proxy (client side)   • Skeleton (server side)        │
├──────────────────────────────────────────────────────────┤
│  SOME/IP layer                                            │  THIS BOOK
│   • Serialization        • Message routing                │
│   • SOME/IP-SD (discovery & subscription)                 │
├──────────────────────────────────────────────────────────┤
│  Transport:  UDP            |            TCP              │  sockets
├──────────────────────────────────────────────────────────┤
│  IP (IPv4 / IPv6)  — addressing, routing, multicast (IGMP)│
├──────────────────────────────────────────────────────────┤
│  Ethernet (100BASE-T1 / 1000BASE-T1) + optional TSN       │  PHY/MAC
└──────────────────────────────────────────────────────────┘
```

## 11.2 Responsibilities per layer

| Layer | Responsibility |
|-------|----------------|
| **Application** | Calls methods, handles events, implements service logic |
| **Proxy/Skeleton** | Language-level stub: turns calls ↔ SOME/IP messages |
| **SOME/IP** | Header build/parse, (de)serialization, message dispatch |
| **SOME/IP-SD** | Offer/Find/Subscribe state machines, endpoint resolution |
| **UDP/TCP** | Datagram or stream transport, ports |
| **IP** | Addressing, routing, multicast group management |
| **Ethernet/TSN** | Framing, physical transmission, time-sensitive queuing |

## 11.3 Proxy & Skeleton pattern

```
 Client App                                   Server App
    │  proxy.getSpeed()                            ▲  skeleton dispatch
    ▼                                              │
 ┌───────┐  serialize+header   ─────────▶  ┌──────────┐
 │ Proxy │ ============ SOME/IP =========  │ Skeleton │
 └───────┘  ◀───────── deserialize        └──────────┘
```

- **Proxy (client side):** a local stand-in for the remote service. Method calls
  on it are serialized into SOME/IP requests.
- **Skeleton (server side):** receives requests, deserializes, invokes the real
  implementation, and serializes the response.

This is the classic **stub/skeleton** RPC pattern (like CORBA/gRPC), generated
from the service interface description (e.g., ARXML/Franca IDL).

## 11.4 Where SD fits

SOME/IP-SD runs **beside** the data plane: it uses the same SOME/IP message
format (with a reserved Service ID `0xFFFF`, Method ID `0x8100`) but only handles
**control** (who/where/subscribe). Once SD resolves endpoints, the **data plane**
(method calls, events) flows directly between the resolved IP:port pairs.

## 11.5 Why this layering matters

- **Separation of concerns:** transport can change (UDP↔TCP) without touching
  application logic.
- **Portability:** the same service code runs over different deployments.
- **Tooling:** each layer is independently testable and analyzable.

## 11.6 Real-world analogy

A **postal system**: the application writes a letter (data), the proxy puts it in
a standard envelope with an address (serialization + header), SD is the **address
book/post office directory**, and IP/Ethernet are the **trucks and roads**. Each
layer does one job and trusts the others.

---

[← Previous: Chapter 10](../part2-core-concepts/10-message-serialization.md) | [Back to TOC](../README.md) | [Next: Chapter 12 →](12-transport-tcp-udp-ip.md)
