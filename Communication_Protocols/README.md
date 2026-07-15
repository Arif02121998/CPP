<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>C O M M U N I C A T I O N &nbsp; P R O T O C O L S</samp>
</p>

<h1 align="center">📘 Communication Protocols — I2C, SPI, UART, CAN & More</h1>

<p align="center"><em>"How chips talk to each other."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Protocol Comparison Overview | [§1](#-1-protocol-comparison-overview) |
| 2 | UART | [§2](#-2-uart--universal-asynchronous-receiver-transmitter) |
| 3 | I2C | [§3](#-3-i2c--inter-integrated-circuit) |
| 4 | SPI | [§4](#-4-spi--serial-peripheral-interface) |
| 5 | CAN Bus | [§5](#-5-can-bus--controller-area-network) |
| 6 | PCIe | [§6](#-6-pcie--peripheral-component-interconnect-express) |
| 7 | USB | [§7](#-7-usb--universal-serial-bus) |
| 8 | Ethernet & TCP/IP | [§8](#-8-ethernet--tcpip) |
| 9 | Interview Questions | [§9](#-9-interview-questions) |

---

## 🔹 1. Protocol Comparison Overview

| Protocol | Wires | Speed | Topology | Duplex | Distance | Use Case |
|----------|:-----:|:-----:|----------|:------:|:--------:|----------|
| **UART** | 2 (TX, RX) | ~115 Kbps - 3 Mbps | Point-to-point | Full | Short | Debug console, GPS, Bluetooth |
| **I2C** | 2 (SDA, SCL) | 100 Kbps - 3.4 Mbps | Multi-master, multi-slave | Half | Short (<1m) | Sensors, EEPROM, PMIC |
| **SPI** | 4 (MOSI, MISO, SCLK, CS) | 10-100+ Mbps | 1 master, N slaves | Full | Short | Flash, display, ADC |
| **CAN** | 2 (CAN_H, CAN_L) | 1 Mbps (classic) | Multi-master bus | Half | Long (40m) | Automotive, industrial |
| **PCIe** | 2+ lanes | 8-64 GT/s per lane | Point-to-point | Full | Short (board) | GPU, NVMe, WiFi card |
| **USB** | 2/4 (D+, D-) | 12 Mbps - 40 Gbps | Host-device tree | Half/Full | 5m (cable) | Everything |

---

## 🔹 2. UART — Universal Asynchronous Receiver-Transmitter

### The Concept

UART is the **simplest** serial protocol. Two devices connect with just **two wires** (TX and RX). There's no clock signal — both sides must agree on the **baud rate** (speed) beforehand.

```
Device A                    Device B
┌──────┐                    ┌──────┐
│  TX  │────────────────────│  RX  │
│  RX  │────────────────────│  TX  │
│  GND │────────────────────│  GND │
└──────┘                    └──────┘
         No clock wire!
         Both must use the SAME baud rate.
```

### Frame Format

```
Idle ─────┐   ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐────── Idle
(HIGH)    │   │ D0│ D1│ D2│ D3│ D4│ D5│ D6│ D7│ P │   │      (HIGH)
          └───┘                                       └───┘
          Start                                       Stop
          bit(0)        8 Data bits          Parity   bit(1)
```

| Field | Bits | Description |
|-------|:----:|-------------|
| Start | 1 | Always 0 — signals start of frame |
| Data | 5-9 | Usually 8 bits (LSB first) |
| Parity | 0-1 | Optional error check (even/odd) |
| Stop | 1-2 | Always 1 — signals end of frame |

### Common Baud Rates

```
9600    — Legacy, very common in embedded
115200  — Default for Linux console, debug UART
921600  — High-speed UART
3000000 — Maximum for most controllers
```

### UART in Code

```c
// Linux userspace — reading from UART
#include <termios.h>
#include <fcntl.h>

int fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY);

struct termios tty;
tcgetattr(fd, &tty);

cfsetispeed(&tty, B115200);     // Input baud rate
cfsetospeed(&tty, B115200);     // Output baud rate

tty.c_cflag &= ~PARENB;        // No parity
tty.c_cflag &= ~CSTOPB;        // 1 stop bit
tty.c_cflag &= ~CSIZE;
tty.c_cflag |= CS8;            // 8 data bits

tcsetattr(fd, TCSANOW, &tty);

char buf[256];
int n = read(fd, buf, sizeof(buf));     // Read data
write(fd, "Hello\n", 6);                 // Write data
close(fd);
```

### Key Points for Interview

- **No clock** → requires matched baud rates. 1% mismatch can cause errors.
- **No addressing** → strictly point-to-point (1 sender, 1 receiver per pair).
- **Start bit** is how the receiver synchronizes to each byte.
- Very common as **debug console** on embedded boards (Qualcomm uses UART for early boot logs).

---

## 🔹 3. I2C — Inter-Integrated Circuit

### The Concept

I2C (pronounced "I-squared-C") uses just **two wires** to connect dozens of devices on the same bus. One wire for data (SDA), one for clock (SCL). Each device has a unique **7-bit address**.

```
            ┌──── 4.7kΩ ────┐──── 4.7kΩ ────┐
            │     (pull-up)  │     (pull-up)  │
    VCC ────┤                │                │
            │                │                │
    ┌───────┼────────────────┼────────────────┼─────────┐
    │       │     SDA        │     SCL        │         │
    │  ┌────┴────┐     ┌────┴────┐     ┌────┴────┐    │
    │  │ Master  │     │ Slave 1 │     │ Slave 2 │    │
    │  │ (SoC)   │     │ (Sensor)│     │ (EEPROM)│    │
    │  │         │     │ Addr:0x48│    │ Addr:0x50│    │
    │  └─────────┘     └─────────┘     └─────────┘    │
    │                     GND                           │
    └───────────────────────────────────────────────────┘
```

### I2C Transaction — Write

```
Master writes 0x42 to register 0x0A of slave at address 0x48:

S ┌─────────┬─┬─────────┬─┬──────────┬─┬──────────┬─┐ P
  │ 1001000 │0│         │ │ 00001010 │ │ 01000010 │ │
  │ (0x48)  │W│  ACK    │ │ (0x0A)   │A│ (0x42)   │A│
  └─────────┴─┴─────────┴─┴──────────┴─┴──────────┴─┘

S = Start condition (SDA ↓ while SCL HIGH)
P = Stop condition  (SDA ↑ while SCL HIGH)
W = Write bit (0)
R = Read bit (1)
A = ACK from slave (SDA pulled LOW)
```

### I2C Transaction — Read

```
Master reads 2 bytes from register 0x0A of slave 0x48:

Step 1: Write register address
S │0x48│W│A│0x0A│A│

Step 2: Repeated start, then read
Sr│0x48│R│A│ Data1 │A│ Data2 │NACK│P

Sr = Repeated Start (no Stop between write and read)
NACK = Master sends NACK to signal "last byte"
```

### I2C Speeds

| Mode | Speed | Name |
|------|:-----:|------|
| Standard | 100 kHz | Most common |
| Fast | 400 kHz | Sensors, PMIC |
| Fast-Plus | 1 MHz | — |
| High-Speed | 3.4 MHz | Less common |

### I2C in Linux

```c
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

int fd = open("/dev/i2c-1", O_RDWR);
ioctl(fd, I2C_SLAVE, 0x48);          // Set slave address

// Write: register 0x0A, value 0x42
uint8_t buf[2] = {0x0A, 0x42};
write(fd, buf, 2);

// Read register 0x0A
uint8_t reg = 0x0A;
write(fd, &reg, 1);                   // Write register address
uint8_t data;
read(fd, &data, 1);                   // Read value
```

### Key Points for Interview

- **Open-drain** bus with pull-up resistors → any device can pull the line LOW
- **Clock stretching** — slave can hold SCL LOW to ask master to wait
- **Arbitration** — if two masters transmit simultaneously, the one sending 0 wins (0 dominates on open-drain)
- **ACK/NACK** — slave acknowledges every byte. NACK means error or end of read.
- **Qualcomm context:** I2C connects to PMIC (power management), sensors, touch controllers

---

## 🔹 4. SPI — Serial Peripheral Interface

### The Concept

SPI is **faster** than I2C but uses more wires. It's a master-slave protocol with a dedicated clock line and separate data lines for each direction (**full duplex**).

```
            Master                          Slave
         ┌──────────┐                   ┌──────────┐
         │     SCLK │───── Clock ──────→│ SCLK     │
         │     MOSI │───── Data Out ───→│ MOSI     │
         │     MISO │←──── Data In ─────│ MISO     │
         │     CS0  │───── Select ─────→│ CS       │
         │     CS1  │──┐               └──────────┘
         └──────────┘  │
                       │               ┌──────────┐
                       └── Select ────→│ Slave 2  │
                                       └──────────┘
```

### SPI Signal Names

| Signal | Full Name | Direction | Description |
|--------|-----------|:---------:|-------------|
| **SCLK** | Serial Clock | Master → Slave | Clock signal |
| **MOSI** | Master Out, Slave In | Master → Slave | Data from master |
| **MISO** | Master In, Slave Out | Slave → Master | Data to master |
| **CS/SS** | Chip Select | Master → Slave | LOW = selected |

> Modern naming: MOSI → SDO (Serial Data Out), MISO → SDI (Serial Data In), SS → CS

### Clock Polarity & Phase (CPOL / CPHA)

```
Mode 0 (CPOL=0, CPHA=0): Clock idle LOW,  capture on RISING edge  ← Most common
Mode 1 (CPOL=0, CPHA=1): Clock idle LOW,  capture on FALLING edge
Mode 2 (CPOL=1, CPHA=0): Clock idle HIGH, capture on FALLING edge
Mode 3 (CPOL=1, CPHA=1): Clock idle HIGH, capture on RISING edge

Mode 0 timing:
CS   ─────┐                                          ┌────
          └──────────────────────────────────────────┘
SCLK ─────────┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐──────────
              └───┘   └───┘   └───┘   └───┘   └───
MOSI ──────╳ D7  ╳ D6  ╳ D5  ╳ D4  ╳ D3  ╳─────────
                ↑       ↑       ↑       ↑
              Data captured on rising edge
```

### SPI vs I2C

| Feature | SPI | I2C |
|---------|-----|-----|
| Wires | 4 + 1 per slave | 2 (shared) |
| Speed | 10-100+ MHz | 100 kHz - 3.4 MHz |
| Duplex | Full | Half |
| Addressing | CS pin per slave | 7-bit address |
| Max devices | Limited by CS pins | 128 (7-bit) |
| Overhead | No address/ACK bytes | Address + ACK per byte |

### Key Points for Interview

- **No addressing** — uses dedicated CS pin per slave
- **Full duplex** — MOSI and MISO transfer simultaneously (even if one direction is dummy data)
- **No flow control** — slave can't slow down the master (unlike I2C clock stretching) 
- **Qualcomm context:** SPI connects to NOR flash, some display panels, external Wi-Fi/BT chips

---

## 🔹 5. CAN Bus — Controller Area Network

### The Concept

CAN is the backbone of **automotive communication**. Designed by Bosch for cars, it connects dozens of ECUs (Electronic Control Units) on a shared two-wire bus with built-in **priority arbitration** and **error detection**.

```
       CAN_H ───────────┬──────────┬──────────┬──────
                         │          │          │
                     ┌───┴───┐ ┌───┴───┐ ┌───┴───┐
                     │ ECU 1 │ │ ECU 2 │ │ ECU 3 │
                     │Engine │ │ ABS   │ │ ADAS  │
                     └───┬───┘ └───┬───┘ └───┬───┘
                         │          │          │
       CAN_L ───────────┴──────────┴──────────┴──────

       120Ω termination resistors at BOTH ends of the bus
```

### CAN Frame

```
┌─────┬────┬───┬───┬────┬──────────┬─────┬─────┬───┬───┬───────┐
│ SOF │ ID │RTR│IDE│ DLC│   Data   │ CRC │ ACK │EOF│IFS│       │
│ (1) │(11)│(1)│(1)│ (4)│ (0-64B)  │(15) │ (2) │(7)│(3)│       │
└─────┴────┴───┴───┴────┴──────────┴─────┴─────┴───┴───┴───────┘

SOF  = Start of Frame (dominant bit)
ID   = 11-bit identifier (also determines PRIORITY)
RTR  = Remote Transmission Request
IDE  = Identifier Extension (0 = standard 11-bit, 1 = extended 29-bit)
DLC  = Data Length Code (0-8 bytes for classic CAN, 0-64 for CAN FD)
Data = Payload
CRC  = Cyclic Redundancy Check (error detection)
ACK  = Acknowledgment slot (receivers pull dominant if CRC OK)
EOF  = End of Frame
```

### Bitwise Arbitration

```
CAN uses "dominant" (0) and "recessive" (1) — like a wired-AND bus.

Node A sends ID: 0x643 = 110 0100 0011
Node B sends ID: 0x501 = 101 0000 0001

Bit:  10  9  8  7  6  5  4  3  2  1  0
A:     1  1  0  0  1  0  0  0  0  1  1
B:     1  0  ← B sends 0 (dominant), A sends 1 (recessive)
Bus:   1  0  ← Bus shows 0 (B wins!)

A sees the bus is 0 but it sent 1 → A backs off
B doesn't know anyone else was transmitting → B continues
→ LOWER ID = HIGHER PRIORITY (wins arbitration)
```

### CAN vs CAN FD

| Feature | CAN Classic | CAN FD |
|---------|:-----------:|:------:|
| Data bytes | 0-8 | 0-64 |
| Bit rate | Up to 1 Mbps | Up to 8 Mbps (data phase) |
| ID bits | 11 or 29 | 11 or 29 |

### Key Points for Interview

- **No master** — any node can transmit at any time
- **Priority** = message ID (lower ID = higher priority)
- **Error handling** — CRC check, bit stuffing, error counters, auto bus-off
- **Qualcomm context:** CAN bus is essential in ADAS/autonomous driving. Qualcomm Snapdragon Ride platform interfaces with vehicle CAN bus for sensor data, vehicle dynamics.

---

## 🔹 6. PCIe — Peripheral Component Interconnect Express

### The Concept

PCIe is a **high-speed serial** interface used to connect components inside a computer or SoC — GPUs, NVMe SSDs, Wi-Fi cards, etc.

### PCIe Architecture

```
┌──────────────────────────────────────────────┐
│                Root Complex                   │
│            (CPU / SoC)                        │
└──┬──────────────┬──────────────┬─────────────┘
   │              │              │
   │ x16 lanes    │ x4 lanes    │ x1 lane
   ↓              ↓              ↓
┌──────┐     ┌──────┐     ┌──────────┐
│ GPU  │     │ NVMe │     │ Wi-Fi    │
│      │     │ SSD  │     │ Card     │
└──────┘     └──────┘     └──────────┘
```

### PCIe Speeds

| Gen | Per Lane Speed | x1 | x4 | x8 | x16 |
|:---:|:-------------:|:---:|:---:|:---:|:----:|
| 3.0 | 8 GT/s | 1 GB/s | 4 GB/s | 8 GB/s | 16 GB/s |
| 4.0 | 16 GT/s | 2 GB/s | 8 GB/s | 16 GB/s | 32 GB/s |
| 5.0 | 32 GT/s | 4 GB/s | 16 GB/s | 32 GB/s | 64 GB/s |

### PCIe Layers

```
┌─────────────────────────┐
│ Transaction Layer (TLP)  │  ← Memory Read/Write, Config, I/O
├─────────────────────────┤
│ Data Link Layer (DLLP)   │  ← CRC, ACK/NACK, flow control
├─────────────────────────┤
│ Physical Layer (PHY)     │  ← Encoding (128b/130b), serialization
└─────────────────────────┘
```

### Key Points for Interview

- **Serial, not parallel** — each lane is a differential pair (TX+, TX-, RX+, RX-)
- **Packet-based** — data sent as Transaction Layer Packets (TLPs)
- **Hot-pluggable** — devices can be added/removed at runtime
- **Qualcomm context:** PCIe connects modem to application processor in some designs, and NVMe storage in automotive/compute platforms.

---

## 🔹 7. USB — Universal Serial Bus

### USB Speeds

| Version | Speed | Name |
|:-------:|:-----:|------|
| USB 1.1 | 12 Mbps | Full Speed |
| USB 2.0 | 480 Mbps | High Speed |
| USB 3.0 | 5 Gbps | SuperSpeed |
| USB 3.1 | 10 Gbps | SuperSpeed+ |
| USB 3.2 | 20 Gbps | SuperSpeed+ Gen 2x2 |
| USB4 | 40 Gbps | Based on Thunderbolt 3 |

### USB Architecture

```
         Host (PC / SoC)
              │
         Root Hub
         /    |    \
      Hub   Device  Device
      / \
  Device  Device

Host-centric: Only the HOST initiates communication.
Devices respond to polls.
```

### USB Transfer Types

| Type | Use Case | Guaranteed? | Speed |
|------|----------|:-----------:|:-----:|
| **Control** | Setup, configuration | ✅ | Low |
| **Bulk** | File transfer, printing | ✅ (retry) | High (best-effort) |
| **Interrupt** | Keyboard, mouse | ✅ | Low (periodic) |
| **Isochronous** | Audio, video stream | ❌ (no retry) | Guaranteed bandwidth |

### Key Points for Interview

- **Host-driven** — devices can't initiate transfers (except USB OTG)
- **Enumeration** — when plugged in, host assigns address and reads descriptors
- **Endpoints** — each device exposes numbered endpoints (EP0 = control, EP1+ = data)
- **Qualcomm context:** USB on Snapdragon handles charging (USB PD), data (ADB), display (USB-C alt mode)

---

## 🔹 8. Ethernet & TCP/IP

### Ethernet Frame

```
┌──────────┬──────────┬──────┬───────────────────────┬──────┐
│ Preamble │ Dest MAC │ Src  │ Type │    Payload      │ CRC  │
│ (8B)     │ (6B)     │ MAC  │ (2B) │ (46-1500 bytes) │ (4B) │
│          │          │ (6B) │      │                  │      │
└──────────┴──────────┴──────┴──────┴─────────────────┴──────┘
```

### TCP/IP Stack

```
┌─────────────────────────────┐
│ Application  (HTTP, DNS)     │
├─────────────────────────────┤
│ Transport    (TCP / UDP)     │  ← Port numbers (80, 443, 53)
├─────────────────────────────┤
│ Network      (IP)            │  ← IP addresses, routing
├─────────────────────────────┤
│ Data Link    (Ethernet/WiFi) │  ← MAC addresses
├─────────────────────────────┤
│ Physical     (Cable/Radio)   │  ← Electrical signals
└─────────────────────────────┘
```

### Socket Programming (Quick Reference)

```c
// TCP Server
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
listen(server_fd, 5);
int client_fd = accept(server_fd, NULL, NULL);
read(client_fd, buffer, sizeof(buffer));
write(client_fd, response, strlen(response));
close(client_fd);

// TCP Client
int fd = socket(AF_INET, SOCK_STREAM, 0);
connect(fd, (struct sockaddr*)&addr, sizeof(addr));
write(fd, request, strlen(request));
read(fd, response, sizeof(response));
close(fd);
```

---

## 🔹 9. Interview Questions

---

### Q1. Compare I2C and SPI. When would you choose one over the other?

**A:** I2C uses 2 wires and supports multiple devices with addressing — best when pin count is limited and speed isn't critical (sensors, EEPROM, PMIC). SPI uses 4+ wires but is 10-100x faster and full-duplex — best for high-speed peripherals (flash, display, ADC). Choose I2C for simplicity and many slow devices; choose SPI for speed and few devices.

---

### Q2. What happens if two I2C masters transmit simultaneously?

**A:** I2C supports multi-master arbitration. Since SDA is open-drain, both masters monitor the bus. If a master sends 1 (recessive) but sees 0 (dominant from the other master), it loses arbitration and stops transmitting. The winning master doesn't even know contention occurred. No data is corrupted.

---

### Q3. Why does CAN use differential signaling?

**A:** Differential signaling (CAN_H - CAN_L) provides **noise immunity**. External noise affects both wires equally, so the difference stays the same. This allows CAN to work reliably in electrically noisy environments (cars, factories) over long distances.

---

### Q4. What is clock stretching in I2C?

**A:** If a slave needs more time to process data, it holds SCL LOW, preventing the master from generating more clock pulses. The master must wait until SCL goes HIGH before continuing. This is the only flow control mechanism in I2C.

---

### Q5. What is the difference between UART and RS-232?

**A:** **UART** is the protocol (framing, baud rate, start/stop bits). **RS-232** is the physical standard (voltage levels: ±3V to ±15V, connector: DB-9). UART on a microcontroller uses 0V/3.3V logic levels. RS-232 uses inverted ±12V levels. A **level shifter** (MAX232) converts between them.

---

### Q6. Explain PCIe enumeration.

**A:** At boot, the BIOS/firmware walks the PCIe bus tree: reads vendor/device IDs from each slot, assigns bus numbers, allocates memory-mapped I/O (MMIO) regions, and configures Base Address Registers (BARs). This is called **PCIe enumeration**. The OS then loads appropriate drivers based on the discovered devices.

---

### Q7. What is USB enumeration?

**A:** When a USB device is plugged in:
1. Hub detects the connection (voltage change on D+/D-)
2. Hub notifies the host controller
3. Host resets the port
4. Host assigns a unique address (SET_ADDRESS)
5. Host reads device descriptors (GET_DESCRIPTOR)
6. Host selects a configuration (SET_CONFIGURATION)
7. Host loads the appropriate driver

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 Communication Protocols — From Bits on the Wire to Packets on the Bus</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
