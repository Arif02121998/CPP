# Chapter 6 — Naming, Addressing, and Lifetime

[← Back to Table of Contents](../README.md) | [← Previous](05-blocking-vs-nonblocking.md)

---

## 6.1 Three Questions Every IPC Channel Must Answer

1. **Naming** — How does a process *find/identify* the channel?
2. **Addressing** — How is a specific endpoint or peer specified?
3. **Lifetime** — When is the channel *created* and *destroyed*, and what happens to
   data and waiters when an endpoint goes away?

Get these wrong and you get leaks, "stale" channels, security holes, or peers that
can never find each other.

---

## 6.2 Naming Schemes

| Scheme | Example mechanisms | How peers rendezvous |
|--------|--------------------|----------------------|
| **Inheritance (unnamed)** | Anonymous pipe, `socketpair` | Created before `fork()`; child inherits fd. No external name. Only related processes. |
| **Filesystem path** | FIFO, Unix domain socket, mmap'd file | Both open the same path. Permissions = file permissions. |
| **Integer key/ID** | System V shm/sem/msg (`ftok`) | Agree on a key; kernel maps key→object. |
| **Network address** | TCP/UDP socket | `IP:port`; DNS for discovery. |
| **Abstract/registered name** | Linux abstract sockets, D-Bus, RPC registry | A name service maps names→endpoints. |

> **Unnamed = fast & private but only for related processes.**
> **Named = discoverable by unrelated processes but needs permission/cleanup care.**

---

## 6.3 Addressing: Pointing at the Right Endpoint

- **Pipes/FIFOs**: directional; you address by *which end* you hold (read fd vs write fd).
- **Sockets**: addressed by `(protocol, local addr, remote addr)`. A listening socket
  has an address; each accepted connection is a distinct endpoint.
- **Message queues**: addressed by queue name/ID; messages may carry a **type/priority**
  used for selective receive.
- **Signals**: addressed by **PID** (or process group); the "message" is just the
  signal number.
- **Shared memory**: addressed by attaching the segment; *within* it you address by
  offset/pointer.

---

## 6.4 Lifetime and Persistence

A critical axis: does the IPC object disappear with its creators, or persist?

| Persistence | Meaning | Examples |
|-------------|---------|----------|
| **Process-lifetime** | Dies when last fd closes / process exits | Anonymous pipe, socketpair, anonymous shared memory |
| **Kernel-persistent** | Lives until explicitly removed or reboot | System V shm/sem/msg, POSIX shm/mq (until `unlink`) |
| **Filesystem-persistent** | A real file persists on disk | FIFO node, mmap'd regular file, Unix socket file |

**Pitfall:** System V and POSIX named objects are **kernel-persistent**. If you forget
to `shm_unlink`/`ipcrm`, they **leak** and survive your program — a classic source of
"why is there stale shared memory?" bugs. (See `ipcs`/`ipcrm` for System V.)

---

## 6.5 Endpoint Death: What Happens?

You must define behavior when a peer disappears:

- **Pipe/FIFO**: if all readers close, a writer gets **`SIGPIPE`** / `EPIPE`. If all
  writers close, a reader sees **EOF** (`read` returns 0).
- **Socket**: peer close → reader sees EOF; writing to a closed connection →
  `EPIPE`/`ECONNRESET`. TCP also detects death via keepalives/timeouts.
- **Shared memory**: the segment persists even if one process crashes — but any
  **lock it held** may be left in a bad state (robust mutexes address this).
- **Message queue**: messages persist in the queue (kernel-persistent ones) even if
  the sender dies.

> **Design rule:** assume any peer can die at any instant. Decide how each channel
> surfaces that (EOF, error, signal, timeout) and handle it.

---

## 6.6 Reference Counting and Cleanup

The kernel reference-counts fds. A pipe's buffer is freed only when **all** read *and*
write fds are closed. Two consequences:

1. **Leaked fds keep channels alive** → "EOF never comes" bugs (a forgotten write-end
   open in some process prevents readers from seeing EOF).
2. **`FD_CLOEXEC`** matters: mark fds close-on-exec so a child running `exec()` doesn't
   accidentally keep a pipe end open.

---

## 6.7 Security Implications of Naming

- **Filesystem-named** channels inherit **file permissions** — set them deliberately
  (mode bits, ownership). A world-writable FIFO/socket is a vulnerability.
- **Abstract** sockets (Linux) have no filesystem permissions — guard with other means.
- **Network** endpoints are reachable by anyone who can route to them → require
  authentication/encryption (TLS) for anything sensitive.
- **System V keys** via `ftok` can collide; predictable keys can be a hijack risk.

---

## 6.8 Text Diagram — Naming Spectrum

```
 Private/fast ────────────────────────────────► Public/discoverable
 anon pipe   socketpair   FIFO/UDS   SysV key   TCP IP:port   name service
 (inherit)   (inherit)    (path)     (ftok)     (address)     (registry)

 Lifetime:  process ──────────────► kernel ──► filesystem ──► external
```

---

## 6.9 Key Takeaways

- Every channel needs **naming** (find it), **addressing** (target the right endpoint),
  and a defined **lifetime**.
- **Unnamed/inherited** channels are fast and private but only link *related*
  processes; **named** channels enable unrelated peers but require cleanup and
  permission care.
- **Kernel-persistent** objects (System V, POSIX named) **leak** if not unlinked.
- Always design **endpoint-death semantics**: EOF, `EPIPE`/`SIGPIPE`, reset, or timeout.
- Naming choice is also a **security** choice (file perms, abstract names, network auth).

---

[← Previous](05-blocking-vs-nonblocking.md) | [Back to ToC](../README.md) | [Next: Pipes →](07-pipes.md)
