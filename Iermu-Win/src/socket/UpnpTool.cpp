#include "stdafx.h"
#include "UpnpTool.h"
#include "upnp.h"

void UpnpTool::GetHostAddress(string &strIPAddr)
{
	char    HostName[100];
	gethostname(HostName, sizeof(HostName));// 获得本机主机名.

	hostent* hn;
	hn = gethostbyname(HostName);//根据本机主机名得到本机ip

	if (hn)
		strIPAddr = inet_ntoa(*(struct in_addr *)hn->h_addr_list[0]);//把ip换成字符串形式
}

int UpnpTool::ReceiveData(int socket, char * data, int length, int timeout, struct sockaddr* addr, socklen_t* len)
{
	int n;
	fd_set socketSet;
	struct timeval timeval;
	FD_ZERO(&socketSet);
	FD_SET(socket, &socketSet);
	timeval.tv_sec = timeout / 1000;
	timeval.tv_usec = (timeout % 1000) * 1000;
	n = select(0, &socketSet, NULL, NULL, &timeval);
	if (n < 0) {
		printf("select");
		return -1;
	}
	else if (n == 0) {
		return 0;
	}
	n = recvfrom(socket, data, length, 0, addr, len);
	if (n < 0) {
		printf("recv");
	}
	return n;
}

bool UpnpTool::UpnpParse(char *bufr, int n, DeviceIDAndIP* ddp)
{
	char *ptmp, *ptmp2;
	memset(ddp, '\0', sizeof(DeviceIDAndIP)); 
	bufr[n] = '\0';

	ptmp = strstr(bufr, "LOCATION: ");
	if (!ptmp) return false;

	ptmp2 = strstr(ptmp, "http://");
	if (!ptmp2) return false;

	ptmp2 += 7; // strlen("http://");
	ptmp = strchr(ptmp2, '/');
	if (!ptmp) return false;

	strncpy(ddp->ip, ptmp2, ptmp - ptmp2);
	ptmp += 1;

	ptmp2 = strchr(ptmp, '/');
	if (!ptmp2) return false;

	strncpy(ddp->id, ptmp, ptmp2 - ptmp);

	return true;
}

//发广播包
bool UpnpTool::upnpDiscover(int delay, SArray<DeviceIDAndIP> &devlist)
{
	bool ret = false;
	WSADATA wsaData;
	int opt = 1;
	int trynum = 1;
	DeviceIDAndIP ddp;
	static const char MSearchMsgFmt[] = "M-SEARCH * HTTP/1.1\r\n  HOST: 239.255.255.250:1900\r\n  ST: %s\r\n  MAN: ssdp:discover\r\n  MX: 3\r\n\r\n";

	string hostip;
	char bufr[1536]; /* reception and emission buffer */
	int sudp;
	int n;
	struct sockaddr_in sockudp_r, sockudp_w;

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) == SOCKET_ERROR){
		return ret;
	}

	sudp = socket(AF_INET, SOCK_DGRAM, 0);
	if (sudp < 0)
	{
		WSACleanup();
		return ret;
	}
	memset(&sockudp_r, 0, sizeof(struct sockaddr_in));
	sockudp_r.sin_family = AF_INET;
	sockudp_r.sin_port = htons(0);
	sockudp_r.sin_addr.s_addr = INADDR_ANY; 

	memset(&sockudp_w, 0, sizeof(struct sockaddr_in));
	sockudp_w.sin_family = AF_INET;
	sockudp_w.sin_port = htons(PORT);
	sockudp_w.sin_addr.s_addr = INADDR_BROADCAST;

	if (setsockopt(sudp, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) < 0)
	{
		goto failed;
	}
	opt = 1;
	if (setsockopt(sudp, SOL_SOCKET, SO_BROADCAST, (const char *)&opt, sizeof(opt)) < 0)
	{
		goto failed;
	}
	GetHostAddress(hostip);
	if (!hostip.empty())
	{
		struct in_addr mc_if;
		mc_if.s_addr = inet_addr(hostip.c_str());
		sockudp_r.sin_addr.s_addr = mc_if.s_addr;
		if (setsockopt(sudp, IPPROTO_IP, IP_MULTICAST_IF, (const char *)&mc_if, sizeof(mc_if)) < 0)
		{
			goto failed;
		}
	}
	if (bind(sudp, (struct sockaddr*)&sockudp_r, sizeof(struct sockaddr_in)) != 0) {
		goto failed;
	}

	n = sprintf(bufr, MSearchMsgFmt, "urn:schemas-upnp-org:device:MediaServer:"); //  //ssdp:all
	n = sendto(sudp, bufr, n, 0, (struct sockaddr*)&sockudp_w, sizeof(struct sockaddr_in));
	/* sending the SSDP M-SEARCH packet */
	if (n < 0) {
		SLOGFMTE("Find upnp device broadcast fail!");
		goto failed;
	}

	devlist.RemoveAll();
	while (trynum > 0) {
		SLOGFMTE("剩余尝试次数%d \n", trynum);		
		do
		{
			memset(bufr, 0, sizeof(bufr));
			n = ReceiveData(sudp, bufr, sizeof(bufr), delay, (struct sockaddr*)&sockudp_w, (socklen_t*)&sockudp_w);
			if (n > 0) {
				bufr[n] = 0;
				SLOGFMTE("%s", bufr);
				if (UpnpParse(bufr, n, &ddp))
				{
					devlist.Add(ddp);
					ret = true;
				}
			}
		} while (n > 0);
		trynum--;
		// usleep(600);
	}
failed:
	closesocket(sudp);
	WSACleanup();
	return ret;
}
