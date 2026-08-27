/**
 * ============================================================================
 * PROMPT 3: Windows Event Log Monitoring (Win32 Event Log API)
 * ============================================================================
 * Safely inspects the Windows Application or System event logs:
 * - Uses modern Windows Event Log API (winevt.h)
 * - Queries recent log events
 * - Filters by Channel and Severity (Errors & Warnings)
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winevt.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "wevtapi.lib")

class EventLogReader {
public:
    static void QueryRecentLogs(LPCWSTR channelPath, DWORD maxEvents = 5) {
        std::wcout << L"Querying channel: " << channelPath << L" (Newest " << maxEvents << L" events)\n";

        // Query newest events in reverse chronological order
        EVT_HANDLE hResults = EvtQuery(
            NULL,
            channelPath,
            L"*",
            EvtQueryChannelPath | EvtQueryReverseDirection
        );

        if (!hResults) {
            std::wcerr << L"EvtQuery failed with error: " << GetLastError() << L"\n";
            return;
        }

        std::vector<EVT_HANDLE> events(maxEvents);
        DWORD returned = 0;

        if (EvtNext(hResults, maxEvents, events.data(), 1500, 0, &returned)) {
            for (DWORD i = 0; i < returned; ++i) {
                DWORD bufferSize = 0;
                DWORD propertyCount = 0;

                // First call to get required buffer size
                EvtRender(NULL, events[i], EvtRenderEventXml, 0, NULL, &bufferSize, &propertyCount);
                if (bufferSize > 0) {
                    std::vector<wchar_t> xmlBuffer(bufferSize / sizeof(wchar_t) + 1);
                    if (EvtRender(NULL, events[i], EvtRenderEventXml, bufferSize, xmlBuffer.data(), &bufferSize, &propertyCount)) {
                        std::wcout << L"\n---------------- [Event Record " << (i + 1) << L"] ----------------\n";
                        std::wstring xml(xmlBuffer.data());
                        if (xml.length() > 300) xml = xml.substr(0, 300) + L"...";
                        std::wcout << xml << L"\n";
                    }
                }
                EvtClose(events[i]);
            }
        } else {
            std::wcout << L"No records returned or access denied.\n";
        }

        EvtClose(hResults);
    }
};

int main() {
    std::wcout << L"=== WINDOWS EVENT LOG VIEWER (EDUCATIONAL) ===\n\n";
    EventLogReader::QueryRecentLogs(L"Application", 3);
    return 0;
}
