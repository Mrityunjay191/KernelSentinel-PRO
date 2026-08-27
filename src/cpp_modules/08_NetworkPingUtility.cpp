/**
 * ============================================================================
 * PROMPT 10 (SYS): Network Ping & Connectivity Testing Tool
 * ============================================================================
 * Uses Windows IpHlpApi and ICMP API to ping endpoints:
 * - IcmpCreateFile
 * - IcmpSendEcho
 * - IcmpCloseHandle
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

class NetworkDiagnostic {
public:
    static void PingHost(const char* ipAddressStr) {
        HANDLE hIcmpFile = IcmpCreateFile();
        if (hIcmpFile == INVALID_HANDLE_VALUE) {
            std::cerr << "Unable to open ICMP handle.\n";
            return;
        }

        unsigned long ipaddr = inet_addr(ipAddressStr);
        char sendData[32] = "StreamerPanelPingTestData";
        DWORD replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
        std::vector<BYTE> replyBuffer(replySize);

        DWORD replies = IcmpSendEcho(
            hIcmpFile,
            ipaddr,
            sendData,
            sizeof(sendData),
            NULL,
            replyBuffer.data(),
            replySize,
            1000 // 1s timeout
        );

        if (replies != 0) {
            PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuffer.data();
            std::cout << "[PING " << ipAddressStr << "] Reply from " 
                      << ipAddressStr << ": time=" 
                      << pEchoReply->RoundTripTime << "ms (TTL=" 
                      << (int)pEchoReply->Options.Ttl << ")\n";
        } else {
            std::cout << "[PING " << ipAddressStr << "] Request timed out.\n";
        }

        IcmpCloseHandle(hIcmpFile);
    }
};

int main() {
    std::cout << "=== NETWORK CONNECTIVITY & LATENCY DIAGNOSTIC ===\n\n";

    // Test DNS Resolvers
    NetworkDiagnostic::PingHost("1.1.1.1"); // Cloudflare
    NetworkDiagnostic::PingHost("8.8.8.8"); // Google

    return 0;
}
