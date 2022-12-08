#include "get_ip_addresses.hpp"

#ifdef _WIN32
#elif __unix__
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif


#ifdef _WIN32
std::vector<std::string> GetIpAddresses()
{
    throw std::runtime_error("GetIpAddresses() not implemented for Windows yet.");
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