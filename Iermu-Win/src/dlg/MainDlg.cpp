// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include "json.h"
#include "SerializeObj.h"

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif


void WkeWebUrlChanged(wkeWebView webView, void* param, wkeWebFrameHandle frame, const wkeString url)
{
	const char *pstr1, *pstr2, *pstr3;
	CMainDlg *dlg = (CMainDlg *)param;
	pstr1 = (char *)wkeToString(url);

	pstr2 = strstr(pstr1, "iermuconnect://success");
	if (pstr2)
		pstr3 = pstr2 + 10;
	
}

void LoadingFinishCallback (wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason)
{
	const char *pstr1, *pstr2, *pstr3;
	CMainDlg *dlg = (CMainDlg *)param;
	int a, b;
	switch(result)
	{
		case WKE_LOADING_SUCCEEDED:
			a = 1;
			break;
		case WKE_LOADING_FAILED:
			b = 2;
			break;
	}
	pstr1 = (char *)wkeToString(failedReason);
	pstr3 = (char *)wkeToString(url);
	pstr2 = strstr(pstr1, "iermuconnect://success");
	if (pstr2)
		pstr3 = pstr2 + 10;
}

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	m_nRetCode = 0;
	return 0;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	CSerializeData  lg;
	ifstream fin("login.dat", ios::binary);
	if (fin)
	{
		lg.loadLoginInfo(fin);
		string user = lg.getLoginUser();
		string pwd = lg.getLoginPasswd();
		if (!user.empty())
		{
			SEdit *edit_user = FindChildByName2<SEdit>(L"login_edit_username");
			SCheckBox *check_user = FindChildByName2<SCheckBox>(L"check_phone");
			SStringA tuser = user.c_str();

			edit_user->SetWindowText(S_CA2T(tuser));
			check_user->SetCheck(true);
		}

		if (!pwd.empty())
		{
			SEdit *edit_passwd = FindChildByName2<SEdit>(L"login_edit_passwd");
			SCheckBox *check_passwd = FindChildByName2<SCheckBox>(L"check_passwd");
			SStringA tpwd = pwd.c_str();

			edit_passwd->SetWindowText(S_CA2T(tpwd));
			check_passwd->SetCheck(true);
		}
	}
#if 0
	CppSQLite3DB db;
	try
	{
		char *sql = "CREATE TABLE if not exists userTable(  \
					  	uid			TEXT		NOT NULL, \
						username    TEXT		UNIQUE NOT NULL, \
						token       TEXT );";
		db.open(USERINFO_DB);
		db.execDML(sql);
	}
	catch (CppSQLite3Exception& e)
	{

	}
	db.close();
#endif
	m_bLayoutInited = TRUE;
	return 0;
}
//TODO:消息映射
void CMainDlg::OnClose()
{
	AnimateHostWindow(200, AW_CENTER | AW_HIDE);
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		AnimateHostWindow(200, AW_CENTER);
	}
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

void CMainDlg::OnBtnLogin()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab)
	{
		pTab->SetCurSel(_T("login_page"));
	}
}

void CMainDlg::OnBtnRegister()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab)
	{
		pTab->SetCurSel(_T("register_page"));
	}
}


bool LoadUrlBeginCallback(wkeWebView webView, void* param, const char *url, void *job)
{
	const char *pstr1, *pstr2, *pstr3;
	CMainDlg *dlg = (CMainDlg *)param;
	pstr1 = url;

	pstr2 = strstr(pstr1, IERMUCONNECTSUC);
	if (pstr2){
		pstr3 = pstr2 + sizeof(IERMUCONNECTSUC);
		return true;
	}


	return false;
}


void LoadUrlEndCallback(wkeWebView webView, void* param, const char *url, void *job, void* buf, int len)
{
	const char *pstr1, *pstr2, *pstr3;
	CMainDlg *dlg = (CMainDlg *)param;
	pstr1 = url;

	pstr2 = strstr(pstr1, IERMUCONNECTSUC);
	if (pstr2){
		pstr3 = pstr2 + sizeof(IERMUCONNECTSUC);
		return;
	}


	return;
}

void CMainDlg::OnBtnBaiduLogin()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab)
	{
		pTab->SetCurSel(_T("baiduAuth_page"));
	}
	SWkeWebkit *pWebkit = FindChildByName2<SWkeWebkit>(L"wke_baidu");
	pWebkit->SetAttribute(L"url", LBAIDUAUTHWEBURL, FALSE);
	wkeGoForward(pWebkit->GetWebView());
	wkeOnURLChanged2(pWebkit->GetWebView(), WkeWebUrlChanged, this);
	//wkeOnURLChanged(pWebkit->GetWebView(), WkeWebUrlChanged, this);
	//wkeOnLoadingFinish(pWebkit->GetWebView(), LoadingFinishCallback, NULL);
	wkeOnLoadUrlBegin(pWebkit->GetWebView(), LoadUrlBeginCallback, IERMUCONNECTSUC);
	wkeOnLoadUrlEnd(pWebkit->GetWebView(), LoadUrlEndCallback, NULL);
}

void CMainDlg::OnBtnBack()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab)
	{
		pTab->SetCurSel(_T("main_page"));
	}
}

void CMainDlg::OnBaiduBack()
{
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab)
	{
		pTab->SetCurSel(_T("login_page"));
	}
}

void CMainDlg::OnBtnLoginShowOrHidePassWord()
{
	SWindow* show_passwd = FindChildByName(L"login_show_passwd");
	SEdit *edit_passwd = FindChildByName2<SEdit>(L"login_edit_passwd");
	SStringT strSrcName = show_passwd->GetWindowTextW();
	if (!show_passwd || !edit_passwd) return;

	if (wcscmp(strSrcName, L"显示密码") == 0)
	{
		show_passwd->SetWindowText(L"隐藏密码");
		edit_passwd->SetAttribute(L"password", L"0", FALSE);
	}
	else
	{
		show_passwd->SetWindowText(L"显示密码");
		edit_passwd->SetAttribute(L"password", L"1", FALSE);
	}
}

void CMainDlg::OnBtnRegShowOrHidePassWord()
{
	SWindow* show_passwd = FindChildByName(L"reg_show_passwd");
	SEdit *edit_passwd = FindChildByName2<SEdit>(L"reg_edit_passwd");
	if (!show_passwd || !edit_passwd) return;

	SStringT strSrcName = show_passwd->GetWindowText();
	if (wcscmp(strSrcName, L"显示密码") == 0)
	{
		show_passwd->SetWindowText(L"隐藏密码");
		edit_passwd->SetAttribute(L"password", L"0", FALSE);
	}
	else
	{
		show_passwd->SetWindowText(L"显示密码");
		edit_passwd->SetAttribute(L"password", L"1", FALSE);
	}
}

void CMainDlg::OnBtnLoginHttpRequest()
{
#ifdef TEST_DEBUG
	m_nRetCode = 0; // 登陆
	OnClose();
	return;
#endif
	STabCtrl *pTab = FindChildByName2<STabCtrl>(L"tab_main");
	if (pTab->GetCurSel() != 1) return;

	SEdit *edit_username = FindChildByName2<SEdit>(L"login_edit_username");
	SEdit *edit_passwd = FindChildByName2<SEdit>(L"login_edit_passwd");

	SStringT  usrName = edit_username->GetWindowTextW();
	SStringT  usrpasswd = edit_passwd->GetWindowTextW();

	if (!usrName.GetLength())
	{
		MessageBox(NULL, _T("请输入用户名"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	if (usrName.GetLength() != 11)
	{
		MessageBox(NULL, _T("请输入11位手机号码"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	if (!usrpasswd.GetLength())
	{
		MessageBox(NULL, _T("请输入密码"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}

	SStringA tusr = S_CT2A(usrName); 
	SStringA tpasswd = S_CT2A(usrpasswd);
	string usr = tusr;
	string passwd = tpasswd;

	string data = "client_id=j095DeA6kUL9oqZrcdmm&grant_type=password&lang=zh-Hans";
	data.append("&mobile=").append(usr).append("&password=").append(passwd);

	m_userData.opt = OPT_LOGIN;
	m_userData.url = AUTHORIZATION_URL;
	m_userData.type = POST;
	m_userData.data = data;

	LPVOID param = (LPVOID)&m_userData;
	OnStartHttpThread(param);
}

void CMainDlg::saveUserInfo(string name)
{
	SCheckBox *check_user = FindChildByName2<SCheckBox>(L"check_phone");
	string  usrName = "", passwd = "";
	if (check_user->IsChecked())
	{
		usrName = name;
	}

	SCheckBox *check_passwd = FindChildByName2<SCheckBox>(L"check_passwd");
	if (check_user->IsChecked())
	{
		SEdit *edit_passwd = FindChildByName2<SEdit>(L"login_edit_passwd");
		SStringA tpasswd = S_CT2A(edit_passwd->GetWindowTextW());
		passwd = tpasswd;
	}

	CSerializeData lg(usrName, passwd);
	ofstream fout("login.dat", ios::binary);
	lg.saveLoginInfo(fout);
	fout.close();
}

void CMainDlg::saveDatabase(string uid, string name)
{
#if 0
	CppSQLite3DB db;
	char sql[256] = {0};
	try
	{
		db.open(USERINFO_DB);
		sprintf(sql, "REPLACE INTO userTable (uid, phone, token) values (\'%s\', \'%s\', \'%s\')", uid.c_str(), name.c_str(), m_token.c_str());
		db.execDML(sql);
	}
	catch(CppSQLite3Exception& e)
	{

	}
	db.close();
#endif
}

UINT CMainDlg::Run(LPVOID data)
{
	MainHttpThread *pEvt = new MainHttpThread(this);
	HttpUrlData *param = (HttpUrlData *)data;

	CHttpConnect WinClient;

	pEvt->opt = param->opt;
	pEvt->nData = WinClient.Request(param->url, param->type, param->data);
	pEvt->retOK = WinClient.GetStatusIsOK();
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
	pEvt->Release();
	Sleep(500);
	return 0;
}

void CMainDlg::OnStartHttpThread(LPVOID data)
{
	if (IsRunning()) return;
	SNotifyCenter::getSingleton().addEvent(EVENTID(MainHttpThread));

	BeginThread(data);
}


bool CMainDlg::OnMainHttpThread(EventArgs *e)
{
	MainHttpThread *pEvt = sobj_cast<MainHttpThread>(e);
	string response = pEvt->nData;
	Json::Reader reader;
	Json::Value jsonobj;
	if (!reader.parse(response, jsonobj))
	{
		MessageBox(NULL, _T("请检查网络后重试"), _T("网络错误"), MB_OK | MB_ICONERROR);
		return false;
	}

	switch (pEvt->opt)
	{
		case OPT_LOGIN:
			if (pEvt->retOK)
			{
				m_uid = jsonobj["uid"].asString();
				m_token = jsonobj["access_token"].asString();
				if (!m_token.empty())
				{			
					SEdit *edit_username = FindChildByName2<SEdit>(L"login_edit_username");
					SStringA tusrName = S_CT2A(edit_username->GetWindowTextW());
					string usrName = tusrName;
					saveUserInfo(usrName);
					//saveDatabase(m_uid, usrName);
					m_nRetCode = 1; // 登陆
					OnClose();
				}
			}
			else
				MessageBox(NULL, _T("请输入正确的爱耳目账号密码。\n百度账号用户请点击“使用百度账号登录”"), _T("账号或密码错误"), MB_OK | MB_ICONERROR);
			break;
		case OPT_REGISTRE:

			break;
		default:
			break;

	}
	SNotifyCenter::getSingleton().removeEvent(MainHttpThread::EventID);
	return true;
}

