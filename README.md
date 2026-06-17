![Language](https://img.shields.io/badge/Language-C++-blue.svg)
![Core](https://img.shields.io/badge/Core-Windows_Internals-red.svg)

---

## 📂 Memory-Mapped Files (Shared Memory) IPC

### 📝 Description
By default, Windows strictly isolates process memory to prevent applications from crashing one another. However, enterprise systems (like high-frequency trading components or anti-cheat engines) often require instantaneous data sharing. Traditional methods like network sockets or disk I/O are too slow. The absolute fastest way to achieve Inter-Process Communication (IPC) is through Memory-Mapped Files. This mechanism asks the Windows Kernel to take a single page of physical RAM and map it into the Virtual Address Space of multiple distinct processes simultaneously, enabling incredibly fast, zero-copy data sharing.

### 🎯 Objective
To practically demonstrate enterprise-grade Inter-Process Communication (IPC) by creating a named shared memory object in a "Writer" process and successfully intercepting its raw physical RAM contents from a completely isolated "Reader" process.

### 🧠 Major Underlying Concepts
* **Memory-Mapped IPC:** Bypassing strict process isolation by mapping the exact same physical memory pages into multiple virtual address spaces, meaning a write in Process A is instantly visible to Process B.
* **Reserve vs. Commit:** Understanding that memory allocation with `MEM_RESERVE` merely claims addresses in the Kernel's VAD tree (like placing a "Reserved" sign on a table without placing food), while `MEM_COMMIT` instantly assigns lightning-fast physical RAM hardware to those virtual addresses.
* **The Role of `pagefile.sys`:** The paging file is not just an address directory; it stores actual raw data. It acts as a physical extension of RAM (a "spillover tank"). The OS Memory Manager constantly shuffles real data between Physical RAM (for speed) and `pagefile.sys` (for overflow), triggering hardware Page Faults to seamlessly retrieve swapped data.
* **Paging-File-Backed Memory:** By passing `INVALID_HANDLE_VALUE` to the `CreateFileMappingW` API, we explicitly instruct the OS to map pure physical RAM (backed by `pagefile.sys` to prevent system crashes) rather than writing to a physical text file on the hard drive.

### 🛠️ Tool Usage & Physical Justification
* **Tool Used:** Dual isolated user-mode processes (`shared_writer.exe` and `shared_reader.exe`)
* **What to Look For:** Run the Writer process, which requests a shared RAM chunk named `Local\MySharedMemory`, writes a secret message into it, and pauses. While the Writer remains open (keeping the memory allocated), execute the Reader process.
* **Physical Justification:** The Reader process instantly outputs: `"Hello from the Writer Process! This is instant shared RAM."` Although Process A and Process B have completely isolated Virtual Address Spaces, the output proves the Windows Memory Manager successfully bridged them by pointing both of their internal VAD tree nodes to the exact same physical RAM page.

---
