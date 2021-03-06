// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <iostream>
#include <iomanip>
#include "ThreadObject.h"
#include "HttpData.h"
//#include "CppSQLite3.h"  

class CMainDlg : public SHostWnd 
				 ,public CThreadObject	//线程对象
				 ,public TAutoEventMapReg<CMainDlg>	//通知中心自动注册
{
public:
	CMainDlg();
	~CMainDlg();

	void OnClose();
	void OnMaximize();
	void OnRestore();
	void OnMinimize();
	void OnSize(UINT nType, CSize size);
	
	void OnBtnMsgBox();
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog(HWND wndFocus, LPARAM lInitParam);
	void OnShowWindow(BOOL bShow, UINT nStatus);
	void OnBtnLogin();
	void OnBtnRegister();
	void OnBtnBaiduLogin();
	void OnBtnBack();
	void OnBaiduBack();
	void OnBtnLoginShowOrHidePassWord();
	void OnBtnRegShowOrHidePassWord();
	void OnBtnLoginHttpRequest();

	string GetToken() { return m_token; }
	string GetUID() { return m_uid; }
	int  GetReturnCode() { return m_nRetCode; }

protected:

	virtual int GetID() const { return SENDER_MAINDLG_ID; }
	virtual UINT Run(LPVOID data);

	void OnStartHttpThread(LPVOID data);
	bool OnMainHttpThread(EventArgs *e);

		//soui消息
	EVENT_MAP_BEGIN()
		//<--通知中心事件
		EVENT_ID_HANDLER(SENDER_MAINDLG_ID, MainHttpThread::EventID, OnMainHttpThread)
		//-->
		EVENT_NAME_COMMAND(L"btn_close", OnClose)
		EVENT_NAME_COMMAND(L"btn_min", OnMinimize)
		EVENT_NAME_COMMAND(L"btn_max", OnMaximize)
		EVENT_NAME_COMMAND(L"btn_restore", OnRestore)
		EVENT_NAME_COMMAND(L"btn_login", OnBtnLogin)
		EVENT_NAME_COMMAND(L"btn_register", OnBtnRegister)
		EVENT_NAME_COMMAND(L"btn_baidu_login", OnBtnBaiduLogin)
		EVENT_NAME_COMMAND(L"btn_back", OnBtnBack)
		EVENT_NAME_COMMAND(L"baidu_back", OnBaiduBack)
		EVENT_NAME_COMMAND(L"text_register", OnBtnRegister)
		EVENT_NAME_COMMAND(L"text_login", OnBtnLogin)
		EVENT_NAME_COMMAND(L"login_show_passwd", OnBtnLoginShowOrHidePassWord)
		EVENT_NAME_COMMAND(L"reg_show_passwd", OnBtnRegShowOrHidePassWord)
		EVENT_NAME_COMMAND(L"btn_login_ok", OnBtnLoginHttpRequest)
	EVENT_MAP_END()
		
	//HostWnd真实窗口消息处理
	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_SIZE(OnSize)
		MSG_WM_SHOWWINDOW(OnShowWindow)
		CHAIN_MSG_MAP(SHostWnd)
		REFLECT_NOTIFICATIONS_EX()
	END_MSG_MAP()
private:
	void saveUserInfo(string name);
	void saveDatabase(string uid, string name);


	string			 m_uid;
	string			 m_token;
	HttpUrlData      m_userData;
	BOOL			 m_bLayoutInited;
	int              m_nRetCode;
	wkeClientHandler m_getClientHandler;
};
