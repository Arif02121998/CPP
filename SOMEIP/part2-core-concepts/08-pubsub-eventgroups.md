# Chapter 8: Publish–Subscribe & Eventgroups

[← Previous: Chapter 7](07-methods-vs-events.md) | [Back to TOC](../README.md) | [Next: Chapter 9 →](09-service-discovery.md)

---

## 8.1 Definition

**Publish/Subscribe** is the push pattern where a provider *publishes* events and
consumers *subscribe* to receive them. In SOME/IP, you do **not** subscribe to
individual events — you subscribe to an **Eventgroup**, a named bundle of one or
more events/field-notifiers.

```
        Eventgroup "DriveData" (ID 0x01)
        ├─ Event  Speed
        ├─ Event  Rpm
        └─ Field  Gear (notifier)

  Consumer ── Subscribe(DriveData) ─▶ Provider
  Consumer ◀── SubscribeAck ──────── Provider
  Consumer ◀── Speed/Rpm/Gear events ── Provider (while subscribed)
```

## 8.2 Why eventgroups (not individual events)?

- **Fewer SD messages:** subscribe once to a logical group instead of N times.
- **Logical cohesion:** related signals that consumers always want together.
- **Efficient transport selection:** a group can be mapped to multicast when
  many consumers want the same data.

## 8.3 Why it is needed

Pub/sub decouples producers from consumers in **number and identity**: the
provider doesn't hardcode who receives data; consumers come and go at runtime.
This is essential for dynamic, software-defined vehicles where features (and
their data needs) are added or updated over the air.

## 8.4 Problem it solves

- Eliminates **wasteful broadcast**: data is sent only to those who asked.
- Eliminates **polling**: consumers are notified on change.
- Supports **1-to-many** efficiently via **multicast** when subscriber count is
  high.

## 8.5 Unicast vs Multicast

| | Unicast (UDP/TCP) | Multicast (UDP) |
|---|---|---|
| Recipients | one per subscriber | one stream, many listeners |
| Provider cost | grows with subscribers | constant |
| When | few subscribers | many subscribers (threshold configurable) |

SOME/IP-SD lets a provider switch an eventgroup to **multicast** once the number
of subscribers crosses a configured threshold.

## 8.6 Subscription lifecycle

```
1. Provider OFFERs service (carries eventgroup info)
2. Consumer sends SubscribeEventgroup (SD)
3. Provider replies SubscribeEventgroupAck (or Nack)
4. Provider streams events until:
     • TTL expires (must renew), or
     • Consumer sends StopSubscribe, or
     • Service StopOffer / goes down
```

Subscriptions carry a **TTL**; consumers must renew, so dead subscribers are
automatically cleaned up.

## 8.7 Tradeoffs

- **Pros:** scalable 1-to-many, bandwidth-efficient, dynamic membership.
- **Cons:** subscription state to manage, TTL tuning, multicast needs network
  (IGMP/switch) support, harder to trace than static signals.

## 8.8 Real-world analogy

A **podcast**. You subscribe to a show (eventgroup), not to each individual
episode (event). New episodes are pushed to all subscribers (multicast-like).
Stop subscribing and you stop receiving — the publisher doesn't track you
forever.

---

[← Previous: Chapter 7](07-methods-vs-events.md) | [Back to TOC](../README.md) | [Next: Chapter 9 →](09-service-discovery.md)
