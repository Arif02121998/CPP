<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <samp>L I N U X &nbsp; K E R N E L &nbsp; & &nbsp; D R I V E R S</samp>
</p>

<h1 align="center">📘 Linux Kernel & Device Drivers — For Embedded Interviews</h1>

<p align="center"><em>"The kernel is the bridge between hardware and software."</em></p>

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>

---

## 📑 Table of Contents

| # | Topic | Jump |
|:-:|-------|:----:|
| 1 | Linux Kernel Overview | [§1](#-1-linux-kernel-overview) |
| 2 | Kernel Space vs User Space | [§2](#-2-kernel-space-vs-user-space) |
| 3 | System Calls | [§3](#-3-system-calls) |
| 4 | Kernel Modules | [§4](#-4-kernel-modules) |
| 5 | Character Device Drivers | [§5](#-5-character-device-drivers) |
| 6 | Platform Drivers & Device Tree | [§6](#-6-platform-drivers--device-tree) |
| 7 | Interrupts in Linux | [§7](#-7-interrupts-in-linux) |
| 8 | Kernel Synchronization | [§8](#-8-kernel-synchronization) |
| 9 | Memory in the Kernel | [§9](#-9-memory-in-the-kernel) |
| 10 | Proc & Sysfs | [§10](#-10-proc--sysfs) |
| 11 | Linux Boot Process | [§11](#-11-linux-boot-process) |
| 12 | Debugging the Kernel | [§12](#-12-debugging-the-kernel) |
| 13 | Interview Questions | [§13](#-13-interview-questions) |

---

## 🔹 1. Linux Kernel Overview

### What is the Kernel?

The kernel is the core of the operating system — it manages hardware resources and provides services to user applications through a controlled interface (system calls).

```
┌─────────────────────────────────────────────────┐
│              User Space                          │
│   App 1    App 2    App 3    Shell    Daemon     │
├─────────────────────────────────────────────────┤
│              System Call Interface                │
│   open() read() write() ioctl() mmap() fork()   │
├─────────────────────────────────────────────────┤
│              Kernel Space                         │
│  ┌──────────┬───────────┬──────────┬──────────┐ │
│  │ Process  │ Memory    │ File     │ Network  │ │
│  │ Scheduler│ Manager   │ System   │ Stack    │ │
│  │          │ (VMM)     │ (VFS)    │ (TCP/IP) │ │
│  └──────────┴───────────┴──────────┴──────────┘ │
│  ┌──────────────────────────────────────────────┐│
│  │           Device Drivers                      ││
│  │  char │ block │ network │ i2c │ spi │ usb    ││
│  └──────────────────────────────────────────────┘│
├─────────────────────────────────────────────────┤
│              Hardware                             │
│  CPU  │  RAM  │  Storage  │  Peripherals         │
└─────────────────────────────────────────────────┘
```

### Linux Kernel is Monolithic (with Modules)

```
Monolithic:  Everything in ONE big address space (fast, direct function calls)
Microkernel: Minimal kernel, drivers as user-space processes (slower, more isolated)
Linux:       Monolithic BUT supports loadable modules (best of both)

Linux vs QNX:
  Linux = Monolithic → Driver crash = KERNEL CRASH
  QNX   = Microkernel → Driver crash = restart the driver process only
```

---

## 🔹 2. Kernel Space vs User Space

### The Concept

```
┌─────────────────────────────────────┐  0xFFFFFFFF (32-bit)
│                                     │
│         Kernel Space                │  Accessible only in kernel mode
│         (1 GB typically)            │  All physical memory mapped
│                                     │
├─────────────────────────────────────┤  0xC0000000 (3:1 split)
│                                     │
│         User Space                  │  Per-process virtual memory
│         (3 GB typically)            │  Isolated from other processes
│                                     │
│   Stack ↓                           │
│                                     │
│   Heap  ↑                           │
│   BSS / Data / Text                 │
└─────────────────────────────────────┘  0x00000000
```

| Feature | User Space | Kernel Space |
|---------|-----------|-------------|
| Access | Own virtual memory only | All memory, hardware registers |
| Fault | SIGSEGV → process dies | Kernel panic / oops → system crash |
| API | libc (printf, malloc) | Kernel API (printk, kmalloc) |
| Privileges | Restricted | Full hardware access |
| Transition | Via system call (`syscall` instruction) | Returns via `sysret` / `iret` |

### Why Separation Matters

```c
// User space — can't access hardware directly
int *reg = (int *)0x40021000;
*reg = 0xFF;    // SIGSEGV! User space can't access physical addresses.

// Kernel space — can access hardware
void __iomem *reg = ioremap(0x40021000, 4);
writel(0xFF, reg);    // ✅ Works — kernel can map physical memory
iounmap(reg);
```

---

## 🔹 3. System Calls

### The Concept

System calls are the **only** way user-space programs can request kernel services. Each syscall has a number, and the `syscall` instruction triggers a transition to kernel mode.

```
User: printf("hello") → libc: write(1, "hello", 5) → syscall #1 (write)
                                                         │
                                                    ┌────┴─────┐
                                                    │  Kernel  │
                                                    │  sys_write│
                                                    │  VFS     │
                                                    │  Driver  │
                                                    └──────────┘
```

### Common System Calls

| # | Name | Purpose |
|:-:|------|---------|
| 0 | `read` | Read from file descriptor |
| 1 | `write` | Write to file descriptor |
| 2 | `open` | Open a file |
| 3 | `close` | Close file descriptor |
| 9 | `mmap` | Map files/devices into memory |
| 16 | `ioctl` | Device-specific control commands |
| 56 | `clone` | Create new process/thread |
| 57 | `fork` | Create child process |
| 59 | `execve` | Execute a program |
| 60 | `exit` | Terminate process |

### Syscall Flow (ARM64)

```
1. User sets syscall number in X8
2. Sets arguments in X0-X5
3. Executes SVC #0 (Supervisor Call)
4. CPU traps to EL1 (kernel)
5. Kernel looks up syscall table: sys_call_table[X8]
6. Calls the handler function
7. Returns result in X0
8. ERET back to EL0 (user space)
```

---

## 🔹 4. Kernel Modules

### The Concept

Kernel modules are pieces of code that can be **loaded and unloaded** into the kernel at runtime without rebooting. Drivers are typically implemented as modules.

### Minimal Kernel Module

```c
#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void) {
    printk(KERN_INFO "Module loaded!\n");
    return 0;    // 0 = success, negative = error
}

static void __exit my_exit(void) {
    printk(KERN_INFO "Module unloaded!\n");
}

module_init(my_init);    // Called on insmod
module_exit(my_exit);    // Called on rmmod

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Developer");
MODULE_DESCRIPTION("Minimal kernel module");
```

### Module Commands

```bash
# Build (requires kernel headers)
make -C /lib/modules/$(uname -r)/build M=$PWD modules

# Load module
sudo insmod my_module.ko

# List loaded modules
lsmod | grep my_module

# View module info
modinfo my_module.ko

# Remove module
sudo rmmod my_module

# Load with dependencies (preferred)
sudo modprobe my_module

# View kernel messages
dmesg | tail
```

### Makefile for Kernel Module

```makefile
obj-m += my_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
```

### Key Differences: Kernel vs User-Space Programming

| Feature | User Space | Kernel Space |
|---------|-----------|-------------|
| `printf` | ✅ | ❌ Use `printk` |
| `malloc` | ✅ | ❌ Use `kmalloc`, `kzalloc` |
| `sleep` | ✅ | ❌ Use `msleep`, `usleep_range` |
| `float/double` | ✅ | ❌ No FPU in kernel (use fixed-point) |
| Stack size | ~8 MB | ~8-16 KB (very small!) |
| Crash | Process dies | Kernel panic |

---

## 🔹 5. Character Device Drivers

### The Concept

In Linux, "everything is a file." Hardware devices are accessed through special files in `/dev/`. A **character device** transfers data byte-by-byte (serial port, keyboard, sensor). A **block device** transfers in blocks (disk, SSD).

### Character Device Driver Template

```c
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mydev"
#define BUF_SIZE 1024

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *my_class;
static char kernel_buffer[BUF_SIZE];
static int data_size = 0;

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf,
                        size_t count, loff_t *offset) {
    int bytes_to_read = min((int)count, data_size - (int)*offset);
    if (bytes_to_read <= 0) return 0;

    // copy_to_user: SAFE way to copy data from kernel to user space
    if (copy_to_user(buf, kernel_buffer + *offset, bytes_to_read))
        return -EFAULT;

    *offset += bytes_to_read;
    return bytes_to_read;
}

static ssize_t my_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *offset) {
    int bytes_to_write = min((int)count, BUF_SIZE);

    // copy_from_user: SAFE way to copy data from user to kernel space
    if (copy_from_user(kernel_buffer, buf, bytes_to_write))
        return -EFAULT;

    data_size = bytes_to_write;
    return bytes_to_write;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case 0x01: data_size = 0; break;         // Clear buffer
        case 0x02: return data_size;              // Get buffer size
        default: return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .open           = my_open,
    .release        = my_release,
    .read           = my_read,
    .write          = my_write,
    .unlocked_ioctl = my_ioctl,
};

static int __init my_init(void) {
    // 1. Allocate device number
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    // 2. Initialize cdev
    cdev_init(&my_cdev, &fops);
    cdev_add(&my_cdev, dev_num, 1);

    // 3. Create device class and device node (/dev/mydev)
    my_class = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(my_class, NULL, dev_num, NULL, DEVICE_NAME);

    printk(KERN_INFO "Device registered: /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit my_exit(void) {
    device_destroy(my_class, dev_num);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Device unregistered\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
```

### Using the Driver from User Space

```bash
# After loading the module:
echo "Hello Kernel" > /dev/mydev      # write()
cat /dev/mydev                         # read() → "Hello Kernel"
```

```c
// Or from C code:
int fd = open("/dev/mydev", O_RDWR);
write(fd, "data", 4);

char buf[64];
read(fd, buf, sizeof(buf));

ioctl(fd, 0x01, 0);    // Clear buffer command
close(fd);
```

### Why `copy_to_user` / `copy_from_user`?

```
NEVER directly dereference user-space pointers in kernel!

char __user *user_buf;     // Points to user-space memory
kernel_buffer[0] = *user_buf;    // ❌ DANGEROUS! Could be:
                                  //    - Invalid address → kernel crash
                                  //    - Malicious address → security hole

copy_from_user(kernel_buffer, user_buf, n);  // ✅ SAFE
// Validates the pointer, handles page faults gracefully
```

---

## 🔹 6. Platform Drivers & Device Tree

### The Concept

On embedded systems (ARM), hardware isn't discoverable like PCIe. The kernel needs to be **told** what hardware exists. This is done via the **Device Tree** — a data structure describing the hardware.

### Device Tree Example

```dts
// my_board.dts
/ {
    compatible = "qualcomm,sm8550";

    soc {
        #address-cells = <2>;
        #size-cells = <2>;

        // I2C controller
        i2c@a94000 {
            compatible = "qcom,geni-i2c";
            reg = <0x0 0xa94000 0x0 0x4000>;
            interrupts = <GIC_SPI 358 IRQ_TYPE_LEVEL_HIGH>;
            clock-frequency = <400000>;
            status = "okay";

            // Temperature sensor on I2C bus
            temp_sensor@48 {
                compatible = "ti,tmp102";
                reg = <0x48>;           // I2C address
            };
        };

        // UART
        serial@a84000 {
            compatible = "qcom,geni-uart";
            reg = <0x0 0xa84000 0x0 0x4000>;
            interrupts = <GIC_SPI 354 IRQ_TYPE_LEVEL_HIGH>;
            status = "okay";
        };
    };
};
```

### Platform Driver (Matches Device Tree)

```c
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int my_probe(struct platform_device *pdev) {
    struct resource *res;

    // Get memory resource from device tree 'reg' property
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    void __iomem *base = devm_ioremap_resource(&pdev->dev, res);

    // Get interrupt number from device tree
    int irq = platform_get_irq(pdev, 0);

    dev_info(&pdev->dev, "Probed! Base: %p, IRQ: %d\n", base, irq);
    return 0;
}

static int my_remove(struct platform_device *pdev) {
    dev_info(&pdev->dev, "Removed!\n");
    return 0;
}

// Match table — links driver to device tree nodes
static const struct of_device_id my_match[] = {
    { .compatible = "mycompany,mydevice" },
    { },
};
MODULE_DEVICE_TABLE(of, my_match);

static struct platform_driver my_driver = {
    .probe  = my_probe,
    .remove = my_remove,
    .driver = {
        .name = "my_driver",
        .of_match_table = my_match,
    },
};

module_platform_driver(my_driver);
MODULE_LICENSE("GPL");
```

### How Device Tree + Driver Matching Works

```
Boot:
  1. Bootloader passes DTB (compiled device tree) to kernel
  2. Kernel parses all nodes
  3. For each node with "compatible" property:
     → Kernel searches all registered drivers for a matching compatible string
     → If match found → calls driver's probe() function
     → probe() initializes the hardware

        Device Tree Node                    Driver
    compatible = "ti,tmp102"    ←match→  .compatible = "ti,tmp102"
                                            ↓
                                       probe() called
```

---

## 🔹 7. Interrupts in Linux

### Top Half vs Bottom Half

```
Hardware IRQ fires
      │
      ↓
┌──────────────────────────────────┐
│  TOP HALF (Hardirq Handler)       │  ← Runs IMMEDIATELY
│  - Acknowledge interrupt           │  ← Interrupts disabled
│  - Read essential data             │  ← Must be FAST (microseconds)
│  - Schedule bottom half            │  ← Cannot sleep, block, or allocate
│  - Return IRQ_HANDLED              │
└──────────────┬───────────────────┘
               │ schedule
               ↓
┌──────────────────────────────────┐
│  BOTTOM HALF (Deferred Work)      │  ← Runs later, when safe
│  - Process the data               │  ← Can take longer
│  - Wake up waiting processes       │
└──────────────────────────────────┘
```

### Bottom Half Mechanisms

| Mechanism | Context | Can Sleep? | Use Case |
|-----------|---------|:----------:|----------|
| **Softirq** | Interrupt | ❌ | Network, block I/O (built-in) |
| **Tasklet** | Interrupt | ❌ | Simple deferred work |
| **Workqueue** | Process | ✅ | Complex work needing sleep |
| **Threaded IRQ** | Process | ✅ | Modern preferred approach |

### Requesting an Interrupt

```c
// Old way — top half + tasklet bottom half
static irqreturn_t my_isr(int irq, void *dev_id) {
    // Top half — fast, minimal work
    data = readl(reg);
    tasklet_schedule(&my_tasklet);
    return IRQ_HANDLED;
}

// Modern way — threaded IRQ (preferred)
static irqreturn_t my_hardirq(int irq, void *dev_id) {
    return IRQ_WAKE_THREAD;    // Just wake the thread
}

static irqreturn_t my_thread(int irq, void *dev_id) {
    // Thread handler — can sleep, take mutex, do heavy work
    process_data();
    return IRQ_HANDLED;
}

// Register in probe()
ret = devm_request_threaded_irq(&pdev->dev, irq,
        my_hardirq,    // Top half (optional, can be NULL)
        my_thread,     // Thread handler
        IRQF_ONESHOT,
        "my_device", dev);
```

---

## 🔹 8. Kernel Synchronization

### Synchronization Primitives

| Primitive | Context | Can Sleep? | Use Case |
|-----------|---------|:----------:|----------|
| **Spinlock** | Any | ❌ | Short critical sections, interrupt context |
| **Mutex** | Process | ✅ | Longer critical sections |
| **Semaphore** | Process | ✅ | Counting resource access |
| **RW Lock** | Any | ❌ | Many readers, few writers |
| **RCU** | Any | Special | Read-heavy, rare updates |
| **Atomic** | Any | N/A | Single variable operations |

### Spinlock

```c
#include <linux/spinlock.h>

static DEFINE_SPINLOCK(my_lock);

// In process context:
spin_lock(&my_lock);
// critical section
spin_unlock(&my_lock);

// If shared with interrupt handler:
unsigned long flags;
spin_lock_irqsave(&my_lock, flags);     // Saves IRQ state + disables IRQ
// critical section
spin_unlock_irqrestore(&my_lock, flags); // Restores IRQ state
```

### Mutex

```c
#include <linux/mutex.h>

static DEFINE_MUTEX(my_mutex);

mutex_lock(&my_mutex);      // Sleeps if locked (CANNOT use in interrupt!)
// critical section (can be long, can sleep)
mutex_unlock(&my_mutex);

// Trylock (non-blocking)
if (mutex_trylock(&my_mutex)) {
    // got the lock
    mutex_unlock(&my_mutex);
}
```

### When to Use What

```
In interrupt context (ISR, tasklet, softirq)?
  → Spinlock (spin_lock_irqsave)
  → NEVER mutex (can't sleep in interrupt!)

In process context, short critical section?
  → Spinlock (less overhead)

In process context, long or sleeping critical section?
  → Mutex

Read-heavy, write-rare?
  → RCU (fastest reads, lock-free)
```

---

## 🔹 9. Memory in the Kernel

### Allocation Functions

```c
// kmalloc — physically contiguous, like malloc
void *p = kmalloc(size, GFP_KERNEL);    // May sleep
void *p = kmalloc(size, GFP_ATOMIC);    // Won't sleep (for ISR context)
kfree(p);

// kzalloc — like kmalloc but zeroed
void *p = kzalloc(size, GFP_KERNEL);

// vmalloc — virtually contiguous, physically may not be
// Use for large allocations where physical contiguity isn't needed
void *p = vmalloc(large_size);
vfree(p);

// devm_* — device-managed (auto-freed when device is removed)
void *p = devm_kzalloc(&pdev->dev, size, GFP_KERNEL);
// No need to free! Kernel frees when device/driver is unbound.
```

### GFP Flags

| Flag | Meaning | Use In |
|------|---------|--------|
| `GFP_KERNEL` | Normal allocation, may sleep | Process context |
| `GFP_ATOMIC` | Won't sleep, uses emergency pool | Interrupt context |
| `GFP_DMA` | Allocate in DMA-capable memory | DMA transfers |
| `GFP_NOWAIT` | Don't sleep but don't use emergency pool | When sleep is undesirable |

### Memory Mapping

```c
// Map physical address to kernel virtual address
void __iomem *base = ioremap(phys_addr, size);
u32 val = readl(base + offset);        // Read 32-bit register
writel(0xFF, base + offset);            // Write 32-bit register
iounmap(base);

// Device-managed version (preferred — auto-unmapped)
void __iomem *base = devm_ioremap_resource(&pdev->dev, res);
```

---

## 🔹 10. Proc & Sysfs

### /proc — Process Information & Kernel Tunables

```bash
cat /proc/cpuinfo          # CPU information
cat /proc/meminfo          # Memory statistics
cat /proc/interrupts       # Interrupt counts per CPU
cat /proc/devices          # Registered char/block devices
cat /proc/modules          # Loaded kernel modules
cat /proc/version          # Kernel version
cat /proc/<pid>/maps       # Memory map of a process
cat /proc/<pid>/status     # Process status
```

### /sys — Device Model & Driver Parameters

```bash
/sys/class/gpio/           # GPIO devices
/sys/class/i2c-dev/        # I2C devices
/sys/bus/platform/drivers/ # Platform drivers
/sys/devices/              # Hardware topology

# Example: Read CPU frequency
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq

# Example: Control LED
echo 1 > /sys/class/leds/led0/brightness
echo 0 > /sys/class/leds/led0/brightness
```

### Creating a Sysfs Attribute in a Driver

```c
static ssize_t my_show(struct device *dev, struct device_attribute *attr,
                        char *buf) {
    return sprintf(buf, "%d\n", my_value);
}

static ssize_t my_store(struct device *dev, struct device_attribute *attr,
                         const char *buf, size_t count) {
    sscanf(buf, "%d", &my_value);
    return count;
}

static DEVICE_ATTR(my_param, 0644, my_show, my_store);

// In probe():
device_create_file(&pdev->dev, &dev_attr_my_param);

// Usage:
// cat /sys/devices/.../my_param      → reads my_value
// echo 42 > /sys/devices/.../my_param → sets my_value = 42
```

---

## 🔹 11. Linux Boot Process

```
┌───────────────────────────────────────────────────────────────┐
│ 1. POWER ON                                                    │
│    → CPU starts at reset vector (ROM bootloader)               │
├───────────────────────────────────────────────────────────────┤
│ 2. BOOTLOADER (U-Boot / ABL on Qualcomm)                       │
│    → Initialize DRAM, load kernel + DTB into memory            │
│    → Pass kernel command line and DTB address                  │
├───────────────────────────────────────────────────────────────┤
│ 3. KERNEL EARLY BOOT                                           │
│    → Decompress kernel image                                   │
│    → Setup page tables, enable MMU                             │
│    → Parse device tree (DTB)                                   │
│    → Initialize scheduler, memory manager                     │
├───────────────────────────────────────────────────────────────┤
│ 4. KERNEL INIT                                                 │
│    → Register all built-in drivers                             │
│    → Probe devices (match device tree + drivers)               │
│    → Mount root filesystem                                     │
│    → Execute /sbin/init (PID 1)                                │
├───────────────────────────────────────────────────────────────┤
│ 5. INIT SYSTEM (systemd / init)                                │
│    → Start services: networking, logging, daemons              │
│    → Reach target runlevel                                     │
│    → Launch login prompt or GUI                                │
└───────────────────────────────────────────────────────────────┘
```

### Qualcomm-Specific Boot

```
Qualcomm Snapdragon boot chain:

PBL (Primary Bootloader) — ROM, non-modifiable
  ↓ Loads & verifies
XBL (eXtensible Bootloader) — UEFI-based
  ↓ Initializes DRAM, storage
ABL (Android Bootloader) / U-Boot
  ↓ Loads kernel + DTB + ramdisk
Linux Kernel
  ↓ Mounts rootfs
Android / Linux userspace
```

Each stage **verifies the signature** of the next stage — this is **Secure Boot** (uses TrustZone).

---

## 🔹 12. Debugging the Kernel

### Essential Tools

```bash
# Kernel log (ring buffer)
dmesg                              # View all kernel messages
dmesg -w                           # Follow (like tail -f)
dmesg -l err,warn                  # Filter by level

# printk levels
printk(KERN_EMERG   "...\n");     // 0 — System unusable
printk(KERN_ALERT   "...\n");     // 1 — Immediate action needed
printk(KERN_CRIT    "...\n");     // 2 — Critical
printk(KERN_ERR     "...\n");     // 3 — Error
printk(KERN_WARNING "...\n");     // 4 — Warning
printk(KERN_NOTICE  "...\n");     // 5 — Normal but significant
printk(KERN_INFO    "...\n");     // 6 — Informational
printk(KERN_DEBUG   "...\n");     // 7 — Debug

# Dynamic debug — enable/disable debug prints at runtime
echo 'module my_driver +p' > /sys/kernel/debug/dynamic_debug/control
```

### Kernel Panic / Oops

```
When the kernel crashes, it prints an "Oops" or "Panic":

[  123.456] Unable to handle kernel NULL pointer dereference at virtual address 00000000
[  123.456] PC is at my_function+0x24/0x80 [my_module]
[  123.456] Call trace:
[  123.456]  my_function+0x24/0x80 [my_module]
[  123.456]  my_probe+0x44/0x60 [my_module]
[  123.456]  platform_drv_probe+0x54/0xa8

Use addr2line to find the exact source line:
  addr2line -e my_module.ko 0x24
```

### Ftrace — Kernel Function Tracer

```bash
# Trace all function calls
echo function > /sys/kernel/debug/tracing/current_tracer
echo 1 > /sys/kernel/debug/tracing/tracing_on
cat /sys/kernel/debug/tracing/trace

# Trace specific functions
echo my_probe > /sys/kernel/debug/tracing/set_ftrace_filter
```

---

## 🔹 13. Interview Questions

---

### Q1. What is the difference between a character device and a block device?

**A:** Character devices transfer data byte-by-byte, are not buffered by the kernel, and don't support seeking (e.g., serial port, keyboard, `/dev/null`). Block devices transfer data in fixed-size blocks, are buffered by the page cache, and support random access/seeking (e.g., hard disk, SSD, `/dev/sda`).

---

### Q2. What is `ioctl`? When would you use it?

**A:** `ioctl` (I/O control) handles device-specific commands that don't fit into the read/write model. For example: setting baud rate on a UART, querying sensor calibration data, resetting a device, or configuring DMA channels. The driver defines custom command numbers, and user space calls `ioctl(fd, cmd, arg)`.

---

### Q3. Why can't you use `mutex_lock` in interrupt context?

**A:** `mutex_lock` may sleep (put the calling thread to sleep if the lock is held). In interrupt context, there's no "thread" to put to sleep — you're in a special context that must complete quickly. Sleeping would deadlock the system. Use `spin_lock_irqsave` instead.

---

### Q4. What is `copy_to_user` / `copy_from_user`? Why not use `memcpy`?

**A:** User-space pointers may be invalid, unmapped, or malicious. `memcpy` would crash the kernel on bad pointers. `copy_to_user`/`copy_from_user` verify the address range, handle page faults gracefully, and return an error code instead of crashing. This is a critical **security boundary**.

---

### Q5. Explain the device tree. Why is it needed?

**A:** ARM hardware isn't self-describing (unlike PCIe which has vendor/device IDs). The device tree (`.dts` files compiled to `.dtb`) describes all hardware: addresses, interrupts, clocks, GPIOs, I2C addresses. The kernel parses it at boot and matches nodes to drivers via the `compatible` property. This separates **hardware description** from **driver code**.

---

### Q6. What is `devm_` prefix in kernel APIs?

**A:** `devm_` stands for **device-managed**. Resources allocated with `devm_` functions (e.g., `devm_kzalloc`, `devm_ioremap`, `devm_request_irq`) are automatically freed when the device is removed or the driver is unbound. This prevents resource leaks and simplifies error handling — you don't need explicit cleanup code.

---

### Q7. What is the difference between `insmod` and `modprobe`?

**A:** `insmod` loads a single module file. If it depends on other modules, it fails. `modprobe` reads `/lib/modules/.../modules.dep` and automatically loads all dependencies first. Always prefer `modprobe` in production.

---

### Q8. How would a user-space application communicate with a kernel driver?

**A:**
1. **File operations** — `open`/`read`/`write`/`close` on `/dev/` nodes
2. **ioctl** — custom commands via `ioctl()`
3. **sysfs** — read/write attributes in `/sys/`
4. **procfs** — read/write entries in `/proc/`
5. **mmap** — map device memory directly into user space
6. **netlink** — socket-based kernel-to-user messaging

---

### Q9. What is a kernel panic vs a kernel oops?

**A:** An **oops** is a non-fatal error — the kernel logs the error, kills the offending process, and tries to continue. A **panic** is fatal — the kernel cannot continue safely (corruption, deadlock in critical path) and halts. Some oopses escalate to panics if they occur in interrupt context or critical code paths.

---

### Q10. Explain the difference between `kmalloc` and `vmalloc`.

**A:** `kmalloc` allocates physically contiguous memory (needed for DMA). It's fast but limited to small allocations (~4MB max). `vmalloc` allocates virtually contiguous but physically scattered pages. Good for large allocations but can't be used for DMA and has higher overhead due to page table setup.

---

<p align="center">
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong><br/>
  <br/>
  <em>🎓 Linux Kernel & Drivers — From Module to Device Tree</em><br/>
  <br/>
  <strong>━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━</strong>
</p>
