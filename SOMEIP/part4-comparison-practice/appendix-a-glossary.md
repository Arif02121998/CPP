# Appendix A: Glossary

[← Previous: Chapter 19](19-interview-prep.md) | [Back to TOC](../README.md) | [Next: Appendix B →](appendix-b-cheatsheet.md)

---

Terms are grouped by theme. **Bold** terms are core SOME/IP vocabulary.

## A.1 Core SOME/IP terms

| Term | Definition |
|------|------------|
| **SOME/IP** | Scalable service-Oriented MiddlewarE over IP — automotive service-oriented communication protocol. |
| **Service** | Logical unit (identified by Service ID) exposing methods, events, and fields. |
| **Service Instance** | A concrete running instance of a service (Instance ID); one service can have many. |
| **Provider / Server** | The ECU/app that offers a service. |
| **Consumer / Client** | The ECU/app that uses a service. |
| **Method** | A callable operation (RPC): request/response or fire-and-forget. |
| **Event** | A notification pushed to subscribers (pub/sub), cyclic or on-change. |
| **Field** | State with optional getter/setter (methods) and a notifier (event). |
| **Eventgroup** | A set of events subscribed to as a unit. |
| **Notification** | The message carrying an event's data to subscribers. |

## A.2 Service Discovery

| Term | Definition |
|------|------------|
| **SOME/IP-SD** | The Service Discovery sub-protocol. |
| **Offer** | SD message: "I provide this service instance at this endpoint." |
| **Find** | SD message: "Who provides this service?" |
| **Subscribe / SubscribeEventgroup** | Consumer request to receive an eventgroup. |
| **SubscribeAck / Nack** | Provider's acknowledgment / rejection of a subscription. |
| **TTL** | Time-To-Live: how long an offer/subscription remains valid. |
| **Entry / Option** | SD message building blocks (entries reference options like IP endpoints). |

## A.3 Message structure & serialization

| Term | Definition |
|------|------------|
| **Message ID** | Service ID + Method/Event ID — identifies what is being called. |
| **Request ID** | Client ID + Session ID — correlates requests with responses. |
| **Message Type** | Request, Response, Notification, Fire&Forget, Error, etc. |
| **Return Code** | Result status of a method (OK, not reachable, error…). |
| **Protocol / Interface Version** | Versioning fields for the protocol and the service interface. |
| **Serialization** | Converting structured data to a byte stream for transmission. |
| **Big-endian** | Most-significant-byte-first byte order (SOME/IP/network order). |
| **SOME/IP-TP** | Transport Protocol: segments large payloads over UDP. |

## A.4 Transport & network

| Term | Definition |
|------|------------|
| **UDP** | Connectionless, best-effort transport; SOME/IP default; supports multicast. |
| **TCP** | Connection-oriented, reliable, ordered transport; for large/critical data. |
| **IP** | Network-layer addressing and routing (IPv4/IPv6). |
| **Port** | 16-bit endpoint identifier on a host. |
| **Socket** | (IP, port, protocol) communication endpoint. |
| **Unicast** | One-to-one delivery. |
| **Multicast** | One-to-many delivery to a subscribed group. |
| **Broadcast** | One-to-all delivery on a subnet. |
| **IGMP / MLD** | Protocols for joining IP multicast groups (IPv4 / IPv6). |
| **MTU** | Maximum Transmission Unit — largest packet payload (~1500 B on Ethernet). |
| **Fragmentation** | Splitting oversized IP packets (avoided in embedded; use TP). |

## A.5 Ethernet & determinism

| Term | Definition |
|------|------------|
| **Ethernet** | Dominant link-layer technology; basis for automotive networking. |
| **Automotive Ethernet** | Variants (100/1000BASE-T1) for in-vehicle use over a single pair. |
| **MAC address** | 48-bit local hardware address of a network interface. |
| **VLAN (802.1Q)** | Logical network segmentation over shared infrastructure. |
| **TSN** | Time-Sensitive Networking (IEEE 802.1) — bounded-latency Ethernet. |
| **802.1AS** | TSN time synchronization. |
| **802.1Qbv** | TSN time-aware traffic scheduling. |
| **Frame preemption** | Letting urgent frames interrupt long ones (802.1Qbu/802.3br). |

## A.6 Architecture & design

| Term | Definition |
|------|------------|
| **SOA** | Service-Oriented Architecture — system built from discoverable services. |
| **Middleware** | Software layer between application and OS/network handling comms. |
| **AUTOSAR** | Automotive software architecture standard (Classic & Adaptive). |
| **ara::com** | AUTOSAR Adaptive communication API (binds to SOME/IP). |
| **ECU** | Electronic Control Unit — a vehicle computer/node. |
| **Gateway** | Bridges between networks/protocols (e.g., CAN ↔ SOME/IP). |
| **Loose coupling** | Components interact via interfaces/discovery, not fixed identities. |
| **E2E protection** | End-to-end integrity (CRC + sequence counters) per AUTOSAR. |
| **RPC** | Remote Procedure Call — invoking a remote operation like a local function. |
| **Pub/Sub** | Publish–Subscribe communication pattern. |

## A.7 Legacy automotive buses

| Term | Definition |
|------|------------|
| **CAN** | Controller Area Network — robust low-bandwidth signal bus (~1 Mbit/s). |
| **LIN** | Local Interconnect Network — very low-cost, low-speed sub-bus. |
| **FlexRay** | Deterministic, time-triggered high-reliability bus. |
| **Signal / PDU** | Fixed bit-packed data unit in classic automotive networks. |

---

[← Previous: Chapter 19](19-interview-prep.md) | [Back to TOC](../README.md) | [Next: Appendix B →](appendix-b-cheatsheet.md)
