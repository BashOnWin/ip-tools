#pragma once

#include <iostream>

using namespace std;

class Interface
{
public:
	Interface() {
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
//private:
	unsigned int type;	// Type of interface: ETH, WIfi...
	string ip_addr;	// IP of interface
	string name;	// Name of interface(description for windows API)	
	string ip_mask;	// Mask of interface
	//string ip_gateway;	// Gateway IP
};
