# Appendix A — Glossary

[← Back to Table of Contents](../README.md) | [← Previous](31-common-mistakes.md)

---

> Concise definitions of every key term used in this book. Cross-references point to the
> chapter with full coverage.

---

## A

**ABA problem** — In CAS-based lock-free code, a value changes A→B→A so a compare-and-swap
succeeds despite intermediate churn, risking corruption. Fixed with tagged pointers,
hazard pointers, or epoch/RCU reclamation. (Ch. 25)

**Address space** — The set of virtual addresses a process can use; private per process,
enforced by the MMU. (Ch. 2, 3)

**Async-signal-safe** — A function guaranteed safe to call inside a signal handler
(short POSIX-defined list). Most library functions are *not*. (Ch. 11)

**Asynchronous I/O** — A model where you submit an operation and are notified on
**completion** (e.g., `io_uring`, IOCP). (Ch. 5)

**At-least-once / at-most-once / exactly-once** — Message delivery semantics; at-least-once
(may duplicate) is common and requires **idempotent** consumers. (Ch. 17)

**Atomic operation** — An indivisible memory operation (e.g., CAS, fetch-add) used for
lock-free coordination. (Ch. 15, 25)

## B

**Backpressure** — Flow control where a slow consumer throttles a fast producer (e.g., a
full buffer blocks writes). (Ch. 5, 18)

**Barrier (memory)** — An instruction/ordering constraint preventing harmful reordering of
memory operations; see release/acquire. (Ch. 17, 25)

**Blocking call** — An operation that waits until it can complete. (Ch. 5)

**Busy-waiting / spinning** — Repeatedly checking a condition without sleeping; low latency
but burns CPU. (Ch. 5, 23)

## C

**C10K problem** — The challenge of handling 10,000+ concurrent connections, which drove
the move to event loops. (Ch. 26)

**Cache coherence / false sharing** — Hardware keeps per-core caches consistent; false
sharing is when two cores contend over the same cache line, hurting performance. (Ch. 9,
23)

**CAS (compare-and-swap)** — Atomic "if value == expected, set to new" — the core
lock-free primitive. (Ch. 25)

**Circuit breaker** — A pattern that stops calling a failing service to prevent cascading
failure. (Ch. 21)

**Condition variable** — A primitive to wait for a predicate while releasing a mutex,
woken by a signal. (Ch. 15)

**Context switch** — The kernel saving one process/thread's state and loading another's;
may flush the TLB. More expensive than a mode switch. (Ch. 4)

**Copy-on-write (COW)** — Pages shared read-only until a write triggers a private copy
(e.g., `MAP_PRIVATE`, `fork`). (Ch. 13)

## D

**Datagram** — A self-contained message preserving boundaries (UDP, UDS-dgram); may be
unreliable/unordered. (Ch. 10, 17)

**Deadlock** — Processes wait forever in a cycle; requires the four Coffman conditions.
(Ch. 16)

**DMA (Direct Memory Access)** — Hardware moving data to/from RAM without CPU copying each
byte. (Ch. 14, 24)

**DPDK** — A kernel-bypass framework for user-space packet processing. (Ch. 23)

## E

**Edge-triggered / level-triggered** — `epoll` modes: edge notifies on the arrival
transition (must drain fully); level notifies while data remains. (Ch. 26)

**EOF (end-of-file)** — Signaled when all write ends of a pipe/stream close; `read` returns
0. (Ch. 6, 7)

**epoll / kqueue** — Scalable readiness-notification APIs (Linux/BSD) returning only ready
fds — O(ready). (Ch. 26)

**Event loop** — A single-threaded loop that waits for ready events and dispatches
handlers (Reactor pattern). (Ch. 26)

**eventfd** — A file descriptor used as a lightweight event/counter, often for cross-thread
wakeups in an event loop. (Ch. 26)

## F

**Fallacies of distributed computing** — Eight false assumptions (network reliable, zero
latency, etc.). (Ch. 20)

**FIFO (named pipe)** — A pipe with a filesystem path, usable by unrelated processes.
(Ch. 7)

**File descriptor (fd)** — A process-local integer handle to a kernel object (file, pipe,
socket). (Ch. 3)

**Framing** — Imposing message boundaries on a byte stream (length prefix or delimiter).
(Ch. 17)

**fork() / exec()** — Process creation (duplicate) / program replacement; fds are inherited
across both. (Ch. 3)

**Futex** — "Fast userspace mutex"; the kernel primitive under most locks — no syscall when
uncontended. (Ch. 14, 15)

## I

**Idempotency** — An operation that, applied multiple times, has the same effect as once;
essential for safe retries. (Ch. 17, 20)

**io_uring** — A Linux completion-based async I/O interface with batched submission/
completion. (Ch. 5, 26)

**IPC (Inter-Process Communication)** — OS mechanisms letting isolated processes exchange
data and synchronize. (Ch. 1)

## K

**Kernel bypass** — Talking to hardware (NIC) from user space, skipping the kernel stack
(DPDK, RDMA). (Ch. 23)

**Kernel space / user space** — Privileged vs unprivileged execution domains separated by
the syscall boundary. (Ch. 3)

## L

**Latency** — Time for a single operation end to end. (Ch. 18)

**Little's Law** — $L = \lambda W$; relates concurrency, throughput, and latency. (Ch. 18)

**Livelock** — Processes keep reacting to each other but make no progress. (Ch. 16)

**Lock-free / wait-free** — Coordination without locks; lock-free = system always
progresses, wait-free = every thread progresses in bounded steps. (Ch. 25)

## M

**Marshalling / serialization** — Converting structured data to bytes for transmission.
(Ch. 12)

**Memory-mapped file** — A file mapped into a process's address space via `mmap`; enables
zero-copy access and file-backed shared memory. (Ch. 13)

**Message queue** — Kernel object delivering discrete, boundary-preserving messages, often
with priorities. (Ch. 8)

**Mode switch** — CPU transitioning between user and kernel mode on a syscall; cheaper than
a context switch. (Ch. 4)

**Mutex** — A mutual-exclusion lock with an owner. (Ch. 15)

## N

**NUMA** — Non-Uniform Memory Access; memory closer to a core is faster — relevant to
shared-memory placement. (Ch. 23)

**Non-blocking I/O** — Calls return immediately with `EAGAIN` if they can't proceed.
(Ch. 5)

## P

**Page cache** — Kernel RAM cache of file data; `read`/`write` and `mmap` both use it.
(Ch. 14)

**Partial failure** — A distributed call may fail in ways indistinguishable from "never
ran" (e.g., reply lost). (Ch. 20)

**Pipe** — A unidirectional, FIFO, byte-stream kernel buffer. (Ch. 7)

**Process** — A running program with a private address space and fd table. (Ch. 1, 3)

**Proactor** — Completion-based event pattern (IOCP/io_uring). (Ch. 26)

## R

**Race condition** — A timing-dependent bug from unsynchronized concurrent access. (Ch. 16)

**RCU (Read-Copy-Update)** — Lock-free reads with deferred reclamation; ideal for
read-mostly data. (Ch. 25)

**RDMA** — Remote Direct Memory Access; one host's NIC reads/writes another's memory
without the remote CPU. (Ch. 23, 24)

**Reactor** — Readiness-based event pattern (epoll/kqueue). (Ch. 26)

**Release/acquire** — Memory-ordering pair ensuring writes before a release are visible
after the matching acquire. (Ch. 17, 25)

**Robust mutex** — A process-shared mutex that allows recovery if the owner dies. (Ch. 15)

**RPC (Remote Procedure Call)** — Calling a function in another process/host as if local,
via stubs and an IDL. (Ch. 12)

**Ring buffer** — A circular buffer; the basis of fast SPSC/MPMC queues. (Ch. 23, 25)

## S

**Saga** — A distributed transaction as a series of local steps with compensating actions.
(Ch. 21)

**Scatter-gather (vectored) I/O** — `readv`/`writev`/`sendmsg` over multiple buffers in one
call. (Ch. 24)

**Semaphore** — An ownerless counter with `wait`/`post` for resource counting/signaling.
(Ch. 15)

**sendfile / splice** — Syscalls moving data between fds inside the kernel (zero-copy).
(Ch. 24)

**Service mesh** — Infrastructure (sidecars) handling discovery, retries, TLS, tracing for
microservices. (Ch. 21)

**Shared memory** — The same physical pages mapped into multiple processes; fastest IPC,
no built-in sync. (Ch. 9)

**Signal** — An asynchronous, near-dataless process notification (software interrupt).
(Ch. 11)

**SIGPIPE** — Signal raised when writing to a pipe/socket with no reader. (Ch. 7, 10)

**Socket** — A bidirectional communication endpoint; same API for local (UDS) and network
(TCP/UDP). (Ch. 10)

**SPSC / MPSC / MPMC** — Single/Multiple Producer, Single/Multiple Consumer queue
configurations. (Ch. 25)

**Spinlock** — A lock that busy-waits instead of sleeping. (Ch. 15)

**Starvation** — A process is perpetually denied a resource. (Ch. 16)

**System call (syscall)** — A controlled entry into the kernel to request a service.
(Ch. 4)

## T

**Tail latency (p99/p99.9)** — High-percentile latency; what users actually feel under
load. (Ch. 18)

**TCP_NODELAY** — Disables Nagle's algorithm to reduce small-message latency. (Ch. 10, 18)

**Throughput** — Work completed per unit time. (Ch. 18)

**TLB (Translation Lookaside Buffer)** — A cache of virtual→physical translations; flushed
on some context switches. (Ch. 4)

**TOCTOU** — Time-of-check-to-time-of-use race, often security-sensitive. (Ch. 16)

**Transactional outbox** — Writing an event in the same DB transaction as the state change,
then relaying it, to avoid dual-write inconsistency. (Ch. 21)

## U

**Unix domain socket (UDS)** — A local socket skipping the IP stack; supports fd passing
and peer credentials. (Ch. 10)

## Z

**Zero-copy** — Moving data without CPU copies through user space (sendfile, splice,
MSG_ZEROCOPY, mmap, shared memory, RDMA). (Ch. 24)

---

[← Previous](31-common-mistakes.md) | [Back to ToC](../README.md) | [Next: Appendix B — Cheat Sheet →](appendix-b-cheat-sheet.md)
