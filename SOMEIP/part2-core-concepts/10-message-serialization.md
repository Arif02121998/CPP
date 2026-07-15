# Chapter 10: Message Structure & Serialization

[← Previous: Chapter 9](09-service-discovery.md) | [Back to TOC](../README.md) | [Next: Chapter 11 →](../part3-architecture/11-protocol-stack.md)

---

## 10.1 Definition

**Serialization** is converting in-memory typed data (structs, arrays, strings)
into the **on-wire byte stream** and back. SOME/IP defines both the **header
format** (routing/identity) and **payload serialization rules** (how data types
are laid out in bytes).

## 10.2 The SOME/IP message header

Every SOME/IP message begins with a fixed 16-byte header, followed by the
payload:

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
┌───────────────────────────────┬───────────────────────────────┐
│          Service ID           │           Method ID           │  Message ID
├───────────────────────────────┴───────────────────────────────┤
│                            Length                              │  (covers from Request ID onward)
├───────────────────────────────┬───────────────────────────────┤
│           Client ID           │           Session ID          │  Request ID
├───────────────┬───────────────┬───────────────┬───────────────┤
│ Protocol Ver  │ Interface Ver │  Message Type │  Return Code  │
├───────────────┴───────────────┴───────────────┴───────────────┤
│                          Payload  ...                          │
└────────────────────────────────────────────────────────────────┘
```

| Field | Size | Purpose |
|-------|------|---------|
| **Message ID** | 32b | `Service ID (16) + Method/Event ID (16)` — *what* is being addressed |
| **Length** | 32b | Bytes from Request ID to end of payload |
| **Request ID** | 32b | `Client ID (16) + Session ID (16)` — correlate request/response |
| **Protocol Version** | 8b | SOME/IP protocol version (e.g., 0x01) |
| **Interface Version** | 8b | Service interface (major) version |
| **Message Type** | 8b | REQUEST / REQUEST_NO_RETURN / NOTIFICATION / RESPONSE / ERROR |
| **Return Code** | 8b | E_OK or error code (in responses) |

### Message Type values

| Value | Name | Meaning |
|-------|------|---------|
| 0x00 | REQUEST | request expecting a response |
| 0x01 | REQUEST_NO_RETURN | fire-and-forget |
| 0x02 | NOTIFICATION | event/field notification |
| 0x80 | RESPONSE | response to a request |
| 0x81 | ERROR | error response |

### Return Code examples

| Code | Meaning |
|------|---------|
| 0x00 | E_OK |
| 0x01 | E_NOT_OK (generic error) |
| 0x02 | E_UNKNOWN_SERVICE |
| 0x03 | E_UNKNOWN_METHOD |
| 0x04 | E_NOT_READY |
| 0x06 | E_WRONG_INTERFACE_VERSION |
| 0x07 | E_MALFORMED_MESSAGE |

## 10.3 Payload serialization rules

- **Endianness:** big-endian (network byte order) by default.
- **Basic types:** `uint8/16/32/64`, `sint*`, `float32/64`, `bool` laid out in
  fixed sizes.
- **Structs:** members serialized in declaration order (optionally with
  alignment/padding).
- **Strings:** with length field and encoding (UTF-8/UTF-16), optional BOM.
- **Arrays:** fixed-length (just elements) or dynamic-length (with a length
  prefix).
- **Length fields:** dynamic members are prefixed by a configurable-size length.
- **Union/variant:** a type selector followed by the selected member.
- **TLV (optional, SOME/IP "tag-length-value"):** wire fields carry **Data IDs**
  so members can be optional/reordered — key for **forward/backward
  compatibility**.

## 10.4 Why it is needed

The two communicating sides may be **different CPUs, compilers, languages, and
byte orders**. A precise, shared serialization contract guarantees that a struct
written on one ECU is read identically on another — *without sharing code*.

## 10.5 Problem it solves

It replaces fragile, manual byte-packing (the CAN "bits 0–7 = RPM" world) with a
**typed, tool-generated** contract, while still being compact and deterministic
enough for embedded real-time use.

## 10.6 Tradeoffs

- **Plain (non-TLV) serialization:** compact and fast, but **rigid** — both sides
  must agree exactly; reordering/adding fields breaks compatibility.
- **TLV serialization:** flexible and evolvable, but **larger and slower** (tags +
  lengths add overhead).
- **General cost:** serialization/deserialization consumes CPU and adds latency
  vs raw signal copy; header overhead is large relative to tiny payloads.

## 10.7 Real-world analogy

Serialization is **flat-pack furniture**. The manufacturer (sender) disassembles
a typed object into a standard box of parts with an instruction sheet (the
format). The buyer (receiver), regardless of country or tools, reassembles the
exact same object by following the same instructions — no need to share the
original factory.

---

[← Previous: Chapter 9](09-service-discovery.md) | [Back to TOC](../README.md) | [Next: Chapter 11 →](../part3-architecture/11-protocol-stack.md)
