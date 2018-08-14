#pragma once

#include "HttpConnect.h"
using namespace std;


#define AUTHORIZATION_URL	"https://api.iermu.com/oauth2/token"
#define GETUSERINFO_URL		"https://api.iermu.com/v2/passport/user?"
#define GETDEVICEINFO_URL	"https://api.iermu.com/v2/pcs/device"

#define EVT_HTTP_BEGIN				(EVT_EXTERNAL_BEGIN + 30000)

typedef enum
{
	OPT_LOGIN,
	OPT_REGISTRE,
	OPT_GETDEVICEINFO,  //获取设备基本信息
	OPT_GETNETWORKINFO, //获取设备网络信息
	OPT_GETUSERINFO,
	OPT_GETTHUMBNAIL,
	OPT_GETCAMERAIP
}HTTPOPTION;

enum 
{ 
	SENDER_MAINDLG_ID = 30000, 
	SENDER_GROUPDLG_ID
};
typedef struct
{
	HTTPOPTION opt;
	string url;
	REQUEST_TYPE type;
	string data;
}HttpUrlData;

typedef struct
{
	const char* errMsg;
}HttpErrorResponse;

//在MainDlg中使用SNotifyCenter的通知的异步事件 
class MainHttpThread : public TplEventArgs<MainHttpThread>
{
	SOUI_CLASS_NAME(MainHttpThread, L"on_main_http_thread")
public:
	MainHttpThread(SObject *pSender) :TplEventArgs<MainHttpThread>(pSender){}
	enum{ EventID = EVT_HTTP_BEGIN };

	HTTPOPTION opt;
	string nData;
	bool  retOK;
};

//在GroupDlg中使用SNotifyCenter的通知的异步事件 
class GroupHttpThread : public TplEventArgs<GroupHttpThread>
{
	SOUI_CLASS_NAME(GroupHttpThread, L"on_group_http_thread")
public:
	GroupHttpThread(SObject *pSender) :TplEventArgs<GroupHttpThread>(pSender){}
	enum{ EventID = EVT_HTTP_BEGIN+1 };

	HTTPOPTION opt;
	string nData;
	bool  retOK;
};

//在CCameraAdapter中使用SNotifyCenter的通知的异步事件 
class CameraAdapterHttpThread : public TplEventArgs<CameraAdapterHttpThread>
{
	SOUI_CLASS_NAME(GroupHttpThread, L"on_CameraAdapter_http_thread")
public:
	CameraAdapterHttpThread(SObject *pSender) :TplEventArgs<CameraAdapterHttpThread>(pSender){}
	enum{ EventID = EVT_HTTP_BEGIN + 2 };

	HTTPOPTION opt;
	string nData;
	bool  retOK;
};