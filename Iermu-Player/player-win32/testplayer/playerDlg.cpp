// playerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "stdio.h"
#include "player.h"
#include "playerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID_FIRST_DIALOG  1
#define TIMER_ID_PROGRESS      2
#define TIMER_ID_HIDE_TEXT     3

static unsigned short AMF_DecodeInt16(const char *data)
{
	unsigned char *c = (unsigned char *)data;
	unsigned short val;
	val = (c[0] << 8) | c[1];
	return val;
}

static int BinaryBytes2String(const unsigned char* pSrc, int nSrcLength, char* pDst)
{
	if (pSrc == 0 || pDst == 0)
		return 0;

	const char tab[] = "0123456789abcdef";

	for (int i = 0; i<nSrcLength; i++)
	{
		*pDst++ = tab[*pSrc >> 4];
		*pDst++ = tab[*pSrc & 0x0f];
		pSrc++;
	}

	*pDst = '/0';

	return nSrcLength * 2;
}

static void get_app_dir(char *path, int size)
{
    HMODULE handle = GetModuleHandle(NULL);
    GetModuleFileNameA(handle, path, size);
    char  *str = path + strlen(path);
    while (*--str != '\\');
    *str = '\0';
}

static void load_fanplayer_params(PLAYER_INIT_PARAMS *params)
{
    char  file[MAX_PATH];
    FILE *fp = NULL;
    char *buf= NULL;
    int   len= 0;

    // clear params
    memset(params, 0, sizeof(PLAYER_INIT_PARAMS));

    // open params file
    get_app_dir(file, MAX_PATH);
    strcat(file, "\\fanplayer.ini");
    fp = fopen(file, "rb");

    if (fp) {
        fseek(fp, 0, SEEK_END);
        len = ftell(fp);
        buf = (char*)malloc(len);
        if (buf) {
            fseek(fp, 0, SEEK_SET);
            fread(buf, len, 1, fp);
            player_load_params(params, buf);
            free(buf);
        }
        fclose(fp);
    }
}

static void save_fanplayer_params(PLAYER_INIT_PARAMS *params)
{
    char  file[MAX_PATH];
    FILE *fp = NULL;

    // open params file
    get_app_dir(file, MAX_PATH);
    strcat(file, "\\fanplayer.ini");
    fp = fopen(file, "wb");

    if (fp) {
        fprintf(fp, "video_stream_cur   = %d;\r\n", params->video_stream_cur   );
        fprintf(fp, "video_thread_count = %d;\r\n", params->video_thread_count );
        fprintf(fp, "video_hwaccel      = %d;\r\n", params->video_hwaccel      );
        fprintf(fp, "video_deinterlace  = %d;\r\n", params->video_deinterlace  );
        fprintf(fp, "video_rotate       = %d;\r\n", params->video_rotate       );
        fprintf(fp, "audio_stream_cur   = %d;\r\n", params->audio_stream_cur   );
        fprintf(fp, "subtitle_stream_cur= %d;\r\n", params->subtitle_stream_cur);
        fprintf(fp, "vdev_render_type   = %d;\r\n", params->vdev_render_type   );
        fprintf(fp, "adev_render_type   = %d;\r\n", params->adev_render_type   );
        fprintf(fp, "init_timeout       = %d;\r\n", params->init_timeout       );
        fclose(fp);
    }
}

// CplayerDlg dialog
CplayerDlg::CplayerDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CplayerDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_ffPlayer    = NULL;
    m_bLiveStream = FALSE;
    m_bResetPlayer= FALSE;
}

void CplayerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

void CplayerDlg::PlayerReset(PLAYER_INIT_PARAMS *params)
{
    player_close(m_ffPlayer);
    m_ffPlayer = player_open(m_strUrl, GetSafeHwnd(), params);
}

int CplayerDlg::irmFlvReadFileTemplate(char *FlvFile, unsigned char *FileLink)
{
	char tbuff[1024], *tp = tbuff;
	FILE* tfd = NULL;
	int tlen = 1024;
	unsigned short len = 0;
	tfd = fopen(FlvFile, "rb");
	if (tfd >0)
	{
		tlen = fread(tbuff, 1, tlen, tfd);
		fclose(tfd);
		while (tlen > 0)
		{
			if (!memcmp(tp, "Templet", 7))
				break;
			tp++;
			tlen--;
		}

		if (tlen > 0)
		{
			len = AMF_DecodeInt16(tp + 8);
			memcpy(FileLink, tp+10, len);
			return len;
		}
	}

	return -1;
}


void CplayerDlg::PlayerOpenFile(TCHAR *file)
{
    CFileDialog dlg(TRUE);
    TCHAR       str[MAX_PATH];
	unsigned char temp[1024] = { 0 };
	char templet[1024] = { 0 };
	int temp_len = 0;
	FILE *fp = NULL;

    // kill player progress timer
    KillTimer(TIMER_ID_PROGRESS);

    // open file dialog
    if (!file) {
        if (dlg.DoModal() == IDOK) {
            wcscpy(str, dlg.GetPathName());
        } else {
            OnOK();
            return;
        }
    } else {
        wcscpy(str, file);
    }
    WideCharToMultiByte(CP_ACP, 0, str, -1, m_strUrl, MAX_PATH, NULL, NULL);

    // set window title
    SetWindowText(TEXT("videoplayer - loading"));

    // player open file
    char ext[MAX_PATH]; _splitpath(m_strUrl, NULL, NULL, NULL, ext);
    if (  strnicmp(m_strUrl, "http://", 7) == 0 && stricmp(ext, ".m3u8") == 0
       || strnicmp(m_strUrl, "rtmp://", 7) == 0
       || strnicmp(m_strUrl, "rtsp://", 7) == 0
       || strnicmp(m_strUrl, "gdigrab://", 10) == 0
       || strnicmp(m_strUrl, "vfwcap", 6) == 0
       || stricmp(ext, ".bmp" ) == 0
       || stricmp(ext, ".jpg" ) == 0
       || stricmp(ext, ".jpeg") == 0
       || stricmp(ext, ".png" ) == 0
       || stricmp(ext, ".gif" ) == 0) {
        m_bLiveStream = TRUE;
    } else {
        m_bLiveStream = FALSE;
    }
	PLAYER_INIT_PARAMS params;
	load_fanplayer_params(&params);
	temp_len = irmFlvReadFileTemplate(m_strUrl, temp);
	if (temp_len > 1)
	{
		temp_len = BinaryBytes2String(temp, temp_len, templet);
		// load fanplayer init params
		params.vdev_render_type = VDEV_RENDER_TYPE_EAPIL;
		memcpy(params.eapil_template, templet, temp_len);
	}
	//strcpy(params.eapil_template, "6c0ca87ff17b22a3c4a1967b41f6c000e79e6deda8e54c2d71b0000eb28e5d0580cc1b86767ee74e441409a2984f8ac9aeb5998e1dc1a5aff3e6e78f9b3179720e2e15d03509dbfad47407982bc49d6a4a7aee9e217d456fe7738f161bd0f580f52df72e69833ee9da8523bfd721fb89cb61922828fe57847fbfb45f3fe40b879ba45260de382ca8c3c0e256b2e73c9b60fdd7201ec06c2a1b3a5b151c98dfeabe263f0d71db7d2848ca845fdb03f3ffa0650e16f34d0089b783aa5184d0633a902d1b8d9f073d0fc3591ce0006c152e110db8ce4a521a5feb39675ceff453495a9ea9168d7e61e6b173ed75594aa0661c18cf40661cd826cc5d9f3635542e0c5e61edb75793eab1ae11b504774e58560304baa43640b0690e045a91c4e53c576c8fe8af3172796534bee503044597b4f804afbf4ed31edd15308bb001dec9e7f3dcb9aec11eb8ac7b18d36900ae2802c9b49efb4a39653e6181c8c020635b162a1b2167c40dfc0d1e94cadf86810a9637df173f7f96d6cb1f6966d8106c2deffd880197f1b18ced564e518438bb9d0a147f1ac3154955b4101b87ad89895a3350c9c7e399f8fed3c3b241c84e29e6eb5e56ba2fe71b7049a74cd594d6445b3d94b7b79e4137e222812a5fa62becc204861a8bd63f719f025d5470a55ca1a1d1db669e8643480c634d7010dad1d0e93c");
    PlayerReset(&params); // reset player
}

void CplayerDlg::PlayerShowText(int time)
{
    player_textout(m_ffPlayer, 20, 20, RGB(0, 255, 0), m_strTxt);
    SetTimer(TIMER_ID_HIDE_TEXT, time, NULL);
}

BEGIN_MESSAGE_MAP(CplayerDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
    ON_WM_CTLCOLOR()
    ON_WM_SIZE()
    ON_COMMAND(ID_OPEN_FILE      , &CplayerDlg::OnOpenFile     )
    ON_COMMAND(ID_VIDEO_MODE     , &CplayerDlg::OnVideoMode    )
    ON_COMMAND(ID_EFFECT_MODE    , &CplayerDlg::OnEffectMode   )
    ON_COMMAND(ID_VRENDER_TYPE   , &CplayerDlg::OnVRenderType  )
    ON_COMMAND(ID_AUDIO_STREAM   , &CplayerDlg::OnAudioStream  )
    ON_COMMAND(ID_VIDEO_STREAM   , &CplayerDlg::OnVideoStream  )
    ON_COMMAND(ID_TAKE_SNAPSHOT  , &CplayerDlg::OnTakeSnapshot )
    ON_COMMAND(ID_STEP_FORWARD   , &CplayerDlg::OnStepForward  )
    ON_COMMAND(ID_PLAY_SPEED_DEC , &CplayerDlg::OnPlaySpeedDec )
    ON_COMMAND(ID_PLAY_SPEED_INC , &CplayerDlg::OnPlaySpeedInc )
    ON_COMMAND(ID_PLAY_SPEED_TYPE, &CplayerDlg::OnPlaySpeedType)
END_MESSAGE_MAP()


// CplayerDlg message handlers

BOOL CplayerDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE );  // Set big icon
    SetIcon(m_hIcon, FALSE);  // Set small icon

    // load accelerators
    m_hAcc = LoadAccelerators(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_ACCELERATOR1)); 

    // TODO: Add extra initialization here
    MoveWindow(0, 0, 800, 480);

    // get dc
    m_pDrawDC = GetDC();

    // setup init timer
    SetTimer(TIMER_ID_FIRST_DIALOG, 100, NULL);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CplayerDlg::OnPaint()
{
    if (IsIconic()) {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width () - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        LONGLONG total = 1, pos = 0;
        player_getparam(m_ffPlayer, PARAM_MEDIA_DURATION, &total);
        player_getparam(m_ffPlayer, PARAM_MEDIA_POSITION, &pos  );
        if (!m_bLiveStream) {
            if (pos > 0) {
                CPaintDC dc(this);
                RECT fill  = m_rtClient;
                fill.right = (LONG)(fill.right * pos / total);
                fill.top   = fill.bottom - 2;
                dc.FillSolidRect(&fill, RGB(250, 150, 0));
                fill.left  = fill.right;
                fill.right = m_rtClient.right;
                dc.FillSolidRect(&fill, RGB(0, 0, 0));
            }
        } else {
            SetWindowText(pos == -1 ? TEXT("videoplayer - buffering") : TEXT("videoplayer"));
        }
#if 0 // for player_testout test
        static int x = 0;
        x++; x %= m_rtClient.right;
        player_textout(m_ffPlayer, x, 10, 0xff00ff00, "testplayer textout test !");
#endif
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CplayerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CplayerDlg::OnDestroy()
{
    CDialog::OnDestroy();
    ReleaseDC(m_pDrawDC);

#if 0
    // save fanplayer init params
    PLAYER_INIT_PARAMS params;
    player_getparam(m_ffPlayer, PARAM_PLAYER_INIT_PARAMS, &params);
    save_fanplayer_params(&params);
#endif

    // close player
    player_close(m_ffPlayer);
    m_ffPlayer = NULL;
}

void CplayerDlg::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent)
    {
    case TIMER_ID_FIRST_DIALOG:
        // kill timer first
        KillTimer(TIMER_ID_FIRST_DIALOG);
        PlayerOpenFile(__argc > 1 ? __wargv[1] : NULL);
        break;

    case TIMER_ID_PROGRESS:
        RECT rect;
        rect.top    = m_rtClient.bottom - 2;
        rect.left   = m_rtClient.left;
        rect.bottom = m_rtClient.bottom;
        rect.right  = m_rtClient.right;
        InvalidateRect(&rect, FALSE);
        break;

    case TIMER_ID_HIDE_TEXT:
        KillTimer(TIMER_ID_HIDE_TEXT);
        player_textout(m_ffPlayer, 0, 0, 0, NULL);
        m_strTxt[0] = '\0';
        break;

    default:
        CDialog::OnTimer(nIDEvent);
        break;
    }
}

void CplayerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	player_leftbtndown(m_ffPlayer, point.x, point.y);
	return;

    if (!m_bLiveStream) {
        if (point.y > m_rtClient.bottom - 8) {
            LONGLONG total = 1;
            player_getparam(m_ffPlayer, PARAM_MEDIA_DURATION, &total);
            KillTimer(TIMER_ID_PROGRESS);
            player_seek(m_ffPlayer, total * point.x / m_rtClient.right, SEEK_PRECISELY);
            SetTimer (TIMER_ID_PROGRESS, 100, NULL);
        } else {
            if (!m_bPlayPause) player_pause(m_ffPlayer);
            else player_play(m_ffPlayer);
            m_bPlayPause = !m_bPlayPause;
        }
    }

    CDialog::OnLButtonDown(nFlags, point);
}

void CplayerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	player_leftbtnup(m_ffPlayer);
	CDialog::OnLButtonUp(nFlags, point);
}

void CplayerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	player_mousemove(m_ffPlayer, point.x, point.y);
	CDialog::OnLButtonUp(nFlags, point);
}

BOOL CplayerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	player_mousewheel(m_ffPlayer, zDelta);
	CDialog::OnMouseWheel(nFlags, zDelta, point);
	return TRUE;
}

HBRUSH CplayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO: Change any attributes of the DC here
    // TODO: Return a different brush if the default is not desired
    if (pWnd == this) return (HBRUSH)GetStockObject(BLACK_BRUSH);
    else return hbr;
}

void CplayerDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    if (nType != SIZE_MINIMIZED) {
        GetClientRect(&m_rtClient);
        player_setrect(m_ffPlayer, 0, 0, 0, cx, cy - 2);
        player_setrect(m_ffPlayer, 1, 0, 0, cx, cy - 2);
    }
}

BOOL CplayerDlg::PreTranslateMessage(MSG *pMsg) 
{
    if (TranslateAccelerator(GetSafeHwnd(), m_hAcc, pMsg)) return TRUE;

    if (pMsg->message == MSG_FFPLAYER) {
        switch (pMsg->wParam)
        {
        case MSG_OPEN_DONE:
            SetWindowText(TEXT("videoplayer"));
            if (TRUE) { // set player dynamic params
                int param = 0;
                //++ set dynamic player params
//              param = 150; player_setparam(m_ffPlayer, PARAM_PLAY_SPEED_VALUE, &param);
//              param = 1  ; player_setparam(m_ffPlayer, PARAM_PLAY_SPEED_TYPE , &param);

                // software volume scale -30dB to 12dB
                // range for volume is [-182, 73]
                // -255 - mute, +255 - max volume, 0 - 0dB
                param = -0;  player_setparam(m_ffPlayer, PARAM_AUDIO_VOLUME, &param);
            }
            player_setrect(m_ffPlayer, 0, 0, 0, m_rtClient.right, m_rtClient.bottom - 2);
            player_setrect(m_ffPlayer, 1, 0, 0, m_rtClient.right, m_rtClient.bottom - 2);
            if (m_bResetPlayer) {
                if (!m_bPlayPause ) player_play(m_ffPlayer);
                if (!m_bLiveStream) player_seek(m_ffPlayer, m_llLastPos, SEEK_PRECISELY);
                if ( m_strTxt[0]  ) PlayerShowText(2000);
                m_bResetPlayer = FALSE;
            } else {
                player_play(m_ffPlayer);
                m_bPlayPause = FALSE;
            }
            SetTimer(TIMER_ID_PROGRESS, 100, NULL);
            break;
        case MSG_PLAY_COMPLETED:
            if (!m_bLiveStream) {
                PlayerOpenFile(NULL);
            }
            break;
        }
        return TRUE;
    }
	else if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam)
		{
			case VK_LEFT://按下左键
				player_setrendertype(m_ffPlayer, 1);
				break;
			case VK_RIGHT://按下右键
				player_setrendertype(m_ffPlayer, 3);
				break;
			case VK_UP://按下上键
				player_setrendertype(m_ffPlayer, 4);
				break;
			case VK_DOWN://按下下键
				player_setrendertype(m_ffPlayer, 2);
				break;
			default:
				break;
		}
		return TRUE;
	}
	else return CDialog::PreTranslateMessage(pMsg);
}

void CplayerDlg::OnOpenFile()
{
    PlayerOpenFile(NULL);
}

void CplayerDlg::OnAudioStream()
{
    PLAYER_INIT_PARAMS params;
    player_getparam(m_ffPlayer, PARAM_PLAYER_INIT_PARAMS, &params);
    player_getparam(m_ffPlayer, PARAM_MEDIA_POSITION, &m_llLastPos);
    params.audio_stream_cur++; params.audio_stream_cur %= params.audio_stream_total + 1;
    if (params.audio_stream_cur == params.audio_stream_total) params.audio_stream_cur = -1;
    m_bResetPlayer = TRUE;
    sprintf(m_strTxt, "audio stream: %d", params.audio_stream_cur);
    PlayerReset(&params);
}

void CplayerDlg::OnVideoStream()
{
    PLAYER_INIT_PARAMS params;
    player_getparam(m_ffPlayer, PARAM_PLAYER_INIT_PARAMS, &params);
    player_getparam(m_ffPlayer, PARAM_MEDIA_POSITION, &m_llLastPos);
    params.video_stream_cur++; params.video_stream_cur %= params.video_stream_total + 1;
    if (params.video_stream_cur == params.video_stream_total) params.video_stream_cur = -1;
    m_bResetPlayer = TRUE;
    sprintf(m_strTxt, "video stream: %d", params.video_stream_cur);
    PlayerReset(&params);
}

void CplayerDlg::OnVideoMode()
{
    int mode = 0;
    player_getparam(m_ffPlayer, PARAM_VIDEO_MODE, &mode);
    mode++; mode %= VIDEO_MODE_MAX_NUM;
    player_setparam(m_ffPlayer, PARAM_VIDEO_MODE, &mode);

    sprintf(m_strTxt, "video mode: %d", mode);
    PlayerShowText(2000);
}

void CplayerDlg::OnEffectMode()
{
    int mode = 0;
    player_getparam(m_ffPlayer, PARAM_VISUAL_EFFECT, &mode);
    mode++; mode %= VISUAL_EFFECT_MAX_NUM;
    player_setparam(m_ffPlayer, PARAM_VISUAL_EFFECT, &mode);
}

void CplayerDlg::OnVRenderType()
{
    PLAYER_INIT_PARAMS params;
    player_getparam(m_ffPlayer, PARAM_PLAYER_INIT_PARAMS, &params);
    player_getparam(m_ffPlayer, PARAM_MEDIA_POSITION, &m_llLastPos);
    params.vdev_render_type++; params.vdev_render_type %= VDEV_RENDER_TYPE_MAX_NUM;
    m_bResetPlayer = TRUE;
    sprintf(m_strTxt, "vdev type: %d", params.vdev_render_type);
    PlayerReset(&params);
}

void CplayerDlg::OnTakeSnapshot()
{
    player_snapshot(m_ffPlayer, "snapshot.jpg", 0, 0, 1000);
    strcpy(m_strTxt, "take snapshot to snapshot.jpg");
    PlayerShowText(2000);
}

void CplayerDlg::OnStepForward()
{
    player_seek(m_ffPlayer, 0, SEEK_STEP);
    m_bPlayPause = TRUE;
    strcpy(m_strTxt, "step forward");
    PlayerShowText(2000);
}

void CplayerDlg::OnPlaySpeedDec()
{
    int speed;
    player_getparam(m_ffPlayer, PARAM_PLAY_SPEED_VALUE, &speed);
    speed -= 10; if (speed < 10) speed = 10;
    player_setparam(m_ffPlayer, PARAM_PLAY_SPEED_VALUE, &speed);

    sprintf(m_strTxt, "speed value: %d", speed);
    PlayerShowText(2000);
}

void CplayerDlg::OnPlaySpeedInc()
{
    int speed;
    player_getparam(m_ffPlayer, PARAM_PLAY_SPEED_VALUE, &speed);
    speed += 10; if (speed > 200) speed = 200;
    player_setparam(m_ffPlayer, PARAM_PLAY_SPEED_VALUE, &speed);

    sprintf(m_strTxt, "speed value: %d", speed);
    PlayerShowText(2000);
}

void CplayerDlg::OnPlaySpeedType()
{
    int type;
    player_getparam(m_ffPlayer, PARAM_PLAY_SPEED_TYPE, &type);
    type = !type;
    player_setparam(m_ffPlayer, PARAM_PLAY_SPEED_TYPE, &type);

    sprintf(m_strTxt, "speed type: %s", type ? "soundtouch" : "swresample");
    PlayerShowText(2000);
}
