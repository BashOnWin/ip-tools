// ip-lib.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "ip.h" // Interface class
#include "crash.h"  // Crash function
#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

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
            adapter.Set_Type(pAdapter->Type);   // Fill all attributes
            adapter.Set_Name(pAdapter->Description);
            adapter.Set_Address(pAdapter->IpAddressList.IpAddress.String);
            adapter.Set_Mask(pAdapter->IpAddressList.IpMask.String);
            //adapter.Set_Gateway(pAdapter->GatewayList.IpAddress.String);  // Currently gateway is not necessary for ifconfig
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
