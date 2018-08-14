#pragma once

//#include <pthread.h>
extern "C" {
#include "ffplayer.h"
}

#define FOURWIN				4

class CFourInterface
{
public:
	CFourInterface();
	~CFourInterface();

	void CreateChildWin(HWND parent);
	void initSingleWin(HWND parent, char *temp);
	void initFourWin(HWND parent);
	void startPlay(const char* playurl);
	void startPlay1(const char* playurl1);
	void startPlay2(const char* playurl2);
	void startPlay3(const char* playurl3);
	void startPlay4(const char* playurl4);

	int ffplaySingleStop();
	int ffplaySingleWithUrl(const char* playurl, char *temp);
	int ffplayFourStop();
	int ffplayFourWithUrl(const char* playurl);
	void ffplaySingleLButtonDown(int x, int y);
	void ffplaySingleLButtonUp();
	void ffplaySingleMouseMove(int x, int y);
	void ffplaySingleMouseWheel(short delta);

public:
	void *m_hplayer = NULL;  //������
	//void *g_hplayer1 = NULL;
	//void *g_hplayer2 = NULL;
	//void *g_hplayer3 = NULL;
	//void *g_hplayer4 = NULL;
	void *g_hplayer[FOURWIN];
	void **g_fourPlayer;

	HWND hStatic1 = 0;
	HWND hStatic2 = 0;
	HWND hStatic3 = 0;
	HWND hStatic4 = 0;
	HWND **g_fourStatic;

	char m_temp[1024];    //ȫ��ģ��
	char m_url[MAX_PATH];  //���ŵ�ַ
	char *g_url[MAX_PATH];
	char *url1;
	char *url2;
	char *url3;
	char *url4;

	PLAYER_INIT_PARAMS	m_Params;  //���Ų���

	HWND				m_hDialog = 0;     //���Ŵ��ھ��
	HWND				hDialog4 = 0;
};