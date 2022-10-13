#pragma once
#include "framework.h"
#include "ip.h" // Interface class
#include "crash.h"  // Crash function
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

string get_broadcast_ip(int index_to_search) {
    int i;

    /* Variables used by GetIpAddrTable */
    PMIB_IPADDRTABLE pIPAddrTable;
    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;

    /* Variables used to return error message */
    LPVOID lpMsgBuf;

    // Before calling AddIPAddress we use GetIpAddrTable to get
    // an adapter to which we can add the IP.
    pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        // Make an initial call to GetIpAddrTable to get the
        // necessary size into the dwSize variable
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) ==
            ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(dwSize);

        }
        if (pIPAddrTable == NULL) {
            printf("Memory allocation failed for GetIpAddrTable\n");
            crash();
        }
    }
    // Make a second call to GetIpAddrTable to get the
    // actual data we want
    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0)) != NO_ERROR) {
        printf("GetIpAddrTable failed with error %d\n", dwRetVal);
        if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),       // Default language
            (LPTSTR)&lpMsgBuf, 0, NULL)) {
            printf("\tError: %s", lpMsgBuf);
            LocalFree(lpMsgBuf);
        }
        crash();
    }
    for (auto interface_raw : pIPAddrTable->table) {
        if (interface_raw.dwIndex == index_to_search) {
            char str[INET_ADDRSTRLEN];
            IPAddr.S_un.S_addr = (u_long)interface_raw.dwMask;
            return inet_ntop(AF_INET, &(IPAddr), str, INET_ADDRSTRLEN);
        };
    }
    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }
    return "Unknown";
}

class Extended_info {
public:
    unsigned long mtu;
    bool is_unicast;
    IF_OPER_STATUS status;
    bool is_multicast;

    Extended_info() {
        mtu = 0;
        is_unicast = false;
        status = IfOperStatusDown;
        is_multicast = false;
    }
};
Extended_info get_extended_info(int index_to_find) {
    /* Declare and initialize variables */

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
    PIP_ADAPTER_ANYCAST_ADDRESS pAnycast = NULL;
    PIP_ADAPTER_MULTICAST_ADDRESS pMulticast = NULL;
    IP_ADAPTER_DNS_SERVER_ADDRESS* pDnServer = NULL;
    IP_ADAPTER_PREFIX* pPrefix = NULL;

    family = AF_INET6;
    outBufLen = WORKING_BUFFER_SIZE;
    Extended_info info = Extended_info();
    do {

        pAddresses = (IP_ADAPTER_ADDRESSES*)MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf
            ("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            exit(1);
        }

        dwRetVal =
            GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        }
        else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            if (pCurrAddresses->IfIndex == index_to_find) {
                pUnicast = pCurrAddresses->FirstUnicastAddress;

                string is_unicast = "";

                if (pUnicast != NULL) {
                    info.is_unicast = true;
                }

                pAnycast = pCurrAddresses->FirstAnycastAddress;
                if (pAnycast) {
                    info.is_unicast = false;
                }

                pMulticast = pCurrAddresses->FirstMulticastAddress;
                if (pMulticast) {
                    info.is_multicast = true;
                }

                info.mtu = pCurrAddresses->Mtu;
                info.status = pCurrAddresses->OperStatus;


                pPrefix = pCurrAddresses->FirstPrefix;

                pCurrAddresses = pCurrAddresses->Next;
            }
            else {
                pPrefix = pCurrAddresses->FirstPrefix;

                pCurrAddresses = pCurrAddresses->Next;
            }

            
        }
    }
    else {
        crash();
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
    return info;
}

vector<Interface> get_interfaces()
{

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;
    UINT i;

    /* variables used to print DHCP time info */
    struct tm newtime;
    char buffer[32];
    errno_t error;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        crash();
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO*)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            crash();
        }
    }

    vector<Interface> interfaces = vector<Interface>(); // Vector of Interface, This will contain all interface of PC
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            Interface adapter = Interface();    // Create a new interface
            adapter.Set_Index(pAdapter->Index);
            adapter.Set_Type(pAdapter->Type);   // Fill all attributes
            adapter.Set_Name(pAdapter->Description);
            adapter.Set_Address(pAdapter->IpAddressList.IpAddress.String);
            adapter.Set_Mask(pAdapter->IpAddressList.IpMask.String);
            adapter.Set_Broadcast(get_broadcast_ip(adapter.index)); // Set Broadcast IP from get_broadcast function
            //adapter.Set_Gateway(pAdapter->GatewayList.IpAddress.String);  // Currently gateway is not necessary for ifconfig

            Extended_info info = get_extended_info(adapter.index);    // Get an extended information from function
            adapter.Set_MTU(info.mtu);  // Set MTU from function
            adapter.Set_Unicast(info.is_unicast);   // Is adapter unicast?
            adapter.Set_Multicast(info.is_multicast);
            adapter.Set_Status(info.status);    // Status of adapter
            interfaces.push_back(adapter);  // Put it in vector
            pAdapter = pAdapter->Next;  // Go to next adapter
            adapter.~Interface();   // Destroy adapter to free memory
        }
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);

    return interfaces;
}