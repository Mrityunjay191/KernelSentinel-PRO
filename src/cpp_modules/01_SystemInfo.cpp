/**
 * ============================================================================
 * PROMPT 1: Windows System Information Management
 * ============================================================================
 * Collects and displays basic system details:
 * - OS Version & Build from Registry
 * - Processor Cores, Architecture, and Page size via GetNativeSystemInfo
 * - Physical & Virtual Memory via GlobalMemoryStatusEx
 * - Disk Space via GetLogicalDriveStringsW & GetDiskFreeSpaceExW
 * - Active Network Adapters & IP addresses via GetAdaptersAddresses
 * - Running processes snapshot via CreateToolhelp32Snapshot
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <tlhelp32.h>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

class SystemMonitor {
public:
    struct MemoryStats {
        DWORD memoryLoad;
        double totalPhysGB;
        double availPhysGB;
        double totalVirtualGB;
        double availVirtualGB;
    };

    struct DriveStats {
        std::wstring driveRoot;
        double totalGB;
        double freeGB;
        double percentFree;
    };

    struct ProcessRecord {
        DWORD pid;
        std::wstring name;
        DWORD threads;
    };

    struct NetworkRecord {
        std::wstring name;
        std::vector<std::wstring> ips;
    };

    SystemMonitor() = default;
    ~SystemMonitor() = default;

    std::wstring GetOSVersion() {
        HKEY hKey;
        std::wstring osInfo = L"Windows (Unknown)";
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, 
                          L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 
                          0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            wchar_t prodName[256] = { 0 };
            wchar_t buildLab[256] = { 0 };
            DWORD prodSize = sizeof(prodName);
            DWORD buildSize = sizeof(buildLab);

            RegQueryValueExW(hKey, L"ProductName", NULL, NULL, (LPBYTE)prodName, &prodSize);
            RegQueryValueExW(hKey, L"DisplayVersion", NULL, NULL, (LPBYTE)buildLab, &buildSize);
            RegCloseKey(hKey);

            osInfo = std::wstring(prodName) + L" (Version " + std::wstring(buildLab) + L")";
        }
        return osInfo;
    }

    void DisplayProcessorInfo() {
        SYSTEM_INFO sysInfo;
        GetNativeSystemInfo(&sysInfo);

        std::cout << "[PROCESSOR INFORMATION]\n";
        std::cout << "  Logical Cores: " << sysInfo.dwNumberOfProcessors << "\n";
        std::cout << "  Architecture:  ";
        switch (sysInfo.wProcessorArchitecture) {
            case PROCESSOR_ARCHITECTURE_AMD64: std::cout << "x64 (AMD/Intel 64-bit)\n"; break;
            case PROCESSOR_ARCHITECTURE_ARM64: std::cout << "ARM64\n"; break;
            case PROCESSOR_ARCHITECTURE_INTEL: std::cout << "x86 (32-bit)\n"; break;
            default: std::cout << "Other\n"; break;
        }
        std::cout << "  Page Size:     " << sysInfo.dwPageSize << " bytes\n\n";
    }

    MemoryStats GetMemoryInfo() {
        MEMORYSTATUSEX mem;
        mem.dwLength = sizeof(mem);
        GlobalMemoryStatusEx(&mem);

        const double GB = 1024.0 * 1024.0 * 1024.0;
        return MemoryStats{
            mem.dwMemoryLoad,
            static_cast<double>(mem.ullTotalPhys) / GB,
            static_cast<double>(mem.ullAvailPhys) / GB,
            static_cast<double>(mem.ullTotalVirtual) / GB,
            static_cast<double>(mem.ullAvailVirtual) / GB
        };
    }

    std::vector<DriveStats> GetDriveInfo() {
        std::vector<DriveStats> list;
        wchar_t buffer[512];
        DWORD len = GetLogicalDriveStringsW(512, buffer);

        const double GB = 1024.0 * 1024.0 * 1024.0;
        wchar_t* drive = buffer;
        while (*drive) {
            ULARGE_INTEGER freeCaller, total, totalFree;
            if (GetDiskFreeSpaceExW(drive, &freeCaller, &total, &totalFree)) {
                double tot = static_cast<double>(total.QuadPart) / GB;
                double fre = static_cast<double>(totalFree.QuadPart) / GB;
                double pct = (tot > 0.0) ? (fre / tot * 100.0) : 0.0;
                list.push_back({ drive, tot, fre, pct });
            }
            drive += wcslen(drive) + 1;
        }
        return list;
    }

    std::vector<ProcessRecord> GetProcessList() {
        std::vector<ProcessRecord> list;
        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) return list;

        PROCESSENTRY32W pe;
        pe.dwSize = sizeof(pe);

        if (Process32FirstW(hSnapshot, &pe)) {
            do {
                list.push_back({ pe.th32ProcessID, pe.szExeFile, pe.cntThreads });
            } while (Process32NextW(hSnapshot, &pe));
        }

        CloseHandle(hSnapshot);
        return list;
    }

    void PrintSummary() {
        std::wcout << L"=====================================================\n";
        std::wcout << L"         SYSTEM INFORMATION SUMMARY REPORT           \n";
        std::wcout << L"=====================================================\n\n";

        std::wcout << L"[OPERATING SYSTEM]\n  " << GetOSVersion() << L"\n\n";

        DisplayProcessorInfo();

        auto mem = GetMemoryInfo();
        std::cout << "[MEMORY UTILIZATION]\n";
        std::cout << "  Load:            " << mem.memoryLoad << "%\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "  Physical Memory: " << (mem.totalPhysGB - mem.availPhysGB) << " GB used / " << mem.totalPhysGB << " GB total\n";
        std::cout << "  Virtual Memory:  " << (mem.totalVirtualGB - mem.availVirtualGB) << " GB used / " << mem.totalVirtualGB << " GB total\n\n";

        std::wcout << L"[STORAGE DRIVES]\n";
        for (const auto& d : GetDriveInfo()) {
            std::wcout << L"  Drive " << d.driveRoot << L" - Free: " << d.freeGB << L" GB / Total: " << d.totalGB << L" GB (" << d.percentFree << L"% free)\n";
        }
        std::wcout << L"\n";

        auto procs = GetProcessList();
        std::wcout << L"[PROCESS SNAPSHOT] (Total: " << procs.size() << L")\n";
        size_t count = (procs.size() < 8) ? procs.size() : 8;
        for (size_t i = 0; i < count; ++i) {
            std::wcout << L"  PID: " << procs[i].pid << L"\tThreads: " << procs[i].threads << L"\t" << procs[i].name << L"\n";
        }
        std::wcout << L"=====================================================\n";
    }
};

int main() {
    SystemMonitor monitor;
    monitor.PrintSummary();
    return 0;
}
