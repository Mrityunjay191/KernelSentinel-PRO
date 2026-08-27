/**
 * ============================================================================
 * EDUCATIONAL: In-Process Virtual Memory & AOB Signature Scanner
 * ============================================================================
 * Demonstrates how a game engine or debug tool scans its OWN address space:
 * - Uses VirtualQuery to enumerate committed memory pages
 * - Safe read-only inspection (PAGE_READONLY, PAGE_READWRITE)
 * - Supports pattern search with wildcards ('?')
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

class InProcessScanner {
public:
    static std::vector<int> ParsePattern(const std::string& patternStr) {
        std::vector<int> pattern;
        size_t i = 0;
        while (i < patternStr.length()) {
            if (patternStr[i] == ' ') { ++i; continue; }
            if (patternStr[i] == '?') {
                pattern.push_back(-1);
                ++i;
                if (i < patternStr.length() && patternStr[i] == '?') ++i;
            } else {
                pattern.push_back(std::stoi(patternStr.substr(i, 2), nullptr, 16));
                i += 2;
            }
        }
        return pattern;
    }

    static std::vector<uintptr_t> Scan(const std::string& patternStr) {
        std::vector<int> pattern = ParsePattern(patternStr);
        std::vector<uintptr_t> results;
        if (pattern.empty()) return results;

        SYSTEM_INFO sys;
        GetSystemInfo(&sys);

        uintptr_t addr = reinterpret_cast<uintptr_t>(sys.lpMinimumApplicationAddress);
        uintptr_t maxAddr = reinterpret_cast<uintptr_t>(sys.lpMaximumApplicationAddress);
        MEMORY_BASIC_INFORMATION mbi;

        while (addr < maxAddr) {
            if (VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi)) == sizeof(mbi)) {
                bool readable = (mbi.State == MEM_COMMIT) &&
                                !(mbi.Protect & PAGE_GUARD) &&
                                !(mbi.Protect & PAGE_NOACCESS) &&
                                (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE));

                if (readable && mbi.RegionSize > pattern.size()) {
                    const BYTE* ptr = reinterpret_cast<const BYTE*>(mbi.BaseAddress);
                    size_t limit = mbi.RegionSize - pattern.size();

                    for (size_t o = 0; o < limit; ++o) {
                        bool match = true;
                        for (size_t p = 0; p < pattern.size(); ++p) {
                            if (pattern[p] != -1 && pattern[p] != ptr[o + p]) {
                                match = false;
                                break;
                            }
                        }
                        if (match) results.push_back(reinterpret_cast<uintptr_t>(ptr + o));
                    }
                }
                addr = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
            } else {
                addr += sys.dwPageSize;
            }
        }
        return results;
    }
};

// Target test data
static const BYTE g_SampleSecret[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0x11, 0x22 };

int main() {
    std::cout << "=== IN-PROCESS MEMORY SIGNATURE SCANNER (EDUCATIONAL) ===\n";
    std::cout << "Target buffer address: 0x" << std::hex << (uintptr_t)g_SampleSecret << std::dec << "\n\n";

    std::string pattern = "AA BB CC DD ? 22";
    std::cout << "Scanning for: \"" << pattern << "\"...\n";

    auto hits = InProcessScanner::Scan(pattern);
    std::cout << "Found " << hits.size() << " match(es):\n";
    for (auto h : hits) {
        std::cout << "  0x" << std::hex << h << std::dec << "\n";
    }

    return 0;
}
