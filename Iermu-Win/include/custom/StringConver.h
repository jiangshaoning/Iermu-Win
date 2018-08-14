#pragma once

#include <iostream>
#include <iomanip>


//string 转换为wstring
void _StrToWstr(wstring& retWStr, const string& str)
{
	if (0 != strlen(str.c_str()))
	{
		const char* pcConvertType = "";

		setlocale(LC_ALL, pcConvertType);
		const char* pc_Source = str.c_str();
		size_t _Dsize = str.size() + 1;

		wchar_t* pus_Dest = new wchar_t[_Dsize];
		wmemset(pus_Dest, 0, _Dsize);
		mbstowcs(pus_Dest, pc_Source, _Dsize);
		retWStr = pus_Dest;
		delete[] pus_Dest;
		setlocale(LC_ALL, "C");
	}
	else
	{
		retWStr = L"";
	}
	return;
}

//wstring 转换为string
void _WstrToStr(string& retStr, const wstring& wstr)
{
	if (0 != wstr.length())
	{
		string curLocale = setlocale(LC_ALL, NULL);
		const char* pcConvertType = "";

		setlocale(LC_ALL, pcConvertType);

		const wchar_t* pus_Source = wstr.c_str();
		int charCount = sizeof(wchar_t) / sizeof(char);
		size_t _Dsize = charCount * wstr.size() + 1;

		char *pc_Dest = new char[_Dsize];
		memset(pc_Dest, 0, _Dsize);
		wcstombs(pc_Dest, pus_Source, _Dsize);
		retStr = pc_Dest;
		delete[] pc_Dest;
		setlocale(LC_ALL, curLocale.c_str());
	}
	else
	{
		retStr = "";
	}
	return;
}