#pragma once

#include "ThreadObject.h"
#include "HttpData.h"
#include "CameraAdapter.h"

class CGroupDlg : public SHostWnd
				,public CThreadObject	//线程对象
				,public TAutoEventMapReg<CGroupDlg>	//通知中心自动注册
{
public:
	string				m_token;
	SArray<CameraItem>  m_cameraList;
	CGroupDlg(string uid, string token);
	~CGroupDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);

	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);

	void OnBtnFreshList();
	LRESULT OnPlayVideo(UINT uMsg, WPARAM wp, LPARAM lp, BOOL & bHandled);
protected:

	void GetUserInfoRequest();
	void GetThumbnailImg();
	void GetDeviceInfoRequest();

	virtual UINT Run(LPVOID data);
	virtual int GetID() const { return SENDER_GROUPDLG_ID; }

	bool OnGroupHttpThread(EventArgs *e);
	void OnStartHttpThread(LPVOID data);

	//soui消息
	EVENT_MAP_BEGIN()
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_refresh", OnBtnFreshList)
		//<--通知中心事件
		EVENT_ID_HANDLER(SENDER_GROUPDLG_ID, GroupHttpThread::EventID, OnGroupHttpThread)
		//-->
	EVENT_MAP_END()


	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CGroupDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		CHAIN_MSG_MAP(SHostWnd)
		MESSAGE_HANDLER(WM_USER_GETPLAYURL, OnPlayVideo)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()

private:
	CameraStatus GetCameraStatus(int liveStatus);
	SStringT GetCameraCmpTag(CameraStatus status);
	void loadDeviceInfo();
	void saveDeviceInfo(string response);
	void SetRefreshDisplay(bool Dpflag);
	void SetAdapterView(bool refdisplay);

	CFourInterface		m_player;
	CCameraAdapter*		m_pAdapter;
	string				m_uid;
	HttpUrlData			m_sendData;
	BOOL				m_bLayoutInited;
};
