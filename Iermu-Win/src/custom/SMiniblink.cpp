#include "StdAfx.h"
#include "SMiniblink.h"
#include <Imm.h>
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"node.lib")
namespace SOUI
{
    //////////////////////////////////////////////////////////////////////////
    // SWkeLoader
   

    SWkeLoader::SWkeLoader() 
    {     
		wkeInit();
    }


    SWkeLoader::~SWkeLoader()
    {
		wkeShutdown();
    }
	
    //////////////////////////////////////////////////////////////////////////
    // SWkeWebkit
    
    SWkeWebkit::SWkeWebkit(void):m_pWebView(NULL)
    {
    }

    SWkeWebkit::~SWkeWebkit(void)
    {
    }

    void SWkeWebkit::OnPaint(IRenderTarget *pRT)
    {
		CRect rcClip;
		pRT->GetClipBox(&rcClip);
		CRect rcClient;
		GetClientRect(&rcClient);
		CRect rcInvalid;
		rcInvalid.IntersectRect(&rcClip, &rcClient);
		HDC hdc = pRT->GetDC();
		if (GetStyle().m_byAlpha != 0xff)
		{
			BLENDFUNCTION bf = { AC_SRC_OVER,0,GetStyle().m_byAlpha,AC_SRC_ALPHA };
			AlphaBlend(hdc, rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height(),wkeGetViewDC((wkeWebView) m_pWebView), rcInvalid.left - rcClient.left, rcInvalid.top - rcClient.top, rcInvalid.Width(), rcInvalid.Height(), bf);
		}
		else
		{
			BitBlt(hdc, rcInvalid.left, rcInvalid.top, rcInvalid.Width(), rcInvalid.Height(), wkeGetViewDC((wkeWebView)m_pWebView), rcInvalid.left - rcClient.left, rcInvalid.top - rcClient.top, SRCCOPY);
		}
		pRT->ReleaseDC(hdc);
    }

    void SWkeWebkit::OnSize( UINT nType, CSize size )
    {
        __super::OnSize(nType,size);
        wkeResize(m_pWebView,size.cx,size.cy);
    }
	void wkePaintUpdatedCallback(wkeWebView webView, void* param, const HDC hdcSrc, int x, int y, int cx, int cy)
	{
 		SWkeWebkit *_THIS = (SWkeWebkit*)param;
		CRect rcClient;
		_THIS->GetClientRect(&rcClient);
		CRect rcInvalid(CPoint(x, y), CSize(cx, cy));
		rcInvalid.OffsetRect(rcClient.TopLeft());
		_THIS->InvalidateRect(rcInvalid);
	}


	int SWkeWebkit::OnCreate( void * )
    {
        m_pWebView =wkeCreateWebView();
        if(!m_pWebView) return 1;
		wkeLoadURL(m_pWebView, S_CT2A(m_strUrl, CP_UTF8));
		//m_pWebView->setTransparent(true);
		wkeSetNavigationToNewWindowEnable((wkeWebView)m_pWebView, false);
		wkeOnPaintUpdated((wkeWebView)m_pWebView, wkePaintUpdatedCallback, this);
        return 0;
	}

	void SWkeWebkit::OnDestroy()
	{
		if(m_pWebView)
			wkeDestroyWebView((wkeWebView)m_pWebView);
	}

	LRESULT SWkeWebkit::OnMouseEvent( UINT message, WPARAM wParam,LPARAM lParam)
	{
		if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
		{
			SetFocus();
			SetCapture();
		}
		else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
		{
			ReleaseCapture();
		}

		CRect rcClient;
		GetClientRect(&rcClient);

		int x = GET_X_LPARAM(lParam) -rcClient.left;
		int y = GET_Y_LPARAM(lParam) -rcClient.top;

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;
		
		SetMsgHandled(wkeFireMouseEvent(m_pWebView, message, x, y, flags));
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyDown( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(wkeFireKeyDownEvent(m_pWebView,wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnKeyUp( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		SetMsgHandled(wkeFireKeyUpEvent(m_pWebView,wParam, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnMouseWheel( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		POINT pt;
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		CRect rc;
		GetWindowRect(&rc);
		pt.x -= rc.left;
		pt.y -= rc.top;

		int delta = GET_WHEEL_DELTA_WPARAM(wParam);

		unsigned int flags = 0;

		if (wParam & MK_CONTROL)
			flags |= WKE_CONTROL;
		if (wParam & MK_SHIFT)
			flags |= WKE_SHIFT;

		if (wParam & MK_LBUTTON)
			flags |= WKE_LBUTTON;
		if (wParam & MK_MBUTTON)
			flags |= WKE_MBUTTON;
		if (wParam & MK_RBUTTON)
			flags |= WKE_RBUTTON;

		//flags = wParam;
		
		SetMsgHandled(wkeFireMouseWheelEvent(m_pWebView, pt.x, pt.y, delta, flags));

		return 0;
	}

	LRESULT SWkeWebkit::OnChar( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		unsigned int charCode = wParam;
		unsigned int flags = 0;
		if (HIWORD(lParam) & KF_REPEAT)
			flags |= WKE_REPEAT;
		if (HIWORD(lParam) & KF_EXTENDED)
			flags |= WKE_EXTENDED;

		//flags = HIWORD(lParam);

		SetMsgHandled(wkeFireKeyPressEvent(m_pWebView,charCode, flags, false));
		return 0;
	}

	LRESULT SWkeWebkit::OnImeStartComposition( UINT uMsg, WPARAM wParam,LPARAM lParam )
	{
		wkeRect caret = wkeGetCaret(m_pWebView);

		CRect rcClient;
		GetClientRect(&rcClient);

		CANDIDATEFORM form;
		form.dwIndex = 0;
		form.dwStyle = CFS_EXCLUDE;
		form.ptCurrentPos.x = caret.x + rcClient.left;
		form.ptCurrentPos.y = caret.y + caret.h + rcClient.top;
		form.rcArea.top = caret.y + rcClient.top;
		form.rcArea.bottom = caret.y + caret.h + rcClient.top;
		form.rcArea.left = caret.x + rcClient.left;
		form.rcArea.right = caret.x + caret.w + rcClient.left;
		COMPOSITIONFORM compForm;
		compForm.ptCurrentPos=form.ptCurrentPos;
		compForm.rcArea=form.rcArea;
		compForm.dwStyle=CFS_POINT;

		HWND hWnd=GetContainer()->GetHostHwnd();
		HIMC hIMC = ImmGetContext(hWnd);
		ImmSetCandidateWindow(hIMC, &form);
		ImmSetCompositionWindow(hIMC,&compForm);
		ImmReleaseContext(hWnd, hIMC);
		return 0;
	}

	void SWkeWebkit::OnSetFocus(SWND wndOld)
	{
	    __super::OnSetCursor(wndOld);
		wkeSetFocus(m_pWebView);
	}

	void SWkeWebkit::OnKillFocus(SWND wndFocus)
	{
		wkeKillFocus(m_pWebView);
		__super::OnKillFocus(wndFocus);
	}	

	BOOL SWkeWebkit::OnSetCursor( const CPoint &pt )
	{
		int curInf=wkeGetCursorInfoType((wkeWebView)m_pWebView);
		
		HCURSOR hCursor = GETRESPROVIDER->LoadCursor(wkeCursor(curInf));
		SetCursor(hCursor);
		return TRUE;
	}
	LPCTSTR SWkeWebkit::wkeCursor(int wekInfId)
	{
		LPCTSTR curResStr = NULL;
		switch (wekInfId)
		{
		case WkeCursorInfoPointer:
			curResStr = IDC_ARROW; break;
		case WkeCursorInfoCross:
			curResStr = IDC_CROSS; break;
		case WkeCursorInfoHand:
			curResStr = IDC_HAND; break;
		case WkeCursorInfoIBeam:
			curResStr = IDC_IBEAM; break;
		case WkeCursorInfoWait:
			curResStr = IDC_WAIT; break;
		case WkeCursorInfoHelp:
			curResStr = IDC_HELP; break;
		case WkeCursorInfoEastResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoNorthEastResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoNorthWestResize:
			curResStr = IDC_SIZENWSE; break;
		case WkeCursorInfoSouthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoSouthEastResize:
			curResStr = IDC_SIZENWSE; break;
		case WkeCursorInfoSouthWestResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoWestResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthSouthResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoEastWestResize:
			curResStr = IDC_SIZEWE; break;
		case WkeCursorInfoNorthEastSouthWestResize:
			//curResStr = IDC_CROSS; break;
		case WkeCursorInfoNorthWestSouthEastResize:
			curResStr = IDC_SIZENESW; break;
		case WkeCursorInfoColumnResize:
			curResStr = IDC_SIZENS; break;
		case WkeCursorInfoRowResize:
			curResStr = IDC_SIZEWE; break;
		case 29:
			curResStr = IDC_SIZEALL; break;
		default:curResStr = IDC_ARROW; break;
		}
		return curResStr;
	}
	BOOL SWkeWebkit::OnAttrUrl( SStringW strValue, BOOL bLoading )
	{
		m_strUrl=strValue;
		if(!bLoading&&m_pWebView) 
			wkeLoadURL(m_pWebView, S_CT2A(m_strUrl, CP_UTF8));
		return !bLoading;
	}



}

