# Primer P8: Serialization, Bytes & Endianness

[← Previous: P7 — Unicast/Multicast/Broadcast](07-multicast-unicast-broadcast.md) | [Primers Index](README.md) | [Next: P9 — Communication Patterns →](09-comm-patterns.md)

---

## P8.1 Definition

- **Serialization** is converting an in-memory data structure (a struct, object,
  array) into a flat sequence of **bytes** suitable for transmission. The reverse,
  rebuilding the structure on the receiver, is **deserialization**.
- **Endianness** is the byte order used to store a multi-byte number:
  - **Big-endian** — most significant byte first (network/SOME/IP default).
  - **Little-endian** — least significant byte first (common on x86 CPUs).

```
   The 32-bit number 0x12345678 in memory:

   Big-endian:     12 34 56 78   ← SOME/IP / network order
   Little-endian:  78 56 34 12   ← typical x86
```

## P8.2 Why it is needed

Memory layout differs between CPUs, compilers, and languages. You cannot just copy
raw memory from ECU A to ECU B and expect it to mean the same thing. Serialization
defines a **shared, agreed wire format** so both sides interpret the bytes
identically.

## P8.3 Problem it solves

- **Interoperability** — a sender on an ARM ECU and a receiver on an x86 ECU agree
  on exactly which byte means what.
- **Portability** — independent of language, compiler padding, or CPU architecture.
- **Determinism** — a fixed layout means fixed, predictable message sizes.

## P8.4 Why endianness matters concretely

If the sender writes `0x12345678` little-endian and the receiver reads it
big-endian, it becomes `0x78563412` — a completely different value. Choosing **one**
byte order (SOME/IP uses **big-endian / network byte order**) eliminates this
class of bug.

## P8.5 SOME/IP serialization characteristics

| Property | SOME/IP approach | Benefit |
|----------|------------------|---------|
| Byte order | Big-endian (network order) | Cross-platform agreement |
| Layout | Mostly fixed/positional | Low overhead, fast parsing |
| Schema | Defined by service interface | No field tags needed inline |
| Strings/arrays | Length-prefixed/configurable | Bounded, predictable parsing |
| Alignment/padding | Configurable | Control over size vs speed |

Because the layout is largely **positional** (defined by the interface, not tagged
per field like JSON), SOME/IP messages are **compact** and **fast to parse** —
important for embedded ECUs and high-frequency events.

## P8.6 Compared to other formats

| Format | Self-describing? | Size | Parse speed | Notes |
|--------|------------------|------|-------------|-------|
| **SOME/IP** | No (schema-driven) | Small | Fast | Tuned for embedded/real-time |
| JSON | Yes (field names) | Large | Slow | Human-readable, web-friendly |
| Protobuf | Partly (field tags) | Small | Fast | Schema + tags, evolvable |
| Raw struct copy | No | Smallest | Fastest | Breaks across CPUs — unsafe |

## P8.7 Tradeoffs

- ✅ Compact, fast, deterministic.
- ❌ Not self-describing — sender and receiver **must** share the same interface
  definition. A version mismatch silently misinterprets bytes.
- ❌ Schema evolution must be handled carefully (field order/size matters).

## P8.8 Real-world analogy

Serialization is **flat-packing furniture for shipping**:

- You disassemble a 3D chair (in-memory object) into flat numbered parts (bytes).
- The receiver reassembles it using the **same instruction manual** (the interface
  schema).
- **Endianness** is agreeing whether the manual is read left-to-right or
  right-to-left — if you disagree, the chair comes out wrong even with all the
  right parts.

---

[← Previous: P7 — Unicast/Multicast/Broadcast](07-multicast-unicast-broadcast.md) | [Primers Index](README.md) | [Next: P9 — Communication Patterns →](09-comm-patterns.md)
