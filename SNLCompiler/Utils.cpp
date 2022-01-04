#include "pch.h"
#include "Utils.h"

Utils::Utils()
{
}


Utils::~Utils()
{
}


int Utils::Cstr2Int(CString str)
{
	return _ttoi(str);
}

CString Utils::Int2Cstr(int s)
{
	CString str;
	str.Format(_T("%d"), s);
	return str;
}

CString Utils::ReadSrc(CString path)
{
	CString src = _T("");

	CFile infile(path, CFile::modeRead, NULL);
	int len = infile.GetLength();
	char* buf = new char[len + 2];
	buf[len] = buf[len + 1] = '\0';
	infile.Read(buf, len);

	src = buf;
	delete[] buf;
	infile.Close();

	return src;
}

