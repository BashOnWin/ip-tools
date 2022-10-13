#pragma once

#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

using namespace std;

class Interface
{
public:
	Interface() {
		index = 0;	// The index of interface, we need to get more details
		type = 0;	// Type of interface: ETH, WIfi...
		ip_addr = "";	// IP of interface
		name = "";	// Name of interface(description for windows API)	
		ip_mask = "";	// Mask of interface
		ip_broadcast = "";	// Broadcast IP
		mtu = 0;	// MTU
		unicast = false;	// Is unicast?
		multicast = false;
		status = IfOperStatusDown;	// Status of adapter
	};
	~Interface() {
#if DEBUG
		cout << "Interface destroyed";
#endif // DEBUG

	};
	void Set_Type(unsigned int new_type) {
		type = new_type;
	}
	void Set_Name(string new_name) {
		name = new_name;
	}
	void Set_Address(string new_addr) {
		ip_addr = new_addr;
	}
	void Set_Mask(string new_mask) {
		ip_mask = new_mask;
	}
	//void Set_Gateway(string new_gateway) {
	//	ip_gateway = new_gateway;
	//}
	void Set_Index(int new_index) {
		index = new_index;
	}
	void Set_Broadcast(string new_broad) {
		ip_broadcast = new_broad;
	}
	void Set_MTU(unsigned long new_mtu) {
		mtu = new_mtu;
	}
	void Set_Unicast(bool is_unicast) {
		unicast = is_unicast;
	}
	void Set_Multicast(bool is_Multicast) {
		multicast = is_Multicast;
	}
	void Set_Status(IF_OPER_STATUS new_state) {
		status = new_state;
	}
//private:
	int index;	// The index of interface, we need to get more details
	unsigned int type;	// Type of interface: ETH, WIfi...
	string ip_addr;	// IP of interface
	string name;	// Name of interface(description for windows API)	
	string ip_mask;	// Mask of interface
	string ip_broadcast;	// Broadcast IP
	unsigned long mtu;	// MTU
	bool unicast;	// Is unicast?
	bool multicast;
	IF_OPER_STATUS status;	// Status of adapter
	//string ip_gateway;	// Gateway IP
};
