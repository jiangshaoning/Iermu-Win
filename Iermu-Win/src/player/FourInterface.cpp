#include "stdafx.h"
#include "Resource.h"
#include "FourInterface.h"
#define RELEASEPLAYER(player) \
if (player) \
{ \
	player_close(player); \
	player = NULL; \
}


LRESULT CALLBACK WinMainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM LParam);
INT_PTR CALLBACK DlgSingleProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK DlgFourProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndChildProc(HWND, UINT, WPARAM, LPARAM);


CFourInterface::CFourInterface() :m_hplayer(NULL)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		g_hplayer[i] = NULL;
	}
}


CFourInterface::~CFourInterface()
{
	int i;
	RELEASEPLAYER(m_hplayer);
	for (i = 0; i < 4; i++)
	{
		RELEASEPLAYER(g_hplayer[i]);
	}
}

//static DWORD WINAPI PlayerOpenThreadProc(LPVOID lpParam)
//{
//	CFourInterface *dlg = (CFourInterface*)lpParam;
//	if (!dlg) return 0;
//
//	while (1) {
//		WaitForSingleObject(dlg->m_hEvent, -1);
//
//		// stop player first
//		RELEASEPLAYER(dlg->m_hplayer);
//		if (dlg->m_bClose) break;
//
//		dlg->m_hplayer = player_open(dlg->m_url, dlg->m_hDialog, &dlg->m_Params);
//		if (dlg->m_hplayer) {
//			int param = 0;
//			//++ set dynamic player params
//			//          param = 150; player_setparam(dlg->m_ffPlayer, PARAM_PLAY_SPEED    , &param);
//
//			// software volume scale -30dB to 12dB
//			// range for volume is [-182, 73]
//			// -255 - mute, +255 - max volume, 0 - 0dB
//			param = -0;  player_setparam(dlg->m_hplayer, PARAM_AUDIO_VOLUME, &param);
//
//			//          param = 1;   player_setparam(dlg->m_ffPlayer, PARAM_VFILTER_ENABLE, &param);
//			//-- set dynamic player params
//		}
//		PostMessage((HWND)dlg->m_hDialog, MSG_FFPLAYER, dlg->m_hplayer ? MSG_OPEN_DONE : MSG_OPEN_FAILED, 0);
//	}
//
//	return 0;
//}

void CFourInterface::initSingleWin(HWND parent, char *temp)
{
	RELEASEPLAYER(m_hplayer);
	if (m_hDialog)
	{
		DestroyWindow(m_hDialog);
		m_hDialog = 0;
	}
		
	m_hDialog = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PLAYSINGLE), parent, (DLGPROC)DlgSingleProc);
	if (m_hDialog)
	{
		::SetWindowLong(m_hDialog, GWL_USERDATA, (LONG)this);
		//hStatic = GetDlgItem(hDialog, IDD_RECT_PLAY);
		//显示对话框  
		ShowWindow(m_hDialog, SW_NORMAL);

		//CreateChildWin(hDialog);
	}

	memset(&m_Params, 0, sizeof(m_Params));
	m_Params.adev_render_type = ADEV_RENDER_TYPE_WAVEOUT;
	if (strlen(temp) > 0)
	{
		m_Params.vdev_render_type = VDEV_RENDER_TYPE_EAPIL;
		strcpy(m_Params.eapil_template, temp);
	}
	else
		m_Params.vdev_render_type = VDEV_RENDER_TYPE_GDI;
	m_Params.init_timeout = 10000;
}

void CFourInterface::initFourWin(HWND parent)
{
	if (hDialog4)
	{
		//DestroyWindow(hDialog4);
		//hDialog4 = 0;
		return;
	}

	hDialog4 = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PLAYFOUR), NULL, (DLGPROC)DlgFourProc);
	if (hDialog4)
	{
		::SetWindowLong(hDialog4, GWL_USERDATA, (LONG)this);

		//显示对话框  
		ShowWindow(hDialog4, SW_NORMAL);
	}
	memset(&m_Params, 0, sizeof(m_Params));
	m_Params.adev_render_type = ADEV_RENDER_TYPE_WAVEOUT;
	m_Params.vdev_render_type = VDEV_RENDER_TYPE_GDI;
	m_Params.init_timeout = 10000;
}

void CFourInterface::startPlay(const char* playurl)
{
	memset(m_url, 0, MAX_PATH);
	strcpy(m_url, playurl);

	m_hplayer = player_open(m_url, m_hDialog, &m_Params);
}

void CFourInterface::startPlay1(const char* playurl1)
{
	RELEASEPLAYER(g_hplayer[0]);

	g_hplayer[0] = player_open((char*)playurl1, hStatic1, &m_Params);


	url1 = (char*)playurl1;
	int msg = MSG_FFPLAYER1;

	player_play(g_hplayer[0]);
	player_setparam(g_hplayer[0], PARAM_FFPLAYER_MSG, &msg);
	player_setparam(g_hplayer[0], PARAM_FFPLAYER_DIALOG, &hDialog4);

	//int mode = VIDEO_MODE_STRETCHED;
	//player_setparam(g_hplayer, PARAM_VIDEO_MODE, &mode);
	//int speed = 120;
	//player_setparam(g_hplayer, PARAM_PLAY_SPEED, &speed);
}

void CFourInterface::startPlay2(const char* playurl2)
{
	RELEASEPLAYER(g_hplayer[1]);

	g_hplayer[1] = player_open((char*)playurl2, hStatic2, &m_Params);

	url2 = (char*)playurl2;
	int msg = MSG_FFPLAYER2;

	player_play(g_hplayer[1]);
	player_setparam(g_hplayer[1], PARAM_FFPLAYER_MSG, &msg);
	player_setparam(g_hplayer[1], PARAM_FFPLAYER_DIALOG, &hDialog4);
}

void CFourInterface::startPlay3(const char* playurl3)
{
	RELEASEPLAYER(g_hplayer[2]);

	g_hplayer[2] = player_open((char*)playurl3, hStatic3, &m_Params);
	
	url3 = (char*)playurl3;
	int msg = MSG_FFPLAYER3;

	player_play(g_hplayer[2]);
	player_setparam(g_hplayer[2], PARAM_FFPLAYER_MSG, &msg);
	player_setparam(g_hplayer[2], PARAM_FFPLAYER_DIALOG, &hDialog4);
}

void CFourInterface::startPlay4(const char* playurl4)
{
	RELEASEPLAYER(g_hplayer[3]);

	g_hplayer[3] = player_open((char*)playurl4, hStatic4, &m_Params);
	
	url4 = (char*)playurl4;
	int msg = MSG_FFPLAYER4;

	player_play(g_hplayer[3]);
	player_setparam(g_hplayer[3], PARAM_FFPLAYER_MSG, &msg);
	player_setparam(g_hplayer[3], PARAM_FFPLAYER_DIALOG, &hDialog4);
	
}
int CFourInterface::ffplaySingleStop()
{
	//DestroyWindow(m_hDialog);
	RELEASEPLAYER(m_hplayer);
	DestroyWindow(m_hDialog);
	m_hDialog = 0;
	return 0;
}

int CFourInterface::ffplaySingleWithUrl(const char* playurl, char *temp)
{
	ffplaySingleStop();
	if (strlen(temp) > 0) strcpy(m_temp, temp);
	initSingleWin(NULL, m_temp);
	startPlay(playurl);
	return 0;
}

void CFourInterface::ffplaySingleLButtonDown(int x, int y)
{
	if (m_hplayer)
		player_leftbtndown(m_hplayer, x, y);
}

void CFourInterface::ffplaySingleLButtonUp()
{
	if (m_hplayer)
		player_leftbtnup(m_hplayer);
}

void CFourInterface::ffplaySingleMouseMove(int x, int y)
{
	if (m_hplayer)
		player_mousemove(m_hplayer, x, y);
}

void CFourInterface::ffplaySingleMouseWheel(short delta)
{
	if (m_hplayer)
		player_mousewheel(m_hplayer, delta);
}

int CFourInterface::ffplayFourStop()
{
	int i;
	for (i = 0; i < 4; i++)
	{
		RELEASEPLAYER(g_hplayer[i]);
	}
	DestroyWindow(hDialog4);
	hDialog4 = 0;
	return 0;
}


int CFourInterface::ffplayFourWithUrl(const char* playurl)
{
	int i = 0;
	initFourWin(NULL);
	for (i = 0; i < 4; i++)
	{
		int status = 0;
		if (g_hplayer[i])
		{
			if (!player_getstatus(g_hplayer[i]))
				break;
		}
		else
			break;
	}
	switch (i)
	{
		case 0:
			startPlay1(playurl);
			break;
		case 1:
			startPlay2(playurl);
			break;
		case 2:
			startPlay3(playurl);
			break;
		case 3:
			startPlay4(playurl);
			break;
		default:
			startPlay1(playurl);
			break;
	}

	//MSG msg;
	//while (GetMessage(&msg, NULL, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	return 0;
}


//LRESULT CALLBACK WinMainProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM LParam)
//{
//	HWND hdlg;
//	switch (msg)
//	{
//		case WM_CREATE:
//			hdlg = CreateDialog(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_PLAYFOUR), hwnd, (DLGPROC)DlgProc);
//			if (hdlg)
//			{
//				显示对话框  
//				ShowWindow(hdlg, SW_NORMAL);
//			}
//			return 0;
//		case WM_DESTROY:
//			PostQuitMessage(0);
//			return 0;
//		default:
//			return DefWindowProc(hwnd, msg, wParam, LParam);
//	}
//	return 0;
//}


// “关于”框的消息处理程序。
INT_PTR CALLBACK DlgSingleProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	int param = 0;
	char url[MAX_PATH] = { 0 };
	CFourInterface *pThisInstance = (CFourInterface*)::GetWindowLong(hDlg, GWL_USERDATA);
	//if (pThisInstance)
		//SLOGFMTE("message =%x   hDlg=%x  hStatic=%x   hDialog=%x\n", message, hDlg, pThisInstance->hStatic, pThisInstance->hDialog);
	switch (message)
	{
		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE)
			{
				// 如果执行了关闭  
				// 销毁对话框，将收到WM_DESTROY消息  
				if (pThisInstance)
				{
					pThisInstance->ffplaySingleStop();
				}
				else
				{
					DestroyWindow(hDlg);
				};
				//PostQuitMessage(0);

			}
			break;
		case WM_SIZE:
			{
				LONG width = LOWORD(lParam); // width of client area
				LONG height = HIWORD(lParam); // height of client area

				if (pThisInstance)
				{
					player_setrect(pThisInstance->m_hplayer, 0, 0, 0, width, height);
				}
			}
			break;

		case MSG_FFPLAYER:
			if (pThisInstance)
			{
				switch (wParam)
				{
					case MSG_PLAY_COMPLETED:
						strcpy(url, pThisInstance->m_url);
						//pThisInstance->ffplaySingleStop();
						pThisInstance->ffplaySingleWithUrl(url, pThisInstance->m_temp);											
						break;
					case MSG_OPEN_DONE:
						param = -0;  player_setparam(pThisInstance->m_hplayer, PARAM_AUDIO_VOLUME, &param);
						player_play(pThisInstance->m_hplayer);
						break;
					case MSG_OPEN_FAILED:
						//RELEASEPLAYER(pThisInstance->m_hplayer);
						//SetEvent(pThisInstance->m_hEvent);
						strcpy(url, pThisInstance->m_url);
						pThisInstance->ffplaySingleStop();
						pThisInstance->ffplaySingleWithUrl(url, pThisInstance->m_temp);
						break;

				}
			}
			break;
		case WM_LBUTTONDOWN:
			if (pThisInstance)
			{
				int x = LOWORD(lParam); // width of client area
				int y = HIWORD(lParam); // height of client area

				pThisInstance->ffplaySingleLButtonDown(x, y);
			}		
			break;
		case WM_LBUTTONUP:
			if (pThisInstance)
			{
				pThisInstance->ffplaySingleLButtonUp();
			}
			break;
		case WM_MOUSEMOVE:
			if (pThisInstance)
			{
				int x = LOWORD(lParam); // width of client area
				int y = HIWORD(lParam); // height of client area

				pThisInstance->ffplaySingleMouseMove(x, y);
			}
			break;
		case WM_MOUSEWHEEL:
			if (pThisInstance)
			{
				short delat = (short)HIWORD(wParam);

				pThisInstance->ffplaySingleMouseWheel(delat);
			}
			break;
	}
	return (INT_PTR)FALSE;
}



// “关于”框的消息处理程序。
INT_PTR CALLBACK DlgFourProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	CFourInterface *pThisInstance = (CFourInterface*)::GetWindowLong(hDlg, GWL_USERDATA);
	//if (pThisInstance)
	//SLOGFMTE("message =%x   hDlg=%x  hStatic=%x   hDialog=%x\n", message, hDlg, pThisInstance->hStatic, pThisInstance->hDialog);
	switch (message)
	{
		case WM_INITDIALOG: 
			{
				RECT dlrect;
				GetClientRect(hDlg, &dlrect);
				LONG width = dlrect.right - dlrect.left;
				LONG height = dlrect.bottom - dlrect.top;
				if (pThisInstance)
				{
					pThisInstance->hStatic1 = GetDlgItem(hDlg, IDD_RECT_PLAY1);
					MoveWindow(pThisInstance->hStatic1, 0, 0, width / 2, height / 2, true);
					pThisInstance->hStatic2 = GetDlgItem(hDlg, IDD_RECT_PLAY2);
					MoveWindow(pThisInstance->hStatic2, width / 2, 0, width / 2, height / 2, true);
					pThisInstance->hStatic3 = GetDlgItem(hDlg, IDD_RECT_PLAY3);
					MoveWindow(pThisInstance->hStatic3, 0, height / 2, width / 2, height / 2, true);
					pThisInstance->hStatic4 = GetDlgItem(hDlg, IDD_RECT_PLAY4);
					MoveWindow(pThisInstance->hStatic4, width / 2, height / 2, width / 2, height / 2, true);
				}
			}

			return (INT_PTR)TRUE;

		case WM_SHOWWINDOW:
			{
				RECT dlrect;
				GetClientRect(hDlg, &dlrect);
				LONG width = dlrect.right - dlrect.left;
				LONG height = dlrect.bottom - dlrect.top;
				if (pThisInstance)
				{
					pThisInstance->hStatic1 = GetDlgItem(hDlg, IDD_RECT_PLAY1);
					MoveWindow(pThisInstance->hStatic1, 0, 0, width / 2, height / 2, true);
					pThisInstance->hStatic2 = GetDlgItem(hDlg, IDD_RECT_PLAY2);
					MoveWindow(pThisInstance->hStatic2, width / 2, 0, width / 2, height / 2, true);
					pThisInstance->hStatic3 = GetDlgItem(hDlg, IDD_RECT_PLAY3);
					MoveWindow(pThisInstance->hStatic3, 0, height / 2, width / 2, height / 2, true);
					pThisInstance->hStatic4 = GetDlgItem(hDlg, IDD_RECT_PLAY4);
					MoveWindow(pThisInstance->hStatic4, width / 2, height / 2, width / 2, height / 2, true);
				}
			}
			break;
		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				if (pThisInstance)
				{
					pThisInstance->ffplayFourStop();
				}
				else
				{
					DestroyWindow(hDlg);
				};
				//PostQuitMessage(0);
				return (INT_PTR)TRUE;
			}
			//switch (LOWORD(wParam))
			//{
			//		//针对框架的命令
			//	case IDC_BUTTON_PLAY1: //这个是 button 的ID	
			//		pThisInstance->startPlay1(pThisInstance->url1);
			//			break;
			//	case IDC_BUTTON_PLAY2: //这个是 button 的ID	
			//		pThisInstance->startPlay2(pThisInstance->url2);
			//			break;
			//	case IDC_BUTTON_PLAY3: //这个是 button 的ID	
			//		pThisInstance->startPlay3(pThisInstance->url3);
			//			break;
			//	case IDC_BUTTON_PLAY4: //这个是 button 的ID	
			//		pThisInstance->startPlay4(pThisInstance->url4);
			//			break;
			//}
			break;
		case WM_SYSCOMMAND:
			if (wParam == SC_CLOSE)
			{
				// 如果执行了关闭  
				// 销毁对话框，将收到WM_DESTROY消息  
				if (pThisInstance)
				{
					pThisInstance->ffplayFourStop();
				}
				else
				{
					DestroyWindow(hDlg);
				};
				//PostQuitMessage(0);

			}
			break;
		case WM_SIZE:
			{
				LONG width = LOWORD(lParam); // width of client area
				LONG height = HIWORD(lParam); // height of client area

				MoveWindow(pThisInstance->hStatic1, 0, 0, width / 2, height / 2, true);
				//player_setrect(pThisInstance->g_hplayer, 1, 0, 0, width / 2, height / 2);
				player_setrect(pThisInstance->g_hplayer[0], 0, 0, 0, width / 2, height / 2);

				MoveWindow(pThisInstance->hStatic2, width / 2, 0, width / 2, height / 2, true);
				player_setrect(pThisInstance->g_hplayer[1], 0, 0, 0, width / 2, height / 2);

				MoveWindow(pThisInstance->hStatic3, 0, height / 2, width / 2, height / 2, true);
				player_setrect(pThisInstance->g_hplayer[2], 0, 0, 0, width / 2, height / 2);

				MoveWindow(pThisInstance->hStatic4, width / 2, height / 2, width / 2, height / 2, true);
				player_setrect(pThisInstance->g_hplayer[3], 0, 0, 0, width / 2, height / 2);
			}
			break;
		case MSG_FFPLAYER:
			if (wParam == MSG_PLAY_COMPLETED)
			{
				if (pThisInstance)
				{
					switch (lParam)
					{
						case MSG_FFPLAYER1:
							//pThisInstance->startPlay1(pThisInstance->url1);
							RELEASEPLAYER(pThisInstance->g_hplayer[0]);
							break;
						case MSG_FFPLAYER2:
							//pThisInstance->startPlay2(pThisInstance->url2);
							RELEASEPLAYER(pThisInstance->g_hplayer[1]);
							break;
						case MSG_FFPLAYER3:
							//pThisInstance->startPlay3(pThisInstance->url3);
							RELEASEPLAYER(pThisInstance->g_hplayer[2]);
							break;
						case MSG_FFPLAYER4:
							//pThisInstance->startPlay4(pThisInstance->url4);
							RELEASEPLAYER(pThisInstance->g_hplayer[3]);
							break;
					}
				}						
			}
			break;
	}
	return (INT_PTR)FALSE;
}
