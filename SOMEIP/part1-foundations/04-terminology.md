# Chapter 4: Key Terminology & Mental Models

[← Previous: Chapter 3](03-shift-to-soa.md) | [Back to TOC](../README.md) | [Next: Chapter 5 →](../part2-core-concepts/05-service-oriented-model.md)

---

## 4.1 The vocabulary you must own

| Term | Meaning |
|------|---------|
| **Service** | A versioned set of methods, events, and fields |
| **Service ID** | 16-bit identifier of a service type |
| **Instance ID** | 16-bit identifier of a specific running instance |
| **Method** | A callable operation (request/response or fire-and-forget) |
| **Method ID** | 16-bit identifier of a method within a service |
| **Event** | A provider-pushed notification |
| **Event ID** | 16-bit identifier (MSB set distinguishes events/notifications) |
| **Eventgroup** | A logical bundle of events you subscribe to as a unit |
| **Field** | Stateful value with getter/setter/notifier |
| **Provider (Server)** | The ECU/app offering a service |
| **Consumer (Client)** | The ECU/app using a service |
| **SOME/IP-SD** | The Service Discovery sub-protocol |
| **Offer/Find** | SD messages: "I provide X" / "Who provides X?" |
| **Subscribe/Ack** | SD messages to join/confirm an eventgroup |
| **Request ID** | Client ID + Session ID, used to match responses |
| **Serialization** | Converting typed data ↔ on-wire byte stream |

## 4.2 The three communication patterns (the heart of SOME/IP)

```
1) REQUEST / RESPONSE (Method)
   Client ──── request ───▶ Server
   Client ◀─── response ─── Server     "Call a function, get a result."

2) FIRE-AND-FORGET (Method, no return)
   Client ──── request ───▶ Server     "Tell, don't ask."

3) PUBLISH / SUBSCRIBE (Event / Field notifier)
   Client ─ subscribe ─▶ Server
   Client ◀─ event ───── Server (repeatedly, on change/cyclic)
```

## 4.3 Identifiers form an address

A SOME/IP endpoint is conceptually addressed by a tuple:

```
(Service ID, Instance ID, Method/Event ID, Interface Version)
        +  transport (IP : Port over UDP/TCP)
```

This is the "phone number + extension" that lets a message reach exactly the
right capability on the right instance.

## 4.4 Mental models to keep

- **SOME/IP ≈ "embedded gRPC + MQTT" tuned for cars.** RPC *and* pub/sub in one
  protocol.
- **SD is the DNS + DHCP of services.** It answers "who/where is this service?"
- **Serialization is the contract.** As long as the byte format matches, the two
  sides need not share code.

> ✅ **Checkpoint (Part I):** You can now explain *what* SOME/IP is, *why* it
> replaced/augmented CAN-style buses, and the SOA mindset behind it. Part II
> opens up *how* each mechanism actually works.

---

[← Previous: Chapter 3](03-shift-to-soa.md) | [Back to TOC](../README.md) | [Next: Chapter 5 →](../part2-core-concepts/05-service-oriented-model.md)
