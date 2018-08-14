#pragma once
#include <iostream>
#include <ws2tcpip.h> 

#pragma comment(lib, "ws2_32.lib")  
using namespace std;

#define PORT	1900
#define UPNP_MCAST_ADDR "239.255.255.250"

typedef struct{
	char id[16];
	char ip[30];
}DeviceIDAndIP;

class UpnpTool
{
public:
	bool upnpDiscover(int delay, SArray<DeviceIDAndIP> &devlist);
private:
	void GetHostAddress(string &strIPAddr);
	int ReceiveData(int socket, char * data, int length, int timeout, struct sockaddr* addr, socklen_t* len);
	bool UpnpParse(char *bufr, int n, DeviceIDAndIP* ddp);
};