# Chapter 3 — IPC, the OS, and the Process Model

[← Back to Table of Contents](../README.md) | [← Previous](02-why-ipc-is-needed.md)

---

## 3.1 The Process Model Recap

To understand IPC you must understand what a process *is* to the OS:

```
 Process Control Block (PCB) — kernel's record of a process
 ┌─────────────────────────────────────────────┐
 │ PID, parent PID, state (run/ready/blocked)    │
 │ Register snapshot (when not running)          │
 │ Page table pointer  → virtual address space   │
 │ File descriptor table → open files/pipes/...  │
 │ Signal handlers & pending signals             │
 │ Credentials (uid/gid), priority, accounting   │
 └─────────────────────────────────────────────┘
```

Two PCB fields matter most for IPC:

- **Page table** — defines the private address space. Shared memory works by making
  entries in *two* page tables point to the *same physical frames*.
- **File descriptor table** — pipes, sockets, message queues, and mmap'd files are
  all reached through **file descriptors (fds)**. This is why "everything is a file"
  is so powerful in Unix IPC.

---

## 3.2 User Space vs Kernel Space

```
 ┌──────────────── Virtual Address Space ────────────────┐
 │  User space (your code, libs, heap, stack)            │  ← ring 3, restricted
 ├───────────────────────────────────────────────────────┤
 │  Kernel space (scheduler, drivers, IPC buffers)       │  ← ring 0, privileged
 └───────────────────────────────────────────────────────┘
```

- User code **cannot** touch kernel memory or hardware directly.
- To use IPC, a process makes a **system call** (`write`, `read`, `sendmsg`,
  `shmat`, `mmap`, ...), which switches the CPU to kernel mode, does the work, and
  returns. (Details in [Ch. 4](04-kernel-boundary-context-switching.md).)

This boundary is *why* most IPC has overhead: crossing it costs CPU cycles, and many
mechanisms copy data across it.

---

## 3.3 How the Kernel Mediates IPC

The kernel provides IPC objects and tracks them in its own data structures:

| IPC object | Kernel representation | Reached via |
|------------|----------------------|-------------|
| Pipe / FIFO | In-kernel ring buffer | file descriptor |
| Message queue | Linked list of messages | fd (POSIX) or ID (System V) |
| Shared memory | Shared physical frames + page-table entries | mapped address |
| Socket | Socket buffers (sk_buff) + protocol state | file descriptor |
| Signal | Bit in pending-signal mask of target PCB | `kill()` / handler |
| Semaphore | Counter + wait queue | fd or ID |

The kernel also enforces **permissions** (who may open/attach), **lifetime**
(when the object is destroyed), and **blocking/wakeups** (which processes wait and
when they're scheduled to run).

---

## 3.4 Namespaces and Identity

For two processes to communicate, they must **name** the same channel. The OS offers
several namespaces:

- **Inheritance** — anonymous pipes are inherited across `fork()`; no name needed.
- **Filesystem path** — named pipes (FIFOs), Unix domain sockets, mmap'd files.
- **Keys / IDs** — System V IPC uses integer keys (`ftok`).
- **Network address** — TCP/IP uses `IP:port`.
- **Abstract names** — Linux abstract sockets, D-Bus names.

Choosing a namespace affects **discovery** (how peers find each other) and
**security** (who can reach the channel). See [Ch. 6](06-naming-addressing-lifetime.md).

---

## 3.5 The Role of `fork()` and `exec()`

Unix IPC is shaped by its process-creation model:

- **`fork()`** duplicates the parent, including its **file descriptor table**. This is
  why a parent can create a pipe, `fork()`, and instantly share it with the child —
  *no naming required*. This is the classic shell pipeline mechanism.
- **`exec()`** replaces the program image but, by default, **keeps open fds** (unless
  marked close-on-exec). So inherited pipes/sockets survive into the new program.

```
 parent: pipe(fds) ──► fork() ──► child inherits fds[0], fds[1]
                                  │
                                  └─► exec("grep") still has the pipe
```

This inheritance model is the foundation of shell pipelines like `ls | grep foo`.

---

## 3.6 Where IPC Lives in the OS Stack

```
 ┌─────────────────────────────────────────────┐
 │ Applications (your processes)                │
 ├─────────────────────────────────────────────┤
 │ Libraries (libc, RPC stubs, message libs)    │  ← convenience wrappers
 ├─────────────────────────────────────────────┤
 │ System call interface                        │  ← the kernel boundary
 ├─────────────────────────────────────────────┤
 │ Kernel IPC subsystems                        │
 │  • VFS (pipes, FIFOs, sockets as files)      │
 │  • Networking stack (TCP/IP, Unix sockets)   │
 │  • Memory manager (shared memory, mmap)      │
 │  • Signal subsystem                          │
 │  • Scheduler (wakeups, blocking)             │
 ├─────────────────────────────────────────────┤
 │ Hardware (MMU, CPU, NIC)                      │
 └─────────────────────────────────────────────┘
```

---

## 3.7 Key Takeaways

- A process is defined by its **page table** (address space) and **fd table** —
  the two structures IPC manipulates.
- IPC almost always crosses the **user/kernel boundary**, which is the source of
  its overhead.
- The kernel is the **trusted broker**: it represents IPC objects, enforces
  permissions and lifetime, and schedules blocked waiters.
- Unix's **`fork()`/`exec()`** model makes fd inheritance a first-class IPC tool.

---

[← Previous](02-why-ipc-is-needed.md) | [Back to ToC](../README.md) | [Next: The Kernel Boundary and Context Switching →](04-kernel-boundary-context-switching.md)
