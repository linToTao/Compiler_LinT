/**
	Log 调试输出工具
*/
#pragma once

#include <string>
using namespace std;

class LogUtil
{
public:
	LogUtil();
	~LogUtil();
	static void Info(CString s);
	static void Error(CString s);
	static void Debug(CString s);
};

