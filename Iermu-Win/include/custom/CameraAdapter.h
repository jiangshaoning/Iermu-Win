#pragma once
#include <helper/SAdapterBase.h>
#include "HttpData.h"
#include "FourInterface.h"

typedef enum
{
	TYPE_STATUS_OFFLINE,
	TYPE_STATUS_POWEROFF,
	TYPE_STATUS_ONLINE
}CameraStatus;

typedef struct
{
	SStringT		tag;
	CameraStatus	status;			//摄像机状态 在线离线关机
	int				con_type;		//摄像机平台 羚羊或百度
	string			deviceid;
	string			thumbnail;		//封面图片
	string			description;
	string			panoTemplate;	//全景模板
}CameraItem;

class CGroupDlg;

class CCameraAdapter : public SAdapterBase
{	
public:
	CFourInterface		m_player;
	SArray<CameraItem>  *m_cameraList;
	CGroupDlg           *m_dlg;
	string				m_clickurl;
	char 				m_playurl[128];

	CCameraAdapter(CGroupDlg *dlg);
	~CCameraAdapter();
	CCameraAdapter(SArray<CameraItem> *cList);
	void SetTags(SArray<CameraItem> *cList);
	//void add(CameraItem &item);
	void refresh();
	void playVideo(string url);
	void CameraAdapterClose();
protected:
	static int TagCheckCmp(const void * p1, const void*p2)
	{
		const CameraItem *tag1=(const CameraItem*)p1;
		const CameraItem *tag2=(const CameraItem*)p2;
		return tag1->tag.Compare(tag2->tag);
	}
	virtual int getCount()
	{
		return m_cameraList->GetCount();
	}

	virtual void getView(int position, SWindow *pItem, pugi::xml_node xmlTemplate)
	{
		if (pItem->GetChildrenCount() == 0)
		{
			pItem->InitFromXml(xmlTemplate);
		}
		CameraStatus status = m_cameraList->GetAt(position).status;
		SImageWnd *pImg = pItem->FindChildByName2<SImageWnd>(L"img_file_icon");
		SWindow* camera_status = pItem->FindChildByName(L"camera_status");

		if (pImg)
		{
			IBitmap *pBitmap =NULL;
			string path = CAMERAFACECOVER;
			path.append("\\").append(m_cameraList->GetAt(position).deviceid).append(".jpg");
			SStringA imgPath = path.c_str();
			SStringT imgPahtT = S_CA2T(imgPath);

			switch (status)
			{
				case TYPE_STATUS_OFFLINE:
					if (IsFileExist(imgPahtT))
						pBitmap = SResLoadFromFile::LoadImage(imgPahtT);

					if (pBitmap)
					{
						pImg->SetImage(pBitmap, kHigh_FilterLevel);
						pBitmap->Release();
					}
					else
					{
						if (m_Bitmap_skin)
							pImg->SetImage(m_Bitmap_skin, kHigh_FilterLevel);
						else
							pImg->SetSkin(GETSKIN(L"iermu_thumb", pImg->GetScale()));
					}
					if (camera_status)
					{
						camera_status->SetWindowText(L"离线");
						camera_status->SetAttribute(L"colorBkgnd", L"@color/gray", FALSE);
					}
					break;
				case TYPE_STATUS_ONLINE:
					if (IsFileExist(imgPahtT))
						pBitmap = SResLoadFromFile::LoadImage(imgPahtT);

					if (pBitmap)
					{
						pImg->SetImage(pBitmap, kHigh_FilterLevel);
						pBitmap->Release();
					}
					else
					{
						if (m_Bitmap_skin)
							pImg->SetImage(m_Bitmap_skin, kHigh_FilterLevel);
						else
							pImg->SetSkin(GETSKIN(L"iermu_thumb", pImg->GetScale()));
					}
					if (camera_status)
					{
						camera_status->SetWindowText(L"在线");
						camera_status->SetAttribute(L"colorBkgnd", L"@color/hblue", FALSE);
					}

					break;
				case TYPE_STATUS_POWEROFF:
					if (m_Bitmap_off)
						pImg->SetImage(m_Bitmap_off, kHigh_FilterLevel);
					else
						pImg->SetSkin(GETSKIN(L"iermu_thumb", pImg->GetScale()));

					if (camera_status)
					{
						camera_status->SetWindowText(L"关机");
						camera_status->SetAttribute(L"colorBkgnd", L"@color/gray", FALSE);
					}

					break;
				default:
					if (m_Bitmap_skin)
						pImg->SetImage(m_Bitmap_skin, kHigh_FilterLevel);
					else
						pImg->SetSkin(GETSKIN(L"iermu_thumb", pImg->GetScale()));

					if (camera_status)
					{
						camera_status->SetWindowText(L"离线");
						camera_status->SetAttribute(L"colorBkgnd", L"@color/gray", FALSE);
					}
					break;
			}
			
			pImg->GetRoot()->SetUserData(position);
			pImg->GetEventSet()->subscribeEvent(EVT_CMD, Subscriber(&CCameraAdapter::OnImageClick, this));
		}

		SWindow* camera_description = pItem->FindChildByName(L"camera_description");
		if (camera_description)
		{
			//SLOGFMTE("#####################  %s  ##################\n", m_CInfo[position].description.c_str());
			SStringA description = S_CA2A(m_cameraList->GetAt(position).description.c_str());
			camera_description->SetWindowText(S_CA2T(description));
		}
		//SButton *pbtn = pItem->FindChildByName2<SButton>(L"btn_test");
		//pbtn->SetWindowTextW(SStringT().Format(_T("btn %d"), position));
		//pbtn->getroot()->setuserdata(position);
		//pbtn->geteventset()->subscribeevent(evt_cmd, subscriber(&ctileadapter::onbuttonclick, this));
	}

	bool OnImageClick(EventArgs *pEvt);
	BOOL IsFileExist(const SStringT& csFile);
private:
	int			m_Count = 0;
	IBitmap		*m_Bitmap_skin;
	IBitmap		*m_Bitmap_off;
};