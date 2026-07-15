<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>A R M &nbsp; A R C H I T E C T U R E</samp>
</p>

<h1 align="center">📘 ARM Architecture — For Embedded & Qualcomm Interviews</h1>

<p align="center"><em>"Every Snapdragon starts with ARM."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | ARM Overview & History | [§1](#-1-arm-overview--history) |
| 2 | ARM vs x86 | [§2](#-2-arm-vs-x86) |
| 3 | ARM Registers | [§3](#-3-arm-registers) |
| 4 | ARM Instruction Set (A32 / T32 / A64) | [§4](#-4-arm-instruction-set-a32--t32--a64) |
| 5 | Pipeline & Execution | [§5](#-5-pipeline--execution) |
| 6 | Exception Levels & Privilege | [§6](#-6-exception-levels--privilege) |
| 7 | Memory Management (MMU, TLB, Page Tables) | [§7](#-7-memory-management-mmu-tlb-page-tables) |
| 8 | Cache Architecture | [§8](#-8-cache-architecture) |
| 9 | ARM TrustZone | [§9](#-9-arm-trustzone) |
| 10 | Interrupts & Exceptions | [§10](#-10-interrupts--exceptions) |
| 11 | ARM big.LITTLE & DynamIQ | [§11](#-11-arm-biglittle--dynamiq) |
| 12 | Qualcomm Snapdragon SoC | [§12](#-12-qualcomm-snapdragon-soc) |
| 13 | Interview Questions & Answers | [§13](#-13-interview-questions--answers) |

---

## 🔹 1. ARM Overview & History

### What is ARM?

**ARM** (Advanced RISC Machines) is not a chip manufacturer — it's a **chip design company**. ARM licenses its processor designs to companies like Qualcomm, Apple, Samsung, and MediaTek, who then build their own chips based on ARM's architecture.

```
ARM Holdings designs → Licenses to → Qualcomm builds Snapdragon
                                   → Apple builds M-series / A-series
                                   → Samsung builds Exynos
                                   → MediaTek builds Dimensity
```

### Key ARM Families

| Family | Architecture | Bits | Use Case |
|--------|:----------:|:----:|----------|
| Cortex-A | ARMv7-A / ARMv8-A / ARMv9 | 32/64 | Application processors (phones, laptops) |
| Cortex-R | ARMv7-R / ARMv8-R | 32/64 | Real-time (automotive, storage controllers) |
| Cortex-M | ARMv6-M / ARMv7-M / ARMv8-M | 32 | Microcontrollers (IoT, sensors, wearables) |

```
               Performance
                    ↑
    Cortex-A78      │  ★ Smartphones, Laptops
    Cortex-A55      │  ★ Efficiency cores
                    │
    Cortex-R52      │  ★ ADAS, Hard disk controllers
                    │
    Cortex-M7       │  ★ Drones, motor control
    Cortex-M4       │  ★ Wearables, sensors
    Cortex-M0+      │  ★ Ultra-low-power IoT
                    └──────────────────→ Power Efficiency
```

**Qualcomm context:**
- **Kryo** CPUs = Qualcomm's custom ARM Cortex-A cores
- **Hexagon** DSP = Qualcomm's proprietary (not ARM)
- **Adreno** GPU = Qualcomm's proprietary (not ARM)

---

## 🔹 2. ARM vs x86

| Feature | ARM | x86 (Intel/AMD) |
|---------|-----|-----------------|
| Architecture | RISC (Reduced Instruction Set) | CISC (Complex Instruction Set) |
| Instructions | Simple, fixed-length (mostly 32-bit) | Complex, variable-length (1-15 bytes) |
| Registers | 31 general-purpose (AArch64) | 16 general-purpose (x86-64) |
| Power | Very low (designed for mobile) | Higher (designed for performance) |
| Decode | Simple — direct decode | Complex — must decompose into micro-ops |
| Memory | Load/Store architecture | Register-memory operations allowed |
| Endianness | Bi-endian (configurable) | Little-endian only |
| Market | Mobile, embedded, servers | Desktop, laptop, servers |

### RISC vs CISC — The Core Difference

```
CISC (x86):
  ADD [memory], register     ← One instruction does load + add + store
  Complex, variable length, fewer instructions but each does more

RISC (ARM):
  LDR R1, [memory]          ← Load from memory to register
  ADD R0, R0, R1             ← Add registers
  STR R0, [memory]           ← Store result to memory
  Simple, fixed length, more instructions but each is fast (1 cycle)
```

> **Why ARM wins on mobile:** Simple instructions = simple hardware = less power. A phone running ARM all day uses a fraction of what x86 would consume.

---

## 🔹 3. ARM Registers

### AArch64 (64-bit) Registers

```
General Purpose (31 registers):
  X0 - X30     64-bit registers
  W0 - W30     Lower 32 bits of X0-X30

Special:
  X0 - X7      Function arguments & return values
  X8            Indirect result (struct return)
  X9 - X15     Temporary (caller-saved)
  X16 - X17    Intra-procedure call (linker use)
  X18          Platform register (reserved on some OS)
  X19 - X28    Callee-saved (must preserve across calls)
  X29 (FP)     Frame pointer
  X30 (LR)     Link register (return address)
  SP            Stack pointer (not X31 — separate)
  PC            Program counter (not directly accessible)
  XZR / WZR    Zero register (reads as 0, writes discarded)
```

### AArch32 (32-bit) Registers

```
General Purpose:
  R0 - R12     General purpose
  R13 (SP)     Stack pointer
  R14 (LR)     Link register
  R15 (PC)     Program counter

Special:
  CPSR          Current Program Status Register
                ┌──┬──┬──┬──┬────────────────┬──┬──┬──┬──┐
                │N │Z │C │V │    Reserved     │I │F │T │M │
                └──┴──┴──┴──┴────────────────┴──┴──┴──┴──┘
                 │  │  │  │                    │  │  │  └─ Mode bits
                 │  │  │  │                    │  │  └─── Thumb state
                 │  │  │  │                    │  └───── FIQ disable
                 │  │  │  │                    └─────── IRQ disable
                 │  │  │  └── Overflow
                 │  │  └───── Carry
                 │  └──────── Zero
                 └─────────── Negative
```

### Calling Convention (AAPCS64)

```
Function call: foo(a, b, c, d, e, f, g, h, extra)
                X0 X1 X2 X3 X4 X5 X6 X7  [stack]

Return value:   X0 (or X0:X1 for 128-bit)

Caller-saved:   X0-X15  (caller must save if needed across calls)
Callee-saved:   X19-X28 (callee must preserve and restore)
```

---

## 🔹 4. ARM Instruction Set (A32 / T32 / A64)

### Three Instruction Sets

| Set | Bits | Encoding | Use |
|-----|:----:|:--------:|-----|
| **A32 (ARM)** | 32-bit | Fixed 32-bit | Performance |
| **T32 (Thumb-2)** | 16/32-bit | Mixed 16+32 | Code density (smaller binaries) |
| **A64 (AArch64)** | 64-bit | Fixed 32-bit | Modern 64-bit (ARMv8+) |

### Common A64 Instructions

```asm
// ── Data Processing ──
MOV  X0, #42           // X0 = 42
ADD  X0, X1, X2        // X0 = X1 + X2
SUB  X0, X1, #10       // X0 = X1 - 10
MUL  X0, X1, X2        // X0 = X1 * X2
AND  X0, X1, X2        // X0 = X1 & X2
ORR  X0, X1, X2        // X0 = X1 | X2
EOR  X0, X1, X2        // X0 = X1 ^ X2
LSL  X0, X1, #3        // X0 = X1 << 3
LSR  X0, X1, #3        // X0 = X1 >> 3

// ── Memory Access (Load/Store) ──
LDR  X0, [X1]          // Load 64-bit from address in X1
STR  X0, [X1]          // Store 64-bit to address in X1
LDRB W0, [X1]          // Load byte (zero-extended)
LDP  X0, X1, [X2]      // Load pair (X0 and X1 from consecutive memory)
STP  X0, X1, [SP, #-16]! // Store pair with pre-decrement (push)

// ── Branching ──
B    label              // Unconditional branch
BL   function           // Branch with link (function call — sets LR)
BLR  X0                 // Branch to address in X0 with link
RET                     // Return (branch to LR)
CBZ  X0, label          // Compare & branch if zero
CBNZ X0, label          // Compare & branch if not zero
B.EQ label              // Branch if equal (after CMP)
B.NE label              // Branch if not equal
B.GT label              // Branch if greater than
B.LT label              // Branch if less than

// ── Compare ──
CMP  X0, X1             // Compare (sets flags: N, Z, C, V)
TST  X0, #0xFF          // Test bits (AND, sets flags, discards result)
```

### Conditional Execution (AArch32 only)

In A32, almost every instruction can be conditionally executed:

```asm
// AArch32 — conditional execution (unique to ARM!)
CMP   R0, #0
ADDNE R1, R1, #1       // ADD only if Not Equal (R0 != 0)
MOVEQ R2, #0           // MOV only if Equal (R0 == 0)
// No branch penalty! Saves pipeline flushes.
```

In AArch64, conditional execution is replaced by **conditional select**:

```asm
CMP   X0, #0
CSEL  X1, X2, X3, EQ   // X1 = (X0==0) ? X2 : X3
CINC  X1, X1, NE        // X1 = (condition) ? X1+1 : X1
```

---

## 🔹 5. Pipeline & Execution

### What is a Pipeline?

Instead of completing one instruction before starting the next, a pipeline overlaps stages:

```
Without pipeline (1 instruction at a time):
  │ Fetch │ Decode │ Execute │ Write │     │ Fetch │ Decode │ Execute │ Write │
  Time: ─────────────────────────────────────────────────────────────────────→

With 4-stage pipeline:
  Instr 1: │ Fetch │ Decode │ Execute │ Write │
  Instr 2:         │ Fetch  │ Decode  │ Execute │ Write │
  Instr 3:                  │ Fetch   │ Decode  │ Execute │ Write │
  Instr 4:                            │ Fetch   │ Decode  │ Execute │ Write │
  → 4 instructions completing in nearly the same time as 1 without pipeline
```

### Modern ARM Pipelines

| Processor | Pipeline Depth | Out-of-Order? |
|-----------|:--------------:|:-------------:|
| Cortex-M0 | 3 stages | No (in-order) |
| Cortex-M4 | 3 stages | No |
| Cortex-A55 | 8 stages | In-order |
| Cortex-A78 | 13 stages | Yes (out-of-order) |
| Cortex-X2 | 13+ stages | Yes (wide OoO) |

### Pipeline Hazards

| Hazard | Problem | Example | Solution |
|--------|---------|---------|----------|
| **Data** | Instruction needs result from previous | `ADD X0,X1,X2` then `SUB X3,X0,X4` | Forwarding / stall |
| **Control** | Branch changes PC — pipeline has wrong instructions | `B.EQ label` | Branch prediction |
| **Structural** | Two instructions need same hardware unit | Two memory accesses | Stall or duplicate unit |

**Branch prediction** — the processor *guesses* which way a branch will go and fetches instructions speculatively. If wrong → **pipeline flush** (expensive!).

```c
// Help the branch predictor:
if (__builtin_expect(error, 0)) {    // Tell compiler: "this is unlikely"
    handle_error();
}
```

---

## 🔹 6. Exception Levels & Privilege

### AArch64 Exception Levels

```
┌────────────────────────────────────────────────────┐
│                   EL3 — Secure Monitor             │ ← ARM Trusted Firmware
│                   (Highest privilege)               │    Switch between worlds
├────────────────────────────────────────────────────┤
│                   EL2 — Hypervisor                 │ ← Runs virtual machines
│                                                     │    (KVM, Xen, Qualcomm)
├────────────────────────────────────────────────────┤
│                   EL1 — OS Kernel                  │ ← Linux, QNX, RTOS
│                   (Supervisor mode)                 │
├────────────────────────────────────────────────────┤
│                   EL0 — User Application           │ ← Your app
│                   (Lowest privilege)                │
└────────────────────────────────────────────────────┘
```

| Level | Who Runs | Can Access |
|:-----:|----------|-----------|
| EL0 | User apps | Own memory only |
| EL1 | Kernel | Page tables, system registers, all EL0 memory |
| EL2 | Hypervisor | Stage-2 page tables, can trap EL1 operations |
| EL3 | Secure Monitor | Everything, controls Secure/Non-Secure worlds |

### Transitions Between Levels

```
User app (EL0) → System call (SVC instruction) → Kernel (EL1)
Kernel (EL1) → Hypervisor call (HVC instruction) → Hypervisor (EL2)
Any level → Secure monitor call (SMC instruction) → Secure Monitor (EL3)

Hardware interrupt → jumps to configured EL (typically EL1 for IRQ)
```

---

## 🔹 7. Memory Management (MMU, TLB, Page Tables)

### Virtual Memory — The Big Picture

```
Process A sees:                Physical RAM:
0x0000 ┌──────────┐           ┌──────────────────┐
       │ .text    │──────────→│ Frame 42         │
0x1000 │ .data    │──────────→│ Frame 107        │
0x2000 │ heap     │──────────→│ Frame 3          │
       │          │           │                  │
0xFFFF │ stack    │──────────→│ Frame 89         │
       └──────────┘           └──────────────────┘

Process B sees the SAME virtual addresses but maps to DIFFERENT physical frames.
Each process thinks it has the whole address space.
```

### MMU (Memory Management Unit)

The MMU translates **virtual addresses** (what software uses) to **physical addresses** (actual RAM):

```
CPU → Virtual Address → MMU → Physical Address → Memory/Cache

MMU uses PAGE TABLES to translate:
  Virtual Page Number → Physical Frame Number + permissions

Page size: typically 4KB, 16KB, or 64KB
  Larger pages = fewer TLB entries needed, but more internal fragmentation
```

### Page Table Walk (AArch64, 4 levels)

```
Virtual Address (48-bit):
┌────────┬────────┬────────┬────────┬────────────┐
│ L0 idx │ L1 idx │ L2 idx │ L3 idx │ Page offset│
│ [47:39]│ [38:30]│ [29:21]│ [20:12]│  [11:0]    │
└────┬───┴────┬───┴────┬───┴────┬───┴────────────┘
     │        │        │        │
     ↓        ↓        ↓        ↓
   L0 Table→L1 Table→L2 Table→L3 Table→Physical Page
   (TTBR0)
```

### TLB (Translation Lookaside Buffer)

The TLB is a **cache of recent translations**. Without it, every memory access would require 4 page table walks (4 memory accesses just to translate!).

```
CPU → Virtual Address → TLB Lookup
                         │
                    Hit? ─┼─ Yes → Physical Address (fast! 1 cycle)
                         │
                         No → Page Table Walk (slow! ~100 cycles)
                              → Store result in TLB for next time
```

**TLB flush** happens on context switch (different process = different page tables). This is why context switches are expensive.

**ASID (Address Space ID):** Tags TLB entries with a process ID so you don't need a full flush on every context switch.

---

## 🔹 8. Cache Architecture

### Cache Hierarchy

```
CPU Core
  ┌─────────┐    ~1 cycle     ┌──────────┐
  │ Register│←───────────────→│ L1 Cache │  32-64 KB (split I-cache + D-cache)
  └─────────┘                 └────┬─────┘
                                   │ ~10 cycles
                              ┌────┴─────┐
                              │ L2 Cache │  256 KB - 1 MB (per core)
                              └────┬─────┘
                                   │ ~30-50 cycles
                              ┌────┴─────┐
                              │ L3 Cache │  2-16 MB (shared across cores)
                              └────┬─────┘
                                   │ ~100+ cycles
                              ┌────┴─────┐
                              │   DRAM   │  GB (main memory)
                              └──────────┘
```

### Cache Line

Memory is transferred in **cache lines** (typically 64 bytes):

```
Request for address 0x1004:
  Cache loads entire line: 0x1000 - 0x103F (64 bytes)
  Next access to 0x1010 → cache HIT (already loaded)

This is why sequential access is fast (spatial locality).
```

### Cache Associativity

| Type | Description |
|------|-------------|
| **Direct-mapped** | Each address maps to exactly 1 cache line. Fast but conflict-prone. |
| **N-way set associative** | Each address can go in N possible cache lines. Good balance. |
| **Fully associative** | Can go anywhere. Flexible but expensive to search. |

Most modern ARM caches are **4-way or 8-way set associative**.

### Cache Coherency — MESI Protocol

When multiple cores have their own L1/L2 caches, they might have different copies of the same data. The **MESI protocol** keeps them synchronized:

```
  State       Meaning
  ─────       ───────
  Modified    Cache has the ONLY copy, it's DIRTY (changed)
  Exclusive   Cache has the ONLY copy, it's CLEAN
  Shared      Multiple caches have CLEAN copies
  Invalid     Cache line is not valid

Core A writes to address X:
  1. Core A sends "invalidate X" to all other cores
  2. Other cores mark their copy of X as Invalid
  3. Core A changes state to Modified
  4. When Core B reads X: Core A provides data (write-back), both go to Shared
```

### False Sharing

```cpp
// ❌ BAD: Two threads modify adjacent data → same cache line bounces
struct Counters {
    int countA;    // Thread A writes this
    int countB;    // Thread B writes this — SAME cache line!
};
// Both cores constantly invalidate each other's cache line

// ✅ FIX: Pad to separate cache lines
struct Counters {
    alignas(64) int countA;    // Own cache line
    alignas(64) int countB;    // Own cache line
};
```

---

## 🔹 9. ARM TrustZone

### The Concept

TrustZone divides the processor into two **worlds**:

```
┌─────────────────────┬─────────────────────┐
│   Normal World      │   Secure World      │
│   (Non-Secure)      │   (Secure)          │
│                     │                     │
│   Android / Linux   │   Trusted OS        │
│   Regular apps      │   (QTEE / OP-TEE)   │
│   Browsers, games   │   Crypto keys       │
│                     │   DRM content       │
│                     │   Biometric data    │
│                     │   Payment info      │
├─────────────────────┴─────────────────────┤
│           EL3 Secure Monitor              │
│        (Switches between worlds)          │
└───────────────────────────────────────────┘
```

**Normal World** cannot access Secure World memory, even the kernel. The NS (Non-Secure) bit in the bus tags every transaction.

**Qualcomm context:** Qualcomm's **QTEE** (Qualcomm Trusted Execution Environment) runs in the Secure World. Used for:
- Fingerprint/face data storage
- DRM key management (Widevine L1)
- Secure boot chain verification
- SIM/eSIM cryptographic operations

---

## 🔹 10. Interrupts & Exceptions

### GIC (Generic Interrupt Controller)

ARM uses the **GIC** to manage interrupts across all cores:

```
Hardware IRQ sources:
  Timer ──────────┐
  UART ───────────┤
  GPIO ───────────┤──→ GIC ──→ Routes to correct CPU core
  DMA ────────────┤         ──→ Priority arbitration
  Watchdog ───────┘         ──→ Preemption
```

### Interrupt Types

| Type | Abbreviation | Priority | Use |
|------|:------------:|:--------:|-----|
| **FIQ** | Fast Interrupt | Highest | Secure, time-critical |
| **IRQ** | Interrupt Request | Medium | Normal peripheral interrupts |
| **SError** | System Error | Varies | Asynchronous abort (ECC, bus error) |
| **Synchronous** | — | Varies | Instruction faults, SVC, data abort |

### Exception Vector Table (AArch64)

```
  Offset   Exception Type
  ──────   ──────────────
  0x000    Synchronous (current EL, SP_EL0)
  0x080    IRQ
  0x100    FIQ
  0x180    SError

  0x200    Synchronous (current EL, SP_ELx)
  0x280    IRQ
  0x300    FIQ
  0x380    SError

  0x400    Synchronous (lower EL, AArch64)
  0x480    IRQ
  0x500    FIQ
  0x580    SError

  0x600    Synchronous (lower EL, AArch32)
  0x680    IRQ
  ...
```

Each entry is **128 bytes** (32 instructions) — enough for a branch to the actual handler.

---

## 🔹 11. ARM big.LITTLE & DynamIQ

### big.LITTLE

Combines high-performance "big" cores with energy-efficient "LITTLE" cores:

```
┌────────────────────────┬────────────────────────┐
│    big Cores           │    LITTLE Cores        │
│    (Cortex-A78)        │    (Cortex-A55)        │
│    High performance    │    Low power           │
│    Gaming, camera      │    Email, idle, music  │
│    2.8 GHz             │    1.8 GHz             │
└────────────────────────┴────────────────────────┘
           Shared L3 Cache
```

### DynamIQ (Successor to big.LITTLE)

DynamIQ allows mixing different core types **within the same cluster**:

```
Snapdragon 8 Gen 3:
┌──────────────────────────────────────────┐
│ 1x Cortex-X4 (Prime)    — 3.3 GHz       │  ← Peak single-thread
│ 3x Cortex-A720 (Perf)   — 3.2 GHz       │  ← Sustained multi-thread
│ 2x Cortex-A720 (Perf)   — 2.3 GHz       │  ← Balanced
│ 2x Cortex-A520 (Eff)    — 1.9 GHz       │  ← Background tasks
└──────────────────────────────────────────┘
                 ALL in one DSU (DynamIQ Shared Unit)
                 Shared L3 Cache
```

---

## 🔹 12. Qualcomm Snapdragon SoC

### SoC Block Diagram

```
┌─────────────────────────────────────────────────────────┐
│                    Snapdragon SoC                        │
│                                                         │
│  ┌─────────┐  ┌─────────┐  ┌──────────┐  ┌──────────┐ │
│  │ Kryo CPU│  │ Adreno  │  │ Hexagon  │  │ Spectra  │ │
│  │ (ARM)   │  │  GPU    │  │  DSP     │  │  ISP     │ │
│  │ 8 cores │  │         │  │ AI+Audio │  │ Camera   │ │
│  └────┬────┘  └────┬────┘  └────┬─────┘  └────┬─────┘ │
│       │             │            │              │       │
│  ─────┴─────────────┴────────────┴──────────────┴────── │
│                    NoC (Network on Chip)                 │
│  ─────┬─────────────┬────────────┬──────────────┬────── │
│       │             │            │              │       │
│  ┌────┴────┐  ┌─────┴───┐  ┌────┴─────┐  ┌────┴────┐  │
│  │ Modem   │  │ Memory  │  │ Security │  │ Periph  │  │
│  │ 5G/LTE  │  │ Ctrl    │  │ (QTEE)   │  │ I2C/SPI │  │
│  │ (X75)   │  │ LPDDR5X │  │ TrustZone│  │ UART/USB│  │
│  └─────────┘  └─────────┘  └──────────┘  └─────────┘  │
└─────────────────────────────────────────────────────────┘
```

### Key Components

| Component | What It Is | Software Interface |
|-----------|-----------|-------------------|
| **Kryo** | Custom ARM CPU cores (big.LITTLE) | Linux/QNX runs here |
| **Adreno** | GPU (graphics + compute) | OpenGL ES, Vulkan, OpenCL |
| **Hexagon** | DSP (scalar + vector + tensor) | FastRPC, Hexagon SDK |
| **Spectra** | Image Signal Processor | Camera HAL, ISP firmware |
| **X75 Modem** | 5G/LTE modem | QMI protocol, modem firmware |
| **QTEE** | Trusted Execution Environment | Secure world services |
| **NoC** | Network on Chip (interconnect) | QoS configuration |

### Communication Between Components

```
Application (ARM EL0)
      │
      │ ioctl / system call
      ↓
Kernel Driver (ARM EL1)
      │
      │ FastRPC / shared memory
      ↓
Hexagon DSP
      │
      │ Processes audio/AI/camera
      ↓
Result back via shared memory → Driver → Application
```

---

## 🔹 13. Interview Questions & Answers

---

### Q1. What is the difference between ARM and Thumb instruction sets?

**A:** ARM instructions are fixed 32-bit, Thumb is a compact 16-bit subset (Thumb-2 mixes 16+32 bit). Thumb reduces code size by ~30% with minimal performance loss. In AArch64 (ARMv8+), there's only one instruction set — A64 (fixed 32-bit encoding with 64-bit operations), so Thumb distinction doesn't apply.

---

### Q2. Why does ARM use a load/store architecture?

**A:** In ARM, arithmetic instructions ONLY operate on registers — they cannot directly access memory. Separate `LDR`/`STR` instructions handle memory access. This simplifies the hardware pipeline (memory access is a separate stage), enables out-of-order execution, and keeps instruction encoding uniform.

---

### Q3. What happens when an interrupt occurs on ARM?

**A:**
1. CPU finishes current instruction
2. Saves PSTATE (flags) to SPSR_ELx
3. Saves return address to ELR_ELx
4. Switches to the target exception level
5. Sets PC to the vector table entry for that interrupt type
6. ISR executes
7. ISR returns with `ERET` → restores PSTATE and PC

---

### Q4. Explain the difference between EL0, EL1, EL2, EL3.

**A:** EL0 = user applications (least privilege). EL1 = OS kernel (manages memory, scheduling). EL2 = hypervisor (virtualizes EL1 — runs multiple OSes). EL3 = secure monitor (switches between Secure and Normal worlds, highest privilege). Each higher level can access everything below it.

---

### Q5. What is the TLB? Why is a TLB miss expensive?

**A:** The TLB caches recent virtual→physical address translations. A TLB hit takes ~1 cycle. A TLB miss requires a **page table walk** — traversing 3-4 levels of page tables in memory (~100+ cycles). On a context switch, the TLB may need to be flushed (ASIDs help avoid full flushes).

---

### Q6. What is cache coherency and why does it matter in multicore?

**A:** Each core has its own L1/L2 cache. If Core A writes to address X, Core B's cached copy is stale. The MESI protocol keeps caches synchronized: Modified (dirty, exclusive), Exclusive (clean, exclusive), Shared (clean, multiple copies), Invalid. Without coherency, multicore programs would read stale data.

---

### Q7. What is false sharing and how do you prevent it?

**A:** False sharing occurs when two threads modify different variables that happen to be on the same cache line. Even though they're writing different data, the cache line bounces between cores' caches (invalidate → transfer → invalidate). Fix: pad variables to cache line boundaries using `alignas(64)`.

---

### Q8. How does TrustZone work?

**A:** TrustZone splits the processor into Secure and Normal worlds using a hardware NS (Non-Secure) bit on every bus transaction. Normal world (Linux, Android) cannot access Secure world memory. The Secure Monitor (EL3) handles switching. Used for DRM, keys, biometrics.

---

### Q9. What is the difference between Cortex-A, Cortex-R, and Cortex-M?

**A:** Cortex-A = application processors (phones, servers) — performance, virtual memory, full OS. Cortex-R = real-time processors — deterministic latency, used in automotive/storage. Cortex-M = microcontrollers — ultra-low-power, no MMU (optional MPU), bare-metal or RTOS.

---

### Q10. Explain the ARM memory ordering model.

**A:** ARM uses a **weakly ordered** memory model — the processor can reorder memory accesses for performance. This means:
- Writes may appear in different order to other cores
- Reads may be satisfied from stale cache

To enforce ordering:
```asm
DMB    // Data Memory Barrier — ensures ordering of memory accesses
DSB    // Data Synchronization Barrier — completes all pending accesses
ISB    // Instruction Synchronization Barrier — flushes pipeline
```

In C++, this maps to `std::atomic` memory orderings:
```cpp
std::atomic<int> flag;
flag.store(1, std::memory_order_release);     // All prior writes visible before this
int val = flag.load(std::memory_order_acquire); // All subsequent reads after this
```

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 ARM Architecture — From Registers to SoC</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
