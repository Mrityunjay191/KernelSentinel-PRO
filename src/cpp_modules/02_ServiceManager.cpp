/**
 * ============================================================================
 * PROMPT 2: Windows Service Management (Win32 API)
 * ============================================================================
 * Demonstrates safe, legitimate interaction with the Windows Service Control
 * Manager (SCM) to:
 * - Enumerate active and inactive Windows services
 * - Query individual service status (Running, Stopped, Paused)
 * - Safe Start/Stop requests requiring appropriate permissions
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsvc.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "advapi32.lib")

class ServiceController {
public:
    struct ServiceEntry {
        std::wstring serviceName;
        std::wstring displayName;
        DWORD currentState;
    };

    static std::wstring StateToString(DWORD state) {
        switch (state) {
            case SERVICE_STOPPED: return L"STOPPED";
            case SERVICE_START_PENDING: return L"START_PENDING";
            case SERVICE_STOP_PENDING: return L"STOP_PENDING";
            case SERVICE_RUNNING: return L"RUNNING";
            case SERVICE_CONTINUE_PENDING: return L"CONTINUE_PENDING";
            case SERVICE_PAUSE_PENDING: return L"PAUSE_PENDING";
            case SERVICE_PAUSED: return L"PAUSED";
            default: return L"UNKNOWN";
        }
    }

    static std::vector<ServiceEntry> EnumerateAllServices() {
        std::vector<ServiceEntry> services;

        SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
        if (!hSCM) {
            std::wcerr << L"[Error] Failed to open Service Control Manager. Error code: " << GetLastError() << L"\n";
            return services;
        }

        DWORD bytesNeeded = 0;
        DWORD servicesReturned = 0;
        DWORD resumeHandle = 0;

        EnumServicesStatusExW(
            hSCM,
            SC_ENUM_PROCESS_INFO,
            SERVICE_WIN32,
            SERVICE_STATE_ALL,
            NULL,
            0,
            &bytesNeeded,
            &servicesReturned,
            &resumeHandle,
            NULL
        );

        if (bytesNeeded > 0) {
            std::vector<BYTE> buffer(bytesNeeded);
            if (EnumServicesStatusExW(
                    hSCM,
                    SC_ENUM_PROCESS_INFO,
                    SERVICE_WIN32,
                    SERVICE_STATE_ALL,
                    buffer.data(),
                    bytesNeeded,
                    &bytesNeeded,
                    &servicesReturned,
                    &resumeHandle,
                    NULL)) {
                
                auto* pServices = reinterpret_cast<ENUM_SERVICE_STATUS_PROCESSW*>(buffer.data());
                for (DWORD i = 0; i < servicesReturned; ++i) {
                    services.push_back({
                        pServices[i].lpServiceName,
                        pServices[i].lpDisplayName,
                        pServices[i].ServiceStatusProcess.dwCurrentState
                    });
                }
            }
        }

        CloseServiceHandle(hSCM);
        return services;
    }

    static bool StartTargetService(const std::wstring& serviceName) {
        SC_HANDLE hSCM = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
        if (!hSCM) return false;

        SC_HANDLE hService = OpenServiceW(hSCM, serviceName.c_str(), SERVICE_START);
        if (!hService) {
            CloseServiceHandle(hSCM);
            return false;
        }

        BOOL success = StartServiceW(hService, 0, NULL);
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCM);
        return (success != 0);
    }
};

int main() {
    std::wcout << L"=== WINDOWS SERVICE MANAGEMENT DEMO ===\n";
    std::wcout << L"Fetching service states from SCM...\n\n";

    auto services = ServiceController::EnumerateAllServices();
    std::wcout << L"Found " << services.size() << L" installed services.\n";
    std::wcout << L"Listing sample first 10 services:\n";
    std::wcout << L"----------------------------------------------------------------------\n";

    size_t displayCount = (services.size() < 10) ? services.size() : 10;
    for (size_t i = 0; i < displayCount; ++i) {
        std::wcout << L"[" << ServiceController::StateToString(services[i].currentState) << L"]\t"
                   << services[i].serviceName << L" (" << services[i].displayName << L")\n";
    }

    std::wcout << L"----------------------------------------------------------------------\n";
    return 0;
}
