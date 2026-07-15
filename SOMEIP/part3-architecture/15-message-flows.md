# Chapter 15: End-to-End Message Flows

[← Previous: Chapter 14](14-design-system-thinking.md) | [Back to TOC](../README.md) | [Next: Chapter 16 →](../part4-comparison-practice/16-comparison.md)

---

This chapter ties everything together with **step-by-step** message flows.

## 15.1 Service startup & discovery

```
Time │ Provider (Body ECU)            Consumer (Head Unit)
─────┼──────────────────────────────────────────────────────────
 t0  │ boot, service ready
 t1  │ [Initial Wait: random delay]
 t2  │ ── OfferService(DoorSvc, TTL=3s) ──▶ (multicast)
     │                                  receives offer, caches endpoint
 t3  │                                  ◀─ SubscribeEventgroup(DoorEvents)
 t4  │ ── SubscribeEventgroupAck ──────▶
 t5  │ [Main Phase] cyclic OfferService every CyclicOfferDelay
─────┴──────────────────────────────────────────────────────────
```

**Key points:** randomized initial delay prevents offer storms; TTL means the
consumer must keep seeing offers or it considers the service gone; subscription
is acknowledged before events flow.

## 15.2 Request / Response (method) flow

```
Consumer                                   Provider
  │  build SOME/IP REQUEST
  │   MsgID = (DoorSvc, LockAll)
  │   ReqID = (ClientID=0x0007, Session=0x0001)
  │   Type  = REQUEST (0x00)
  │ ───────────────────────────────────────▶  deserialize, execute LockAll()
  │                                            build RESPONSE
  │                                            Type = RESPONSE (0x80), RC = E_OK
  │ ◀───────────────────────────────────────  same ReqID echoes back
  │ match Session=0x0001 → resolve the call
```

- The **Request ID** (Client+Session) is echoed so the consumer correlates the
  reply, even with many in-flight calls.
- On error, Provider returns **Type=ERROR (0x81)** with a non-zero **Return
  Code** (e.g., `E_NOT_OK`).

## 15.3 Fire-and-forget flow

```
Consumer ── REQUEST_NO_RETURN (SetCabinTemp=22) ──▶ Provider
            (no response; Session may still increment for tracing)
```

Use when an acknowledgment isn't required and lost commands are tolerable or
handled by periodic re-send.

## 15.4 Event notification flow

```
Provider (state changes: door opens)
   │  build NOTIFICATION (0x02)
   │   MsgID = (DoorSvc, DoorStateChanged[event id, MSB set])
   │  for each subscriber  (or one multicast send)
   ▼
Subscriber(s) ◀── NOTIFICATION ──  update UI / logic
```

- Sent **only to subscribers** (unicast) or to a **multicast group** if the
  subscriber count crossed the threshold.
- A **field notifier** also sends an **initial value** right after
  SubscribeEventgroupAck.

## 15.5 Subscription renewal & teardown

```
... events flowing ...
Consumer ── SubscribeEventgroup(TTL renew) ──▶ Provider   (before TTL expires)
Provider ── Ack ─────────────────────────────▶ Consumer

# teardown
Consumer ── StopSubscribeEventgroup (TTL=0) ─▶ Provider   (clean unsubscribe)
# or provider shutdown:
Provider ── StopOffer (Offer TTL=0) ─────────▶ all        (consumers drop state)
```

## 15.6 Large payload over UDP (SOME/IP-TP)

```
Provider has 9 KB response
  ── TP seg #1 (offset 0,    more=1) ──▶
  ── TP seg #2 (offset 1392, more=1) ──▶
  ── ...                               ──▶
  ── TP seg #N (offset X,    more=0) ──▶  Consumer reassembles → full message
```

## 15.7 Full sequence (consumer-first boot)

```
HeadUnit boots before Body ECU:
1. HeadUnit ── FindService(DoorSvc) ─▶ (multicast)     "anyone have it?"
2. (Body ECU not ready → no answer; HeadUnit retries per SD timing)
3. Body ECU boots, becomes ready
4. Body ECU ── OfferService(DoorSvc) ─▶ (multicast)
5. HeadUnit ── SubscribeEventgroup ───▶ Body ECU
6. Body ECU ── SubscribeEventgroupAck + initial field value ─▶ HeadUnit
7. Body ECU ── NOTIFICATION (on each change) ─▶ HeadUnit
8. HeadUnit ── REQUEST LockAll ──▶ Body ECU ── RESPONSE ─▶ HeadUnit
```

This single diagram exercises **discovery, subscription, events, and
request/response** — the complete SOME/IP lifecycle.

---

[← Previous: Chapter 14](14-design-system-thinking.md) | [Back to TOC](../README.md) | [Next: Chapter 16 →](../part4-comparison-practice/16-comparison.md)
