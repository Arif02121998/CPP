# Chapter 7 — Pipes (Anonymous and Named)

[← Back to Table of Contents](../README.md) | [← Previous](06-naming-addressing-lifetime.md)

---

## 7.1 Definition

A **pipe** is a unidirectional, byte-stream IPC channel backed by an in-kernel
ring buffer. One end is written to, the other is read from, in **FIFO** order.

- **Anonymous pipe** (`pipe()`): unnamed; shared via fd inheritance across `fork()`.
  Used for **related** processes (parent↔child).
- **Named pipe / FIFO** (`mkfifo`): has a filesystem path, so **unrelated** processes
  can open it by name.

---

## 7.2 How It Works Conceptually

The kernel allocates a fixed-size circular buffer (commonly 64 KB on Linux). Writers
append bytes; readers consume from the front.

```
   Process A                         Process B
  write(wfd, "hello", 5)            read(rfd, buf, 64)
        │                                  ▲
        ▼                                  │
  ┌──────────────── kernel pipe buffer ───────────────┐
  │  h e l l o _ _ _ _ ... (circular, FIFO)           │
  └───────────────────────────────────────────────────┘
        write end (wfd)            read end (rfd)
```

Key properties:
- **Byte stream, not messages**: there are *no record boundaries*. If A writes
  `"AB"` then `"CD"`, B might read `"ABCD"`, `"A"`+`"BCD"`, etc. You must frame
  messages yourself (length prefixes, delimiters).
- **Unidirectional**: for two-way comms, use **two pipes** or a `socketpair`.
- **Blocking by default**: read on empty blocks; write on full blocks (backpressure).
- **Atomicity**: writes up to `PIPE_BUF` (≥512, 4096 on Linux) are atomic — they won't
  interleave with other writers. Larger writes may be split.

### Shell pipelines
`ls | grep foo` works because the shell creates a pipe, `fork()`s twice, wires `ls`'s
stdout to the write end and `grep`'s stdin to the read end via `dup2`.

```
 ls (stdout=wfd) ──► pipe ──► (stdin=rfd) grep
```

---

## 7.3 Why It Exists (Problem It Solves)

Pipes are the **simplest possible** producer→consumer channel. They existed in early
Unix to compose small tools: each program reads stdin, writes stdout, and pipes glue
them into pipelines. The problem solved: **stream data from one program to another
without temp files, with automatic flow control and EOF signaling.**

---

## 7.4 Performance Characteristics

- **Latency:** low for local data, but each `read`/`write` is a **syscall** and a
  handoff often causes a **context switch** (two per round trip).
- **Throughput:** good for streaming; data is **copied twice** (user→kernel→user).
  Linux `vmsplice`/`splice` can reduce copies for special cases (Ch. 24).
- **Buffer size:** ~64 KB default (tunable via `fcntl(F_SETPIPE_SZ)` on Linux).
  Small buffer → more frequent blocking/wakeups under load.

---

## 7.5 Tradeoffs

| Dimension | Pipe |
|-----------|------|
| **Speed** | Moderate (2 copies + syscalls); fine for streams, not for huge bulk |
| **Complexity** | Very low — easiest IPC to use |
| **Safety** | High — kernel-managed, no shared memory hazards |
| **Structure** | Byte stream only — you implement framing |
| **Directionality** | One-way; need pairs for duplex |
| **Scope** | Anonymous: related procs; FIFO: same host, any procs |

---

## 7.6 When to Use / When Not to Use

**Use pipes when:**
- Streaming bytes parent↔child or in a shell-style pipeline.
- Simple, one-directional producer/consumer on one host.
- You want the simplest reliable channel with built-in backpressure and EOF.

**Avoid pipes when:**
- You need **message boundaries** → use message queues or datagram sockets.
- You need **bidirectional** request/response → use sockets/`socketpair`.
- You move **huge** data with minimal copying → use shared memory.
- Peers are on **different machines** → use TCP sockets.

---

## 7.7 Common Pitfalls

- **Forgetting to close unused ends.** If a reader keeps the *write* end open, it will
  **never see EOF**. Always close the ends you don't use after `fork`.
- **`SIGPIPE` crashes.** Writing to a pipe with no readers raises `SIGPIPE` (default:
  terminate). Handle/ignore it and check for `EPIPE`.
- **Assuming message framing.** Byte stream ≠ messages; partial reads/writes are normal.
- **Deadlock from full buffers.** Two processes each blocked writing while neither reads
  → classic pipe deadlock. Drain both directions (use non-blocking or separate threads).

---

## 7.8 Key Takeaways

- A pipe is a **unidirectional, FIFO, byte-stream** kernel buffer.
- **Anonymous** = inherited by related processes; **FIFO** = named on the filesystem.
- No message boundaries — **you must frame** your data.
- Simple and safe, with built-in **backpressure** and **EOF**, but **two copies** and
  syscall/context-switch overhead make it unsuitable for huge bulk transfers.
- Watch for the **unclosed-write-end / no-EOF** and **SIGPIPE** pitfalls.

---

[← Previous](06-naming-addressing-lifetime.md) | [Back to ToC](../README.md) | [Next: Message Queues →](08-message-queues.md)
