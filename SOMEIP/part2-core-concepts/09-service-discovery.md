# Chapter 9: Service Discovery (SOME/IP-SD)

[← Previous: Chapter 8](08-pubsub-eventgroups.md) | [Back to TOC](../README.md) | [Next: Chapter 10 →](10-message-serialization.md)

---

## 9.1 Definition

**SOME/IP-SD** is the control-plane sub-protocol that lets ECUs **announce**,
**find**, and **subscribe to** service instances at runtime — without any
static, system-wide address configuration. It is itself a SOME/IP message
(carried over **UDP**, usually to a configured **multicast** address/port).

> **Mental model:** SD = **DNS + DHCP + subscription manager** for in-vehicle
> services.

## 9.2 Why it is needed

- ECUs boot in **different orders** and at **different times**; addresses
  (IP/port) may be assigned dynamically.
- A service may run on **different ECUs** across variants — consumers must locate
  it without hardcoding.
- Pub/sub needs a place to **register subscriptions**.

## 9.3 Problem it solves

It removes the **static communication matrix**. Instead of flashing every ECU
with "service X lives at IP/port Y," providers advertise and consumers discover
**dynamically at runtime**, enabling relocatable, hot-pluggable services.

## 9.4 The core SD message entries

SD messages contain **Entries** (the action) and **Options** (the details, e.g.,
IP endpoints):

| Entry | Sent by | Meaning |
|-------|---------|---------|
| **FindService** | Consumer | "Who provides Service X (instance/version)?" |
| **OfferService** | Provider | "I provide Service X at IP:Port (TTL=...)" |
| **StopOffer** (Offer, TTL=0) | Provider | "Service X is going away" |
| **SubscribeEventgroup** | Consumer | "Add me to eventgroup G" |
| **SubscribeEventgroupAck** | Provider | "You're subscribed" |
| **SubscribeEventgroupNack** | Provider | "Subscription rejected" |

Each entry carries a **TTL**; an offer/subscription is only valid while its TTL
is alive (it must be refreshed), giving automatic cleanup of dead entities.

## 9.5 Offer phases (state machine)

```
       power on
          │
     ┌────▼─────┐  no consumers / link down
     │  DOWN    │◀──────────────────────────┐
     └────┬─────┘                            │
          │ service ready                    │
     ┌────▼─────────┐  send Offers fast      │
     │ INITIAL WAIT │  (random delay)        │
     └────┬─────────┘                        │
          │                                  │
     ┌────▼─────────┐  burst of offers       │
     │ REPETITION   │  with increasing gap   │
     └────┬─────────┘                        │
          │                                  │
     ┌────▼─────────┐  steady cyclic offers  │
     │ MAIN PHASE   │  every CyclicOfferDelay │
     └──────────────┘────────────────────────┘
```

- **Initial Wait Phase:** random delay (avoids all ECUs offering at once).
- **Repetition Phase:** several offers with exponentially increasing intervals —
  helps late/early consumers find the service quickly.
- **Main Phase:** periodic cyclic offers at a steady interval.

## 9.6 Discovery workflows

**Provider-driven (typical):**
```
Provider ── OfferService (multicast) ─────▶ all
Consumer (was waiting) ── SubscribeEventgroup ─▶ Provider
Provider ── SubscribeEventgroupAck ──────────▶ Consumer
```

**Consumer-driven (if it boots first / impatient):**
```
Consumer ── FindService (multicast) ──▶ all
Provider ── OfferService (uni/multicast) ─▶ Consumer
Consumer ── SubscribeEventgroup ─────────▶ Provider
Provider ── SubscribeEventgroupAck ──────▶ Consumer
```

## 9.7 Tradeoffs

- **Pros:** zero static address config, runtime relocation, automatic cleanup via
  TTL, scalable startup via randomized/repetition phases.
- **Cons:** added control-plane traffic, SD state machine complexity, careful
  **timing parameter tuning** (TTL, delays) required; multicast needs network
  support; misconfiguration causes hard-to-debug "service not found" issues.

## 9.8 Real-world analogy

A **conference with a notice board**. Speakers (providers) pin "I'm giving talk X
in room 4, valid until 3 PM" (Offer + TTL). Attendees (consumers) read the board
or shout "who's talking about X?" (Find). To get slides pushed to you, you sign
the **sign-up sheet** (Subscribe) — which expires unless you re-sign (TTL).

---

[← Previous: Chapter 8](08-pubsub-eventgroups.md) | [Back to TOC](../README.md) | [Next: Chapter 10 →](10-message-serialization.md)
