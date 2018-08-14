
#include "stdafx.h"
#include "GroupDlg.h"
#include "json.h"
#include <helper/SAdapterBase.h>
#include "CppSQLite3.h"

#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif

CGroupDlg::CGroupDlg(string uid, string token) : SHostWnd(_T("LAYOUT:XML_GROUPWND"))
{
	m_bLayoutInited = FALSE;
	m_uid = uid;
	m_token = token;

#ifdef TEST_DEBUG
	m_uid = "189642";
	m_token = "1.j095DeA6kUL9oqZrcdmm.2592000.1503999038.de9f7d4ca003f9d2ff7a8a3a001b3a02";
#endif
}

CGroupDlg::~CGroupDlg()
{
	if (m_pAdapter)
	{
		m_pAdapter->CameraAdapterClose();
		m_pAdapter->Release();
		m_pAdapter = NULL;
	}
}

int CGroupDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = { 5, 5, 30, 5 };
	DwmExtendFrameIntoClientArea(m_hWnd, &mar);
#endif

	SetMsgHandled(FALSE);
	return 0;
}

BOOL CGroupDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	loadDeviceInfo();
	m_bLayoutInited = TRUE;
	GetDeviceInfoRequest();
	return 0;
}
//TODO:消息映射
void CGroupDlg::OnClose()
{
	AnimateHostWindow(200, AW_CENTER | AW_HIDE);
	CSimpleWnd::DestroyWindow();
	if (m_pAdapter)
	{
		m_pAdapter->CameraAdapterClose();
		m_pAdapter->Release();
		m_pAdapter = NULL;
	}
}

void CGroupDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CGroupDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CGroupDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CGroupDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (bShow)
	{
		AnimateHostWindow(200, AW_CENTER);
	}
}

void CGroupDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;

	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if (!pBtnMax || !pBtnRestore) return;

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

LRESULT CGroupDlg::OnPlayVideo(UINT uMsg, WPARAM wp, LPARAM lp, BOOL & bHandled)
{
	char *url = (char *)wp;
	char *panoTemplate = (char *)lp;
	SCheckBox *check_fourplayer = FindChildByName2<SCheckBox>(L"check_fourplayer");
	if(check_fourplayer->IsChecked())
		m_player.ffplayFourWithUrl(url);
	else
		m_player.ffplaySingleWithUrl(url, panoTemplate);

	return 0;
}

// Dpflag  flase： 隐藏  true: 显示  
void CGroupDlg::SetRefreshDisplay(bool Dpflag)
{
	SWindow *WinRefresh = FindChildByName(L"win_refresh");
	SWindow *WinCamera = FindChildByName(L"camera_image");

	if (WinRefresh)
	{
		if(Dpflag)
			WinRefresh->SetAttribute(L"show", L"1", FALSE);
		else
			WinRefresh->SetAttribute(L"show", L"0", FALSE);
	}
		
	if (WinCamera)
	{
		WinCamera->SetAttribute(L"show", L"1", FALSE);
	}
}

//count: 摄像机个数   refdisplay: 是否显示刷新
void CGroupDlg::SetAdapterView(bool refdisplay)
{
	STileView *pTileView = FindChildByName2<STileView>("camera_image");
	if (pTileView)
	{
		m_pAdapter = new CCameraAdapter(this);
		pTileView->SetAdapter(m_pAdapter);
		m_pAdapter->SetTags(&m_cameraList);
	}

	SetRefreshDisplay(refdisplay);
}

void CGroupDlg::OnBtnFreshList()
{
	GetDeviceInfoRequest();
	SetRefreshDisplay(TRUE);
}

void CGroupDlg::GetDeviceInfoRequest()
{
	string data = "access_token=";
	data.append(m_token).append("&data_type=my&device_type=1&lang=en&list_type=all&method=list");

	m_sendData.opt = OPT_GETDEVICEINFO;
	m_sendData.url = GETDEVICEINFO_URL;
	m_sendData.type = POST;
	m_sendData.data = data;

	LPVOID param = (LPVOID)&m_sendData;
	OnStartHttpThread(param);
}

CameraStatus CGroupDlg::GetCameraStatus(int liveStatus)
{
	CameraStatus status = TYPE_STATUS_OFFLINE;

	if (liveStatus == 0) {
		//离线
		status = TYPE_STATUS_OFFLINE;
	}
	else if ((liveStatus > 0) && (liveStatus & 4) == 0) {
		//关机
		status = TYPE_STATUS_POWEROFF;
	}
	else if ((liveStatus> 0) && ((liveStatus & 4) == 4)) {
		//开机
		status = TYPE_STATUS_ONLINE;
	}

	return status;
}

SStringT CGroupDlg::GetCameraCmpTag(CameraStatus status)
{
	SStringT tag = L"3";

	switch (status)
	{
		case TYPE_STATUS_OFFLINE:
			tag = L"3";
			break;
		case TYPE_STATUS_POWEROFF:
			tag = L"2";
			break;
		case TYPE_STATUS_ONLINE:
			tag = L"1";
			break;
	}

	return tag;
}

void CGroupDlg::loadDeviceInfo()
{
	CppSQLite3DB db;
	try
	{
		char *sql = "CREATE TABLE if not exists deviceTable(  \
						uid					varchar(255)	 NOT NULL, \
						deviceid			varchar(255)	 NOT NULL,\
						shareid				varchar(255), \
						uk					varchar(255), \
						description			TEXT, \
						share				integer, \
						status				integer, \
						thumbnail			varchar(255), \
						data_type			integer,\
						connect_type		integer, \
						stream_id			varchar(255), \
						cvr_day				varchar(255), \
						cvr_end_time		varchar(255), \
						avatar				varchar(255), \
						username			varchar(255), \
						viewnum				integer, \
						approvenum			integer, \
						subscribe			integer, \
						grantnum			integer, \
						force_upgrade		integer, \
						need_upgrade		integer, \
						device_type			integer, \
						cvr_type			integer, \
						cvr_free			integer, \
						cid					TEXT, \
						intro				varchar(255), \
						commentnum			integer, \
						location			TEXT, \
						showlocation		integer, \
						needpassword		BOOL, \
						share_end_time		DOUBLE, \
						share_expires_in	DOUBLE, \
						timezone			varchar(255), \
						reportstatus		integer, \
						pano_config			varchar(255), \
						PRIMARY KEY(uid, deviceid, data_type));";

		db.open(IERMU_DB);
		db.execDML(sql);

	}
	catch (CppSQLite3Exception& e){
		printf("%s", e.errorMessage());
	}

	try
	{
		int row = 0;
		char queSql[128] = { 0 };

		sprintf(queSql, "SELECT deviceid, thumbnail, description, status, connect_type, pano_config FROM %s WHERE uid=\'%s\'", DEVICETABLE, m_uid.c_str());
		CppSQLite3Table query = db.getTable(queSql);  //执行查询 	
		row = query.numRows();
		CameraItem item;
		for (int i = 0; i < row; i++)
		{
			string temp;
			query.setRow(i);

			item.deviceid = query.fieldValue("deviceid");
			item.thumbnail = query.fieldValue("thumbnail");
			item.description = query.fieldValue("description");
			item.con_type = atoi(query.fieldValue("connect_type"));
			item.status = GetCameraStatus(query.getIntField("status"));
			if (query.fieldValue("pano_config"))
				item.panoTemplate = query.fieldValue("pano_config");
			item.tag = GetCameraCmpTag(item.status);
			m_cameraList.Add(item);
			//SLOGFMTE("deviceid : %s   status: %d ", item.deviceid.c_str(), query.getIntField("status"));
		}

		query.finalize();
	}
	catch (CppSQLite3Exception& e){
		printf("%s", e.errorMessage());
	}

	SetAdapterView(TRUE);
	db.close();
}

void CGroupDlg::saveDeviceInfo(string response)
{
	CppSQLite3DB db;
	Json::Reader reader;
	Json::Value jsonobj;

	if (!reader.parse(response, jsonobj))
		return;

	if (!jsonobj["list"].isNull())
	{
		CameraItem item;
		int list_size = jsonobj["list"].size();
		if (list_size > 0)
		{
			m_cameraList.RemoveAll();
			db.open(IERMU_DB);
		}
					
		for (int i = 0; i < list_size; i++)
		{	
			char sql[4096] = { 0 };
			try
			{
				item.deviceid = jsonobj["list"][i]["deviceid"].asString();
				item.thumbnail = jsonobj["list"][i]["thumbnail"].asString();
				item.description = jsonobj["list"][i]["description"].asString();
				item.con_type = atoi(jsonobj["list"][i]["connect_type"].asString().c_str());
				item.status = GetCameraStatus(atoi(jsonobj["list"][i]["status"].asString().c_str()));
				item.panoTemplate = jsonobj["list"][i]["pano_config"].asString();
				item.tag = GetCameraCmpTag(item.status);
				m_cameraList.Add(item);

				sprintf(sql, "REPLACE INTO %s \
							 (uid, deviceid, shareid, uk \
							 ,description, share, status \
							 ,thumbnail, data_type, connect_type \
							 ,stream_id, cvr_day, cvr_end_time \
							 ,avatar, username, viewnum \
							 ,approvenum, subscribe, grantnum \
							 ,force_upgrade, need_upgrade, device_type \
							 ,cvr_type, cvr_free, cid \
							 ,intro, commentnum, location \
							 ,showlocation, needpassword, share_end_time \
							 ,share_expires_in, timezone, reportstatus, pano_config) values \
							 (\'%s\', \'%s\', \'%s\', \'%s\' \
							 ,\'%s\', %d,  %d \
							 ,\'%s\', %d,  %d \
							 ,\'%s\', \'%s\', \'%s\' \
							 ,\'%s\', \'%s\', %d \
							 ,%d,  %d,  %d \
							 ,%d,  %d,  %d \
							 ,%d,  %d, \'%s\' \
							 ,\'%s\', %d, \'%s\' \
							 ,%d, %d, %f \
							 ,%f, \'%s\', %d,  \'%s\')", DEVICETABLE
							 , m_uid.c_str(), item.deviceid.c_str(), jsonobj["list"][i]["shareid"].asString().c_str(), jsonobj["list"][i]["uk"].asString().c_str()
							 , item.description.c_str(), atoi(jsonobj["list"][i]["share"].asString().c_str()), atoi(jsonobj["list"][i]["status"].asString().c_str())
							 , item.thumbnail.c_str(), jsonobj["list"][i]["data_type"].asInt(), atoi(jsonobj["list"][i]["connect_type"].asString().c_str())
							 ,jsonobj["list"][i]["stream_id"].asString().c_str(), jsonobj["list"][i]["cvr_day"].asString().c_str(), jsonobj["list"][i]["cvr_end_time"].asString().c_str()
							 ,jsonobj["list"][i]["avatar"].asString().c_str(), jsonobj["list"][i]["username"].asString().c_str(), atoi(jsonobj["list"][i]["viewnum"].asString().c_str())
							 ,atoi(jsonobj["list"][i]["approvenum"].asString().c_str()), jsonobj["list"][i]["subscribe"].asInt(), atoi(jsonobj["list"][i]["grantnum"].asString().c_str())
							 ,atoi(jsonobj["list"][i]["force_upgrade"].asString().c_str()), atoi(jsonobj["list"][i]["need_upgrade"].asString().c_str()), atoi(jsonobj["list"][i]["device_type"].asString().c_str())
							 ,atoi(jsonobj["list"][i]["cvr_type"].asString().c_str()), jsonobj["list"][i]["cvr_free"].asInt(), "[]"
							 ,jsonobj["list"][i]["intro"].asString().c_str(), atoi(jsonobj["list"][i]["commentnum"].asString().c_str()), ""
							 ,jsonobj["list"][i]["showlocation"].asInt(), jsonobj["list"][i]["needpassword"].asInt(), jsonobj["list"][i]["share_end_time"].asFloat()
							 , jsonobj["list"][i]["share_expires_in"].asFloat(), jsonobj["list"][i]["timezone"].asString().c_str(), 0, item.panoTemplate.c_str());
												
				db.execDML(sql);
			}
			catch (CppSQLite3Exception& e)
			{
				printf("%s", e.errorMessage());
			}
		}

		if (list_size > 0)
		{
			db.close();
		}
			
	}
}

void CGroupDlg::GetUserInfoRequest()
{
	string data = "access_token=";
	data.append(m_token).append("&connect=1&lang=zh-Hans&method=info");

	m_sendData.opt = OPT_GETUSERINFO;
	m_sendData.url = GETUSERINFO_URL;
	m_sendData.type = POST;
	m_sendData.data = data;

	LPVOID param = (LPVOID)&m_sendData;
	OnStartHttpThread(param);
}

void CGroupDlg::GetThumbnailImg()
{
	m_sendData.opt = OPT_GETTHUMBNAIL;

	LPVOID param = (LPVOID)&m_sendData;
	m_sendData.type = POST;
	OnStartHttpThread(param);
}

UINT CGroupDlg::Run(LPVOID data)
{
	GroupHttpThread *pEvt = new GroupHttpThread(this);
	HttpUrlData *param = (HttpUrlData *)data;
	CHttpConnect WinClient;
	Sleep(500);

	pEvt->opt = param->opt;
	switch (pEvt->opt)
	{
		case OPT_GETDEVICEINFO:
		{
			pEvt->nData = WinClient.Request(param->url, param->type, param->data);
			pEvt->retOK = WinClient.GetStatusIsOK();
			if (pEvt->retOK)
			{
				saveDeviceInfo(pEvt->nData);
			}			
			break;
		}

		case OPT_GETNETWORKINFO:
		{
			pEvt->nData = WinClient.Request(param->url, param->type, param->data);
			pEvt->retOK = WinClient.GetStatusIsOK();
			if (pEvt->retOK)
			{
				//saveDeviceInfo(pEvt->nData);
			}
			break;
		}

		case OPT_GETTHUMBNAIL:
		{
			size_t index = 0;
			while (m_cameraList.GetCount() > index)
			{
				char path[256] = { 0 };
				sprintf(path, "uires\\facecover\\%s.jpg", m_cameraList.GetAt(index).deviceid.c_str());
				WinClient.Download(m_cameraList.GetAt(index).thumbnail, path);
				wstring error = WinClient.GetErrorMsg();
				index++;
				if (pEvt->retOK)
				{
					printf("retOK");
				}
			}
			break;
		}
		default:
			break;
	}
		
	SNotifyCenter::getSingleton().FireEventAsync(pEvt);
	pEvt->Release();
	return 0;
}

void CGroupDlg::OnStartHttpThread(LPVOID data)
{
	if (IsRunning()) return;
	SNotifyCenter::getSingleton().addEvent(EVENTID(GroupHttpThread));

	BeginThread(data);
}

bool CGroupDlg::OnGroupHttpThread(EventArgs *e)
{
	GroupHttpThread *pEvt = sobj_cast<GroupHttpThread>(e);
	string response = pEvt->nData;

	if (response.empty())
	{
		return false;
	}

	switch (pEvt->opt)
	{
		case OPT_GETDEVICEINFO:
			if (pEvt->retOK)
			{

			}
		case OPT_GETNETWORKINFO:
			if (pEvt->retOK)
			{				
				//MessageBox(NULL, _T("返回正确”"), _T("账号或密码错误"), MB_OK | MB_USERICON);
				if (!m_pAdapter) break;
				m_pAdapter->SetTags(&m_cameraList);
				SetRefreshDisplay(FALSE);
				GetThumbnailImg();
			}
			else
				MessageBox(NULL, _T("获取设备列表失败"), _T("获取设备信息失败"), MB_OK | MB_ICONERROR);
			break;
		case OPT_GETUSERINFO:
			if (pEvt->retOK)
			{
				if(m_pAdapter)
					m_pAdapter->SetTags(&m_cameraList);
				//MessageBox(NULL, _T("返回正确”"), _T("账号或密码错误"), MB_OK | MB_USERICON);
			}
			else
				//MessageBox(NULL, _T("请输入正确的爱耳目账号密码。\n百度账号用户请点击“使用百度账号登录”"), _T("账号或密码错误"), MB_OK | MB_ICONERROR);
			break;
		case OPT_GETTHUMBNAIL:
			m_pAdapter->refresh();
			break;
		default:
			break;

	}
	return true;
}
