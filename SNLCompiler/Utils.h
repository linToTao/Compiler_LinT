/**
	π§æﬂ¿‡
*/
#pragma once

class Utils
{
public:
	Utils();
	~Utils();
	static int Cstr2Int(CString str);
	static CString Int2Cstr(int s);
	static CString ReadSrc(CString path);
	template <typename ... Types>
	static CString FormatCString(CString format, const Types& ... args)
	{
		CString s;
		s.Format(format, args...);
		return s;
	}
};

