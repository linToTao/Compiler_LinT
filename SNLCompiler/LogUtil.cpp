#include "pch.h"
#include "LogUtil.h"
#include <iostream>


LogUtil::LogUtil()
{
}


LogUtil::~LogUtil()
{
}


void LogUtil::Info(CString s)
{
	CString ostring = _T("[INFO]\t");
	ostring += s;
	ostring += '\n';
	OutputDebugString(ostring);
}


void LogUtil::Error(CString s)
{
	CString ostring = _T("[ERROR]\t");
	ostring += s;
	ostring += '\n';
	OutputDebugString(ostring);
}


void LogUtil::Debug(CString s)
{
	CString ostring = _T("[DEBUG]\t");
	ostring += s;
	ostring += '\n';
	OutputDebugString(ostring);
}
