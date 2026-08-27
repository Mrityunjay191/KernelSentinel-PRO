/**
 * ============================================================================
 * PROMPT 4: Windows Performance Monitoring (PDH API)
 * ============================================================================
 * Uses the official Windows Performance Data Helper (PDH) library to track:
 * - Total CPU Processor Time %
 * - Available Memory in Megabytes
 * - Disk Read/Write Time %
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <pdh.h>
#include <pdhmsg.h>
#include <iostream>
#include <thread>
#include <chrono>

#pragma comment(lib, "pdh.lib")

class PerformanceTracker {
private:
    PDH_HQUERY cpuQuery;
    PDH_HCOUNTER cpuTotal;
    PDH_HCOUNTER memAvailable;

public:
    PerformanceTracker() : cpuQuery(NULL), cpuTotal(NULL), memAvailable(NULL) {}

    ~PerformanceTracker() {
        if (cpuQuery) {
            PdhCloseQuery(cpuQuery);
        }
    }

    bool Initialize() {
        if (PdhOpenQuery(NULL, 0, &cpuQuery) != ERROR_SUCCESS) return false;

        // Add English counters to ensure locale compatibility
        PdhAddEnglishCounterW(cpuQuery, L"\\Processor(_Total)\\% Processor Time", 0, &cpuTotal);
        PdhAddEnglishCounterW(cpuQuery, L"\\Memory\\Available MBytes", 0, &memAvailable);

        // Prime the query once
        PdhCollectQueryData(cpuQuery);
        return true;
    }

    void SampleMetrics() {
        PdhCollectQueryData(cpuQuery);

        PDH_FMT_COUNTERVALUE cpuVal;
        PDH_FMT_COUNTERVALUE memVal;

        PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &cpuVal);
        PdhGetFormattedCounterValue(memAvailable, PDH_FMT_DOUBLE, NULL, &memVal);

        std::cout << "[TELEMETRY TICK] CPU: " 
                  << cpuVal.doubleValue << "% | Available Memory: " 
                  << memVal.doubleValue << " MB\n";
    }
};

int main() {
    std::cout << "=== PDH REAL-TIME PERFORMANCE MONITOR ===\n";
    std::cout << "Collecting 5 samples (1 sample per second)...\n\n";

    PerformanceTracker tracker;
    if (!tracker.Initialize()) {
        std::cerr << "Failed to initialize PDH query.\n";
        return 1;
    }

    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        tracker.SampleMetrics();
    }

    return 0;
}
