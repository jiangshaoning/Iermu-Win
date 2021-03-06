// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "MainDlg.h"
#include "GroupDlg.h"
#include "SImageMaskWnd.h"
#include "SSwitch.h"
#include "SouiRealWndHandler.h"

//从PE文件加载，注意从文件加载路径位置
#define RES_TYPE 1
// #define RES_TYPE 1  //从PE资源中加载UI资源

	
//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP
	
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));

    int nRet = 0;
    
    SComMgr *pComMgr = new SComMgr;

    //将程序的运行路径修改到项目所在目录所在的目录
    TCHAR szCurrentDir[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));
    LPTSTR lpInsertPos = _tcsrchr(szCurrentDir, _T('\\'));
    _tcscpy(lpInsertPos + 1, _T("..\\Iermu-Win"));
    SetCurrentDirectory(szCurrentDir);
    {
        BOOL bLoaded=FALSE;
        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
        bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [render] failed!"));
        bLoaded=pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
        SASSERT_FMT(bLoaded,_T("load interface [%s] failed!"),_T("imgdecoder"));

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
        SApplication *theApp = new SApplication(pRenderFactory, hInstance);
        {
            CAutoRefPtr<IResProvider> sysResProvider;
            CreateResProvider(RES_PE, (IObjRef**)&sysResProvider);
            sysResProvider->Init((WPARAM)hInstance, 0);
            theApp->LoadSystemNamedResource(sysResProvider);
        }

        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
        if (!pResProvider->Init((LPARAM)_T("uires"), 0))
        {
            SASSERT(0);
            return 1;
        }
#else 
        CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance, 0);
#endif
		SWkeLoader wkeLoader;
		theApp->RegisterWindowClass<SWkeWebkit>();
		theApp->InitXmlNamedID(namedXmlID,ARRAYSIZE(namedXmlID),TRUE);
		theApp->RegisterWindowClass<SImageMaskWnd>();//注册SImageMaskWnd
		theApp->RegisterWindowClass<SSwitch>();//注册Switch
        theApp->AddResProvider(pResProvider);
		
		//设置真窗口处理接口
		CSouiRealWndHandler * pRealWndHandler = new CSouiRealWndHandler();
		theApp->SetRealWndHandler(pRealWndHandler);
		pRealWndHandler->Release();

		SNotifyCenter *pNotifyCenter = new SNotifyCenter;
        // BLOCK: Run application
        {
            CMainDlg dlgMain;
            dlgMain.Create(GetActiveWindow());
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet = theApp->Run(dlgMain.m_hWnd);
        		
			if (dlgMain.GetReturnCode() == 1)
			{
				string nToken = dlgMain.GetToken();
				string nUid = dlgMain.GetUID();
				CGroupDlg dlgGroup(nUid, nToken);
				dlgGroup.Create(GetActiveWindow());
				dlgGroup.SendMessage(WM_INITDIALOG);
				dlgGroup.CenterWindow(dlgGroup.m_hWnd);

				dlgGroup.ShowWindow(SW_SHOWNORMAL);
				nRet = theApp->Run(dlgGroup.m_hWnd);
			}		
		}
		delete pNotifyCenter;
        delete theApp;
    }
    
    delete pComMgr;
    
    OleUninitialize();
    return nRet;
}
