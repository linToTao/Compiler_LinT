#include "pch.h"
#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer()
{
	mOrignalSrcCode = _T("");

	mSrcPtr = 0;
	mCurLine = 1;

	mLex2String[LexType::PROGRAM] = _T("<PROGRAM>");
	mLex2String[LexType::IDENTIFIER] = _T("<IDENTIFIER>");
	mLex2String[LexType::TYPE] = _T("<TYPE>");
	mLex2String[LexType::UINTEGER] = _T("<UINTEGER>");
	mLex2String[LexType::ARRAY] = _T("<ARRAY>");
	mLex2String[LexType::RECORD] = _T("<RECORD>");
	mLex2String[LexType::OF] = _T("<OF>");
	mLex2String[LexType::PROCEDURE] = _T("<PROCEDURE>");
	mLex2String[LexType::EQU] = _T("<EQU>");
	mLex2String[LexType::LT] = _T("<LT>");
	mLex2String[LexType::LEXEOF] = _T("<LEXEOF>");
	mLex2String[LexType::SPACE] = _T("<SPACE>");
	mLex2String[LexType::INTEGER] = _T("<INTEGER>");
	mLex2String[LexType::SEMICOLON] = _T("<SEMICOLON>");
	mLex2String[LexType::VAR] = _T("<VAR>");
	mLex2String[LexType::BEGIN] = _T("<BEGIN>");
	mLex2String[LexType::END] = _T("<END>");
	mLex2String[LexType::WHILE] = _T("<WHILE>");
	mLex2String[LexType::DO] = _T("<DO>");
	mLex2String[LexType::ENDWHILE] = _T("<ENDWHILE>");
	mLex2String[LexType::IF] = _T("<IF>");
	mLex2String[LexType::THEN] = _T("<THEN>");
	mLex2String[LexType::ELSE] = _T("<ELSE>");
	mLex2String[LexType::FI] = _T("<FI>");
	mLex2String[LexType::CHARACTER] = _T("<CHARACTER>");
	mLex2String[LexType::READ] = _T("<READ>");
	mLex2String[LexType::WRITE] = _T("<WRITE>");
	mLex2String[LexType::LPARENTHESIS] = _T("<LPARENTHESIS>");
	mLex2String[LexType::RPARENTHESIS] = _T("<RPARENTHESIS>");
	mLex2String[LexType::LSQUAREBRACKET] = _T("<LSQUAREBRACKET>");
	mLex2String[LexType::RSQUAREBRACKET] = _T("<RSQUAREBRACKET>");
	mLex2String[LexType::ASSIGN] = _T("<ASSIGN>");
	mLex2String[LexType::PLUS] = _T("<PLUS>");
	mLex2String[LexType::MINUS] = _T("<MINUS>");
	mLex2String[LexType::MULTIPLY] = _T("<MULTIPLY>");
	mLex2String[LexType::DIVIDE] = _T("<DIVIDE>");
	mLex2String[LexType::COMMA] = _T("<COMMA>");
	mLex2String[LexType::UNDERANGE] = _T("<UNDERANGE>");
	mLex2String[LexType::DOT] = _T("<DOT>");
	mLex2String[LexType::RETURN] = _T("<RETURN>");
	mLex2String[LexType::EPSILON] = _T("<EPSILON>");
	mLex2String[LexType::LEXERR] = _T("<LEXERR>");

	for (map<LexType, CString>::iterator it = mLex2String.begin(); it != mLex2String.end(); it++)
		mString2Lex[it->second] = it->first;

	mReservedWords[_T("program")] = LexType::PROGRAM;
	mReservedWords[_T("type")] = LexType::TYPE;
	mReservedWords[_T("var")] = LexType::VAR;
	mReservedWords[_T("integer")] = LexType::INTEGER;
	mReservedWords[_T("char")] = LexType::CHARACTER;
	mReservedWords[_T("array")] = LexType::ARRAY;
	mReservedWords[_T("record")] = LexType::RECORD;
	mReservedWords[_T("of")] = LexType::OF;
	mReservedWords[_T("procedure")] = LexType::PROCEDURE;
	mReservedWords[_T("var")] = LexType::VAR;
	mReservedWords[_T("begin")] = LexType::BEGIN;
	mReservedWords[_T("end")] = LexType::END;
	mReservedWords[_T("while")] = LexType::WHILE;
	mReservedWords[_T("do")] = LexType::DO;
	mReservedWords[_T("endwh")] = LexType::ENDWHILE;
	mReservedWords[_T("if")] = LexType::IF;
	mReservedWords[_T("then")] = LexType::THEN;
	mReservedWords[_T("else")] = LexType::ELSE;
	mReservedWords[_T("fi")] = LexType::FI;
	mReservedWords[_T("read")] = LexType::READ;
	mReservedWords[_T("write")] = LexType::WRITE;
	mReservedWords[_T("return")] = LexType::RETURN;
}


LexicalAnalyzer::~LexicalAnalyzer()
{
}


CString LexicalAnalyzer::getNextChar()
{
	CString ret = _T("");
	if (mSrcPtr >= mOrignalSrcCode.GetLength())
		ret = "\0";
	else
		ret = mOrignalSrcCode.GetAt(mSrcPtr++);
	return ret;
}

void LexicalAnalyzer::ungetNextChar()
{
	//if (mSrcPtr <= mOrignalSrcCode.GetLength())
	mSrcPtr--;
}

void LexicalAnalyzer::getTokenList()
{
	CString str = _T("");
	CString cur_char = _T("");
S0:
	cur_char = getNextChar();
	if (cur_char == "")
	{
		Token t(mCurLine, LexType::LEXEOF, str + cur_char);
		mTokenList.push_back(t);
		return;
	}
	else if (isAlpha(cur_char))
	{
		goto INID;
	}
	else if (isDigit(cur_char))
	{
		goto INNUM;
	}
	else if (isSingleDelimiter(cur_char))
	{
		goto INSINGLE;
	}
	else if (cur_char == ":")
	{
		goto INASSIGN;
	}
	else if (cur_char == "{")
	{
		goto INCOMMENT;
	}
	else if (cur_char == ".")
	{
		goto INRANGE;
	}
	/*else if (cur_char == ',')
	{
		goto INCHAR;
	}*/
	else
	{
		goto INERROR;
	}

INID:
	str += cur_char;
	cur_char = getNextChar();
	if (isAlNum(cur_char))
	{
		goto INID;
	}
	else
	{
		Token t(mCurLine, LexType::IDENTIFIER, str);
		if (isReservedWord(str))
			t.lex = mReservedWords[str];
		mTokenList.push_back(t);
		str = "";
		if (cur_char != _T(""))	// 若已经到达文件尾，则无需unget
			ungetNextChar();
		goto S0;
	}

INNUM:
	str += cur_char;
	cur_char = getNextChar();
	if (isDigit(cur_char))
	{
		goto INNUM;
	}
	else if (isDelimiter(cur_char))
	{
		Token t(mCurLine, LexType::UINTEGER, str);
		mTokenList.push_back(t);
		str = "";
		if (cur_char != _T(""))	// 若已经到达文件尾，则无需unget
			ungetNextChar();
		goto S0;
	}
	else
		goto INERROR;

INSINGLE:
	{
		Token t;
		str += cur_char;
		t.line = mCurLine;
		t.sem = str;
		str = "";
		if (cur_char == "\n")
		{
			mCurLine++;
			goto S0;
		}
		else if (cur_char == "+")
		{
			t.lex = LexType::PLUS;
		}
		else if (cur_char == "-")
		{
			t.lex = LexType::MINUS;
		}
		else if (cur_char == "*")
		{
			t.lex = LexType::MULTIPLY;
		}
		else if (cur_char == "/")
		{
			t.lex = LexType::DIVIDE;
		}
		else if (cur_char == "(")
		{
			t.lex = LexType::LPARENTHESIS;
		}
		else if (cur_char == ")")
		{
			t.lex = LexType::RPARENTHESIS;
		}
		else if (cur_char == ";")
		{
			t.lex = LexType::SEMICOLON;
		}
		else if (cur_char == "[")
		{
			t.lex = LexType::LSQUAREBRACKET;
		}
		else if (cur_char == "]")
		{
			t.lex = LexType::RSQUAREBRACKET;
		}
		else if (cur_char == "=")
		{
			t.lex = LexType::EQU;
		}
		else if (cur_char == "<")
		{
			t.lex = LexType::LT;
		}
		else if (cur_char == "\0")
		{
			t.lex = LexType::LEXEOF;
			mTokenList.push_back(t);
			return;                  //出口
		}
		else if (cur_char == " " || cur_char == "\t" || cur_char == "\r")
		{
			//t.lex = LexType::SPACE;
			goto S0;
		}
		else if (cur_char == ",")
		{
			t.lex = LexType::COMMA;
		}

		mTokenList.push_back(t);

		goto S0;
	}


INASSIGN:
	str += cur_char;
	cur_char = getNextChar();
	if (cur_char == "=")
	{
		str += cur_char;
		Token t(mCurLine, LexType::ASSIGN, str);
		mTokenList.push_back(t);
		str = "";
		goto S0;
	}
	else
	{
		goto INERROR;
	}

INCOMMENT:
	//str += cur_char;
	{
		str = "";
		cur_char = getNextChar();
		while (cur_char != "" && cur_char != "}")
		{
			if (cur_char == "\n")
				mCurLine++;
			cur_char = getNextChar();
		}
		goto S0;
	}

INRANGE:
	str += cur_char;
	cur_char = getNextChar();
	if (cur_char == ".")
	{
		str += cur_char;
		Token t(mCurLine, LexType::UNDERANGE, str);
		mTokenList.push_back(t);
		str = "";
		goto S0;
	}
	else
	{
		Token t(mCurLine, LexType::DOT, str);
		mTokenList.push_back(t);
		str = "";
		if (cur_char != _T(""))	// 若已经到达文件尾，则无需unget
			ungetNextChar();
		goto S0;
	}

	/*INCHAR:
		str += cur_char;
		cur_char = getNextChar();
		if (isalnum(cur_char))
		{
			goto DONE;
		}
		else
		{
			goto INERROR;
		}*/
INERROR:
	{
		Token t(mCurLine, LexType::LEXERR, str + cur_char);
		mTokenList.push_back(t);
		CString s = _T("Unexpected symbol \"");
		LogUtil::Error(s + cur_char + "\" in line " + Utils::Int2Cstr(mCurLine));
		str = "";
		/*cur_char = getNextChar();
		while (!isSingleDelimiter(cur_char))
			cur_char = getNextChar();
		ungetNextChar();*/
		goto S0;
	}
	return;
}




void LexicalAnalyzer::Lex2File()
{
	CFile outfile(LEXFILENAME, CFile::modeCreate | CFile::modeReadWrite);
	CString outstr = _T("");
	BYTE UH[] = { 0xff, 0xfe };
	outfile.Write(UH, 2);
	for (int i = 0; i < mTokenList.size(); i++)
	{
		Token t = mTokenList[i];
		if (t.lex == LexType::LEXERR)
		{
			outstr += _T("[ERROR] Unexpected symbol \"");
			outstr += t.sem;
			outstr += _T("\" in line ");
			outstr += Utils::Int2Cstr(t.line);
			outstr += "\r\n";
			continue;
		}
		CString linestr = _T("");
		linestr += _T("LINE ");
		linestr += Utils::Int2Cstr(t.line);
		linestr += _T(": ");
		int k = 10 - linestr.GetLength();
		if (k > 0)
			while (k--)
				linestr += " ";
		linestr += mLex2String[t.lex];
		k = 30 - linestr.GetLength();
		if (k > 0)
			while (k--)
				linestr += " ";
		linestr += t.sem;
		linestr += "\r\n";
		outstr += linestr;
	}
	outfile.Write((LPCTSTR)outstr.GetBuffer(outstr.GetLength()), outstr.GetLength() * sizeof(TCHAR));
	outfile.Close();
	//AfxMessageBox(outstr);
}


bool LexicalAnalyzer::isDelimiter(CString ch)
{

	return isSingleDelimiter(ch) || ch == ":=" || ch == "." || ch == "..";
}

bool LexicalAnalyzer::isSingleDelimiter(CString ch)
{
	return (
		ch == "+" || ch == "-" || ch == "*" || ch == "/"
		|| ch == "(" || ch == ")" || ch == ";" || ch == "["
		|| ch == "]" || ch == "=" || ch == "<" || ch == " "
		|| ch == "\n" || ch == "\t" || ch == "\r" || ch == "\0"
		|| ch == ","
	);
}


bool LexicalAnalyzer::isReservedWord(CString word)
{
	return mReservedWords.find(word) != mReservedWords.end();
}


bool LexicalAnalyzer::isAlpha(CString ch)
{
	if (ch == "\0")
		return false;
	static CString alpha = _T("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	return alpha.Find(ch) != -1;
}


bool LexicalAnalyzer::isDigit(CString ch)
{
	if (ch == "\0")
		return false;
	static CString digit = _T("0123456789");
	return digit.Find(ch) != -1;
}


bool LexicalAnalyzer::isAlNum(CString ch)
{
	return isAlpha(ch) || isDigit(ch);
}


