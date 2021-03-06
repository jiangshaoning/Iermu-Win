// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once


#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <winsock2.h>
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include <event/notifycenter.h>
#include "SMiniblink.h"
#include "resource.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"

using namespace SOUI;


//#define TEST_DEBUG


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS


//补充定义
#define INT64_MIN       (-9223372036854775807i64 - 1)
#define INT64_MAX       9223372036854775807i64


#define IERMU_DB			"iermu.db"
#define DEVICETABLE			"deviceTable"

#define CAMERAFACECOVER		"uires\\facecover"
#define LCAMERAFACECOVER	L"uires\\facecover"
#define LBAIDUAUTHWEBURL	L"https://api.iermu.com/oauth2/authorize?response_type=code&client_id=j095DeA6kUL9oqZrcdmm&redirect_uri=iermuconnect://success&scope=netdisk&display=mobile&connect_type=1/"
#define IERMUCONNECTSUC		"iermuconnect://success/?code="


#include "helper/SMenu.h"
#include "helper/SMenuEx.h"
#define MS_REALWND				WM_USER+100		//父子窗口通信
#define MS_REALWND_URLPLAY		WM_USER+102		//父子窗口通信 URL播放
#define MS_PLAYING_PATHNAME		WM_USER+101		//通知播放文件
#define MS_ADD_FILESED			WM_USER+103		//增加文件后的通知
#define WM_USER_PLAYING         WM_USER + 1     // 开始播放文件
#define WM_USER_POS_CHANGED     WM_USER + 2     // 文件播放位置改变
#define WM_USER_END_REACHED     WM_USER + 3     // 播放完毕
#define WM_USER_GETPLAYURL		WM_USER + 4     // 获取播放地址
