# Inter-Process Communication (IPC)

### A System-Level Study Guide for Operating Systems, Distributed Systems, and Technical Interviews

---

> **How to use this book**
> Read Parts I–II to build mental models, Part III for mechanism-by-mechanism depth, Parts IV–VI for design and comparison, and Parts VII–IX for real-world application, advanced topics, and interview preparation. Each mechanism chapter follows the same template so you can compare them directly.

This study guide is split into **separate files per concept**, organized into parts. Use the table of contents below to navigate.

---

## Table of Contents

### Part I — Foundations
1. [What Is IPC?](chapters/01-what-is-ipc.md)
2. [Why IPC Is Needed](chapters/02-why-ipc-is-needed.md)
3. [IPC, the OS, and the Process Model](chapters/03-ipc-os-process-model.md)

### Part II — Core Concepts You Need First
4. [The Kernel Boundary and Context Switching](chapters/04-kernel-boundary-context-switching.md)
5. [Blocking vs Non-Blocking Communication](chapters/05-blocking-vs-nonblocking.md)
6. [Naming, Addressing, and Lifetime](chapters/06-naming-addressing-lifetime.md)

### Part III — IPC Mechanisms (Deep Coverage)
7. [Pipes (Anonymous and Named)](chapters/07-pipes.md)
8. [Message Queues](chapters/08-message-queues.md)
9. [Shared Memory](chapters/09-shared-memory.md)
10. [Sockets (Unix Domain and TCP/IP)](chapters/10-sockets.md)
11. [Signals](chapters/11-signals.md)
12. [Remote Procedure Call (RPC)](chapters/12-rpc.md)
13. [Memory-Mapped Files](chapters/13-memory-mapped-files.md)

### Part IV — Internals
14. [Kernel Involvement and Data Paths](chapters/14-kernel-involvement-data-paths.md)
15. [Synchronization Primitives](chapters/15-synchronization-primitives.md)
16. [Deadlocks and Race Conditions](chapters/16-deadlocks-race-conditions.md)

### Part V — Design Considerations
17. [Data Consistency and Ordering](chapters/17-data-consistency-ordering.md)
18. [Throughput vs Latency](chapters/18-throughput-vs-latency.md)

### Part VI — Comparison
19. [Mechanism Comparison Tables](chapters/19-mechanism-comparison.md)
20. [Local IPC vs Distributed Communication](chapters/20-local-vs-distributed.md)

### Part VII — Real-World Scenarios
21. [Microservices Communication](chapters/21-microservices-communication.md)
22. [OS-Level Services](chapters/22-os-level-services.md)
23. [High-Performance Systems](chapters/23-high-performance-systems.md)

### Part VIII — Advanced Topics
24. [Zero-Copy Mechanisms](chapters/24-zero-copy.md)
25. [Lock-Free Communication](chapters/25-lock-free-communication.md)
26. [Event-Driven Systems](chapters/26-event-driven-systems.md)

### Part IX — Interview Preparation
27. [Interview Questions: Beginner → Advanced](chapters/27-interview-beginner-advanced.md)
28. [Scenario-Based Questions](chapters/28-interview-scenario-based.md)
29. [Debugging and Performance Questions](chapters/29-interview-debugging-performance.md)
30. [System Design Questions](chapters/30-interview-system-design.md)

### Part X — Pitfalls
31. [Common Mistakes and Anti-Patterns](chapters/31-common-mistakes.md)

### Appendices
- [A. Glossary](chapters/appendix-a-glossary.md)
- [B. Quick Reference Cheat Sheet](chapters/appendix-b-cheat-sheet.md)

---

## Chapter Template (for mechanism chapters)

Each mechanism chapter in Part III follows the same structure so you can compare them directly:

1. **Definition** — what it is in one paragraph
2. **How It Works Conceptually** — the mental model + a text diagram
3. **Why It Exists** — the problem it solves
4. **Performance Characteristics** — latency, throughput, copies, syscalls
5. **Tradeoffs** — speed vs complexity vs safety
6. **When to Use / When Not to Use**
7. **Common Pitfalls**
8. **Key Takeaways**

---

## Reading Paths

- **Interview crash course:** Ch 1 → 7–13 (skim) → 19 → 27–30 → 31
- **OS course companion:** Ch 1–6 → 7–16 → 17–18
- **Distributed systems focus:** Ch 1–3 → 10 → 12 → 20–21 → 26
- **Performance engineering:** Ch 4 → 9 → 13–15 → 18 → 23–25
