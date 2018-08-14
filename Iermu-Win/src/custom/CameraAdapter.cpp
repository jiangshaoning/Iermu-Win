
#include "stdafx.h"
#include <future>   
#include "CameraAdapter.h"
#include "json.h"
#include "CppSQLite3.h"
#include "UpnpTool.h"
#include "GroupDlg.h"

#define	EXTRANET_LINKS

extern int fisheyePlayer();
int g_pos = 0;
static DWORD WINAPI GetLanIPThreadProc(void *pArguments)
{
	CCameraAdapter *adapter = (CCameraAdapter *)pArguments;
	Json::Reader reader;
	Json::Value jsonobj;
	CHttpConnect WinClient;

	string response = WinClient.Request(GETDEVICEINFO_URL, POST, adapter->m_clickurl);

	if (!reader.parse(response, jsonobj))
		return -1;

#ifdef EXTRANET_LINKS
	int stat = atoi(jsonobj["status"].asString().c_str());
	if (!(stat > 0 && (stat & 4) == 4))
		return false;

	string url = jsonobj["url"].asString();
	if (url.empty())
	{
		return -1;
	}
#else
	string wifi = jsonobj["wifi"].asString();
	//system("ping -n 192.168.1.1");

	string ip = jsonobj["ip"].asString();
	string url = "rtmp://";
	url.append(ip).append(":1935/").append(adapter->m_cameraList->GetAt(g_pos).deviceid);
#endif
	memset(adapter->m_playurl, 0, sizeof(adapter->m_playurl));
	strcpy(adapter->m_playurl,(char *)url.c_str());
	//strcpy(adapter->m_playurl, "test.mp4");
	PostMessage(adapter->m_dlg->m_hWnd, WM_USER_GETPLAYURL, (WPARAM)adapter->m_playurl, (LPARAM)(adapter->m_cameraList->GetAt(g_pos).panoTemplate.c_str()));

	return 0;
}

CCameraAdapter::CCameraAdapter(CGroupDlg *dlg)
{
	m_dlg = dlg;
	m_cameraList = &(dlg->m_cameraList);

	string path_skin = CAMERAFACECOVER;
	path_skin.append("\\iermu_thumb.png");
	string path_off = CAMERAFACECOVER;
	path_off.append("\\listCameraOff.png");


	SStringA imgPath = path_skin.c_str();
	m_Bitmap_skin = SResLoadFromFile::LoadImage(S_CA2T(imgPath));

	imgPath = path_off.c_str();
	m_Bitmap_off = SResLoadFromFile::LoadImage(S_CA2T(imgPath));
}

CCameraAdapter::~CCameraAdapter()
{
}

void CCameraAdapter::CameraAdapterClose()
{
	if (m_Bitmap_skin)
	{
		m_Bitmap_skin->Release();
		m_Bitmap_skin = NULL;
	}
		
	if (m_Bitmap_off)
	{
		m_Bitmap_off->Release();
		m_Bitmap_off = NULL;
	}
		
}


CCameraAdapter::CCameraAdapter(SArray<CameraItem> *cList)
{
	SetTags(cList);
}

//void CCameraAdapter::add(CameraItem &item)
//{
//	m_cameraList.Add(item);
//	notifyDataSetChanged();
//}

void CCameraAdapter::refresh()
{
	notifyDataSetChanged();
}

BOOL CCameraAdapter::IsFileExist(const SStringT& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

void CCameraAdapter::SetTags(SArray<CameraItem> *cList)
{
	m_cameraList = cList;
	//m_cameraList.RemoveAll();
	//for (int i = 0; i<tags.GetCount(); i++)
	//{
	//	CameraItem obj = tags[i];
	//	m_cameraList.Add(obj);
	//}
	qsort(m_cameraList->GetData(), m_cameraList->GetCount(), sizeof(CameraItem), TagCheckCmp);
	notifyDataSetChanged();
}


bool CCameraAdapter::OnImageClick(EventArgs *pEvt)
{
	string url;
	//char localurl[128] = { 0 };
	Json::Reader reader;
	Json::Value jsonobj;
	SImageWnd *pImg = sobj_cast<SImageWnd>(pEvt->sender);
	g_pos = pImg->GetRoot()->GetUserData();

#ifdef EXTRANET_LINKS
	m_clickurl = "access_token=";
	m_clickurl.append(m_dlg->m_token).append("&deviceid=").append(m_cameraList->GetAt(g_pos).deviceid).append("&lang=en&method=liveplay");
#else
	m_clickurl = "access_token=";
	m_clickurl.append(m_dlg->m_token).append("&deviceid=").append(m_cameraList->GetAt(g_pos).deviceid).append("&lang=en&type=info&method=setting");
#endif
	HANDLE hThrd = CreateThread(NULL, 0, GetLanIPThreadProc, (LPVOID)this, 0, NULL);
	//fisheyePlayer();

	//url = "https://api.iermu.com/v2/pcs/device?method=vod&access_token=1.j095DeA6kUL9oqZrcdmm.2592000.1503303685.f576325160a6cc05139ec5386e99a89d&deviceid=137893759883&st=1503331369&et=1503332316";
	//SMessageBox(NULL, SStringT().Format(_T("button of %d item was clicked"), iItem), _T("haha"), MB_OK);
	//UpnpTool pnp;
	//SArray<DeviceIDAndIP> devlist;
	//pnp.upnpDiscover(3000, devlist);
	//sprintf(localurl, "rtmp://%s:1935/live/%s", devlist.GetAt(0).ip, devlist.GetAt(0).id);

	return true;
}
