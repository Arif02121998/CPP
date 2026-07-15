# Chapter 7: Methods vs Events

[← Previous: Chapter 6](06-client-server.md) | [Back to TOC](../README.md) | [Next: Chapter 8 →](08-pubsub-eventgroups.md)

---

## 7.1 Definition

- A **Method** is a **client-initiated** operation. The client *pulls*: it sends
  a request, optionally receiving a response.
- An **Event** is a **server-initiated** notification. The server *pushes* to
  clients that have subscribed.

```
 METHOD (pull)                     EVENT (push)
 Client                            Server
   │  request  ─────▶ Server         │   (state changes)
   │  ◀───── response                │  notify ─────▶ Subscriber(s)
```

## 7.2 Method types

| Type | Request | Response | Message Type code |
|------|---------|----------|-------------------|
| **Request/Response** | yes | yes | `REQUEST` (0x00) → `RESPONSE` (0x80) |
| **Fire & Forget** | yes | no | `REQUEST_NO_RETURN` (0x01) |
| **Error reply** | — | yes (error) | `ERROR` (0x81) |

Request/Response is **synchronous-feeling** (the API often blocks or uses a
future), while fire-and-forget is **one-way** (commands where no ack is needed).

## 7.3 Events and fields

- **Event:** a pure notification (e.g., `SpeedChanged`). No stored value implied.
- **Field notifier:** like an event but backed by a **stored current value**, so
  a new subscriber gets an **initial value** immediately on subscription.

Events are delivered only to **subscribed** clients (see Chapter 8), making them
bandwidth-efficient compared to always-on CAN signals.

## 7.4 Why both are needed

| Need | Use |
|------|-----|
| "Give me the current odometer value now" | **Method** (getter) |
| "Tell me whenever the door opens" | **Event** |
| "Set the AC target temperature" | **Method** (setter / fire&forget) |
| "Keep me updated on battery SoC" | **Field** (initial + on-change) |

Pull (methods) suits **on-demand** queries and commands; push (events) suits
**state propagation** and **streaming**.

## 7.5 Problem it solves

Without events, every consumer would have to **poll** ("are we there yet?"),
wasting bandwidth and adding latency. Without methods, you could not perform
**directed queries/commands** with correlated answers. SOME/IP provides both so
each data-flow pattern uses the efficient mechanism.

## 7.6 Tradeoffs

- **Methods:** simple mental model, but round-trip latency and provider load per
  call; risk of blocking if used synchronously.
- **Events:** efficient for many consumers, but require **subscription
  management**, and over-frequent events can flood the network.

## 7.7 Real-world analogy

- **Method** = phoning a shop to ask "do you have size 9 in stock?" (you ask,
  they answer).
- **Event** = subscribing to the shop's "back in stock" alerts (they notify you
  when something changes).

---

[← Previous: Chapter 6](06-client-server.md) | [Back to TOC](../README.md) | [Next: Chapter 8 →](08-pubsub-eventgroups.md)
