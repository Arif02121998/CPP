# Chapter 22 — OS-Level Services

[← Back to Table of Contents](../README.md) | [← Previous](21-microservices-communication.md)

---

## 22.1 The Setting

Operating systems themselves are built from cooperating processes that talk via IPC.
Understanding these real, shipping examples cements *why* each mechanism exists.

---

## 22.2 Shell Pipelines — Pipes in Action

The most-used IPC on any Unix system:

```
 $ cat access.log | grep ERROR | sort | uniq -c | sort -rn | head
   cat ─pipe─► grep ─pipe─► sort ─pipe─► uniq ─pipe─► sort ─pipe─► head
```

- Each `|` is an **anonymous pipe**; the shell `fork`s each stage and wires
  stdout→stdin via `dup2`.
- **Backpressure** is automatic: if `sort` is slow, the pipe fills and `grep` blocks.
- Demonstrates composition of small tools — the original Unix philosophy.

---

## 22.3 System Logging — Sockets and Queues

- **`syslog` / `journald`**: applications send log messages to a **Unix domain socket**
  (`/dev/log`). A logging daemon collects, filters, and persists them.
- Decouples app from storage; the daemon can buffer, rate-limit, and route logs.
- `journald` uses sockets plus on-disk journals; demonstrates **local UDS as a service
  bus**.

---

## 22.4 D-Bus — Desktop and System Message Bus

Linux desktops and system services coordinate via **D-Bus**, a higher-level IPC built on
**Unix domain sockets**:

```
   App ──┐                         ┌── NetworkManager
   App ──┼──► [ D-Bus daemon ] ◄───┼── systemd
   App ──┘     routing, names      └── Bluetooth service
```

- Provides **method calls** (RPC-style), **signals** (pub/sub events), and **named
  services** with discovery.
- Example: your app asks NetworkManager "are we online?" via a D-Bus **method call**;
  NetworkManager broadcasts a **signal** when connectivity changes.
- Shows IPC layering: **RPC + pub/sub semantics over a UDS transport**.

---

## 22.5 systemd — Signals, Sockets, and Socket Activation

- **Signals** for lifecycle: `systemctl stop` ultimately sends **`SIGTERM`** (graceful),
  then **`SIGKILL`** after a timeout. `SIGHUP` often triggers **config reload**.
- **Socket activation:** systemd creates the listening **socket** and passes its **fd**
  to the service on demand (via fd inheritance). The service starts only when a client
  connects — lazy startup, faster boot, and zero lost connections during restarts.
- Demonstrates **fd passing** and **signals** as the OS's process-control backbone.

---

## 22.6 The X Window System / Display Servers

- **X11** and **Wayland** are **client/server**: applications (clients) talk to the
  display server over a **Unix domain socket** (local) or TCP (remote X).
- Large pixel buffers are shared via **shared memory** (MIT-SHM extension) to avoid
  copying images through the socket — a textbook case of **"control over socket, bulk
  data over shared memory."**

```
 App ──UDS: draw commands──► Display server
 App ══shared memory: pixel buffer══► Display server   (zero-copy frames)
```

---

## 22.7 Databases — Shared Memory + Sockets

- **PostgreSQL**: a **multi-process** architecture. Backends share a big **shared-memory**
  segment (buffer pool, locks, WAL buffers) protected by semaphores/spinlocks. Clients
  connect over **Unix domain sockets** (local) or **TCP** (remote).
- Demonstrates the full stack: **shared memory** for high-performance internal state,
  **sockets** for client communication.

---

## 22.8 Container Runtimes — UDS and fd Passing

- **Docker/containerd/Kubernetes CRI**: the CLI/kubelet talks to the daemon over a
  **Unix domain socket** (`/var/run/docker.sock`).
- **fd passing** over UDS hands open file descriptors (e.g., a container's stdio,
  network sockets) between processes — something **only Unix sockets** can do.

---

## 22.9 The OS Kernel Itself

- **Signals** deliver hardware/exception events (`SIGSEGV`, `SIGFPE`) and child status
  (`SIGCHLD`).
- **Pseudo-terminals (ptys)** connect terminal emulators to shells via a kernel-mediated
  bidirectional channel — IPC underlying every terminal window and `ssh` session.
- **`/proc` and `/sys`**: file-like interfaces let user space query/configure the kernel —
  IPC dressed as a filesystem ("everything is a file").

---

## 22.10 Patterns You Can Steal

From these examples, recurring **architectural patterns**:

1. **Control plane / data plane split** — small messages (commands) over sockets/queues;
   bulk data over shared memory (X11, databases).
2. **Local daemon + UDS** — a privileged/central service reached by many clients
   (syslog, Docker, D-Bus).
3. **Signals for lifecycle, sockets for work** — systemd, most daemons.
4. **fd passing** to delegate resources across processes (systemd socket activation,
   containers).
5. **File-as-interface** — `/proc`, `/sys`, FIFOs for simple discoverable IPC.

---

## 22.11 Key Takeaways

- Real OS services demonstrate every mechanism: **pipes** (shell), **UDS** (syslog,
  D-Bus, Docker), **shared memory** (X11, PostgreSQL), **signals** (systemd lifecycle),
  **fd passing** (socket activation, containers).
- The dominant pattern is **"control messages over sockets/queues, bulk data over shared
  memory."**
- **fd passing over Unix sockets** is a unique, powerful capability the OS relies on.
- "Everything is a file" makes **`/proc`, `/sys`, FIFOs** into simple, discoverable IPC.

---

[← Previous](21-microservices-communication.md) | [Back to ToC](../README.md) | [Next: High-Performance Systems →](23-high-performance-systems.md)
