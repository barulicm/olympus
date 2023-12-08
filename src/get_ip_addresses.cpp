#include "get_ip_addresses.hpp"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#elif __unix__
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#ifdef _WIN32
std::vector<std::string> GetIpAddresses()
{
    std::vector<std::string> addresses;

    // https://learn.microsoft.com/en-us/windows/win32/iphlp/managing-ip-addresses-using-getipaddrtable
    MIB_IPADDRTABLE  *pIPAddrTable;
    DWORD            dwSize = 0;
    DWORD            dwRetVal;
    pIPAddrTable = (MIB_IPADDRTABLE*) malloc( sizeof(MIB_IPADDRTABLE) );
    if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
        free( pIPAddrTable );
        pIPAddrTable = (MIB_IPADDRTABLE *) malloc ( dwSize );
    }
    if ( (dwRetVal = GetIpAddrTable( pIPAddrTable, &dwSize, 0 )) == NO_ERROR ) {
        for (auto i = 0; i < pIPAddrTable->dwNumEntries; ++i) {
            IN_ADDR address;
            address.S_un.S_addr = pIPAddrTable->table[i].dwAddr;
            char buffer[MAX_PATH];
            inet_ntop(AF_INET, &address, buffer, MAX_PATH);
            addresses.emplace_back(buffer);
        }
    }
    if (pIPAddrTable)
        free(pIPAddrTable);

    return addresses;
}

#elif __unix__
std::vector<std::string> GetIpAddresses()
{
    std::vector<std::string> addresses;
    ifaddrs * iface_info=nullptr;
    getifaddrs(&iface_info);
    auto iface_info_current = iface_info;
    while(iface_info_current != nullptr)
    {
        if(!iface_info_current->ifa_addr) {
            continue;
        }

        if(iface_info_current->ifa_addr->sa_family == AF_INET) {
            // IPv4 address
            void* net_address = &(reinterpret_cast<sockaddr_in*>(iface_info_current->ifa_addr)->sin_addr);
            char buffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, net_address, buffer, INET_ADDRSTRLEN);
            addresses.emplace_back(buffer);
        } else if(iface_info_current->ifa_addr->sa_family == AF_INET6) {
            // IPv6 address
            void* net_address = &(reinterpret_cast<sockaddr_in*>(iface_info_current->ifa_addr)->sin_addr);
            char buffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, net_address, buffer, INET6_ADDRSTRLEN);
            addresses.emplace_back(buffer);
        }

        iface_info_current = iface_info_current->ifa_next;
    }
    if(iface_info != nullptr) {
        freeifaddrs(iface_info);
    }
    return addresses;
}

#else
std::vector<std::string> GetIpAddresses()
{
    throw std::runtime_error("GetIpAddresses() not implemented for current OS type.");
}
#endif