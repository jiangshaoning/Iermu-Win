#pragma once

#include <iostream>  
#include <fstream>  
#include <string>  
//#include "irmString.h"

using namespace std;

class CSerializeData
{
public:
	CSerializeData() : next(0) {} // next��0 
	CSerializeData(string phonenum, string password) : phonenum(phonenum), password(password), next(0){}
	CSerializeData(const string& access_token, const string& uid, const string& refresh_token)
		: access_token(access_token), uid(uid), refresh_token(refresh_token), next(0){}

		//��дע���û�������
	void saveLoginInfo(ofstream& stream);
	void loadLoginInfo(ifstream& stream);
	string getLoginUser() { return phonenum; }
	string getLoginPasswd() { return password; }


	    //д�ļ�  
	void save(ofstream& stream);
	void load(ifstream& stream);

	CSerializeData* next;

private:
	string phonenum;
	string password;
	string access_token;
	string uid;
	string refresh_token;

};