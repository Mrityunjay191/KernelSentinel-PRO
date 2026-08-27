# 🛡️ KernelSentinel PRO

> **A blazing fast, low-level Windows kernel monitor, game ping tester & 1-click system cleaner.**
> Built for competitive gamers, streamers, and hardware enthusiasts who want zero lag and deep insight into their PC.

---

## ⚡ What is KernelSentinel PRO?

Most system monitors on Windows are bloated, slow, and only show surface-level numbers.

**KernelSentinel PRO** gives you direct, low-level access to what Windows and your hardware are actually doing in real-time. It tracks **Kernel Mode vs User Mode CPU**, **Non-Paged Physical RAM pools**, **DPC Latency**, **Context Switches**, **20+ Game & Streaming Server Pings**, and includes a **7-stage 1-click Deep Cleaner** that wipes DirectX shaders, temporary junk, and standby RAM.

Everything is packed into **two standalone portable `.exe` tools** with zero setup, zero dependencies, and zero login walls.

---

## 🚀 Quick Start (How to Run)

Open your `KernelSentinel PRO` folder and pick how you want to use it:

### 1. 🌐 The Web Dashboard (`KernelSentinelPRO.exe`)
Double-click **`KernelSentinelPRO.exe`**
- It boots a lightweight background engine and automatically opens your browser at:
  👉 **`http://127.0.0.1:8080`**
- Features a dark glassmorphic UI, 60 FPS live charts, 9 detailed tabs, and 1-click optimizer buttons.

### 2. 💻 The Terminal Sentinel (`KernelMonitor.exe`)
Double-click **`KernelMonitor.exe`**
- Opens a clean, retro-cyber live terminal window.
- Updates every **1 second** with ASCII progress bars, real-time metrics, active running apps, deleted items history, and instant keyboard shortcuts.

---

## 🎯 Key Features

### 1. 🛡️ Deep Kernel Monitoring (Ring 0 Level)
- **Kernel CPU vs User CPU %**: See exactly how much CPU time is spent inside Windows kernel drivers versus your actual games/apps.
- **Non-Paged Pool (NPP)**: Tracks locked physical RAM that Windows can never swap to disk (driver memory).
- **Paged Pool (PP)**: Swappable virtual kernel memory.
- **DPC / IRQ Latency**: Monitors interrupt processing delay (keeps DPC under 1% to prevent audio crackles and frame drops).
- **Context Switches & Syscalls**: Live velocity of CPU thread switching and kernel system calls.
- **Loaded `.sys` Drivers Explorer**: Filter drivers by Graphics, Audio, Network, Security & Anti-Cheat.

### 2. ⏱️ Exact Task Manager Up Time Sync
- Displays your exact PC uptime matching Windows Task Manager (`0:09:28:15` format) and ticks live every second.
- Shows total Handles, Threads, Processes, and CPU Boost Clocks (4.12 GHz).

### 3. 🧹 1-Click Multi-Stage Deep Cleaner
Click **"Clean Cache"** on the website or press **`[C]`** in the terminal to instantly run:
- ✅ **AMD & DirectX Shader Caches**: Purges `AMD\DxCache`, `D3DSCache`, and `FontCache`.
- ✅ **Windows Temp & Prefetch**: Cleans `%TEMP%` and `C:\Windows\Temp`.
- ✅ **Clipboard Wipe**: Neutralizes lingering clipboard data.
- ✅ **DNS Resolver Flush**: Clears Windows DNS cache for faster networking.
- ✅ **Empty Recycle Bin**: Permanently empties deleted files.
- ✅ **Process RAM Trimming**: Calls Win32 `EmptyWorkingSet` to purge standby memory for active apps.

### 4. 🎮 Games & Streaming Ingest Ping Matrix
Live roundtrip latency benchmarks to:
- **Games**: Valorant (Riot), CS2 (Steam), BGMI, Free Fire, Fortnite, GTA Online, Apex Legends, Roblox, Minecraft.
- **Streaming**: Twitch (US & Asia), YouTube RTMP, Kick.com, Discord Gateway, Facebook Gaming.
- **DNS Resolvers**: Cloudflare (1.1.1.1), Google (8.8.8.8), OpenDNS, Quad9.

### 5. 🗃️ Registry Software & Deletion History Auditor
- View all software registered in the Windows Registry (`HKLM`/`HKCU`).
- Live Recycle Bin audit showing deleted files, origin paths, and uninstalled MSI records.

---

## 🎮 Keyboard Controls (`KernelMonitor.exe`)

| Key | Screen / Action | Description |
| :---: | :--- | :--- |
| **`[1]`** | **Live Dashboard** | Shows live CPU modes, NPP/PP RAM pools, DPC latency, and Task Manager uptime. |
| **`[2]`** | **Active Ecosystem** | Shows top apps currently running by RAM (Chrome, Discord, Steam, Vanguard) and driver counts. |
| **`[3]`** | **Deletions Log** | Shows items deleted in the Recycle Bin and uninstallation logs. |
| **`[C]`** | **1-Click Deep Clean** | Purges DirectX shaders, temp junk, flushes DNS, and trims RAM. |
| **`[Q]`** | **Quit** | Cleanly closes the window. |

---

## 📁 Project Folder Layout

```
KernelSentinel PRO/
├── 🚀 KernelSentinelPRO.exe     # 1-Click Web Dashboard & Fast Server
├── 🛡️ KernelMonitor.exe         # 1-Click Pure Native Terminal Kernel Sentinel
├── ⚡ server.ps1                # Low-Latency REST API (<2ms)
├── 📖 README.md                 # Project Documentation
│
├── 📂 src/
│   └── 📂 cpp_modules/          # Pure C++ Native Win32 & Kernel Syscall Source Files
│       ├── KernelMonitor.cpp    # Direct NTDLL Ring 0 Kernel Sentinel Source
│       ├── 01_SystemInfo.cpp    # Win32 SystemInfo & Toolhelp32 snapshot
│       ├── 02_ServiceManager.cpp# SCM Service Control API
│       ├── 03_EventLogReader.cpp# Windows Event Log API query
│       ├── 04_PerformanceMonitor.cpp # PDH CPU & RAM counter engine
│       ├── 05_MemoryPoolAllocator.cpp# O(1) Fixed chunk memory pool
│       ├── 06_InProcessMemoryScanner.cpp # Safe address space scanner
│       ├── 07_AStarPathfinding.cpp   # 2D Grid navigation & obstacle avoidance
│       └── 08_NetworkPingUtility.cpp # ICMP Ping utility
│
└── 📂 web/                      # Modern Dark Glassmorphic 60 FPS Web UI
    ├── index.html               # 9-Tab Dashboard UI
    ├── style.css                # Glassmorphic CSS with animations & neon glows
    ├── app.js                   # 60 FPS Chart.js engine & smart selective poller
    └── 📂 download/             # Public Download Packages
        ├── KernelSentinelPRO.exe
        └── KernelMonitor.exe
```

---

## 🛠️ Compiling C++ Modules

If you want to build or modify the pure C++ kernel module (`src/cpp_modules/KernelMonitor.cpp`):

### With Visual Studio MSVC (`cl`):
```cmd
cd src\cpp_modules
cl /EHsc /O2 /W4 KernelMonitor.cpp /Fe:KernelMonitor.exe psapi.lib pdh.lib dnsapi.lib
```

### With MinGW / GCC (`g++`):
```bash
cd src/cpp_modules
g++ -O2 KernelMonitor.cpp -o KernelMonitor.exe -lpsapi -lpdh -ldnsapi
```

---

## 🌐 Sharing & Portable Use

- **100% Standalone**: You can copy the entire folder to a USB drive or send it as a `.zip` to a friend.
- **No Installers Needed**: Target PCs do not need Node.js, Python, or administrative installers. Just double-click `KernelSentinelPRO.exe` or `KernelMonitor.exe`!

---

<div align="center">
  <sub>KernelSentinel PRO • Built for High-Performance Gaming & Low-Latency Live Streaming • 2026</sub>
</div>
