/**
	DFA ´Ê·¨·ÖÎöÆ÷
*/
#pragma once

#include <vector>
#include "LogUtil.h"
#include "Utils.h"
#include <map>

const CString LEXFILENAME = _T("../TestCase/out/lex.txt");

enum LexType
{
	PROGRAM,
	IDENTIFIER,
	TYPE,
	UINTEGER,
	ARRAY,
	RECORD,
	OF,
	PROCEDURE,
	EQU,
	LT,				// <
	LEXEOF,			// EOF
	SPACE,
	INTEGER,
	SEMICOLON,		// ;
	VAR,
	BEGIN,
	END,
	WHILE,
	DO,
	ENDWHILE,
	IF,
	THEN,
	ELSE,
	FI,
	CHARACTER,
	READ,
	WRITE,
	LPARENTHESIS,	// (
	RPARENTHESIS,	// )
	LSQUAREBRACKET,	// [
	RSQUAREBRACKET,	// ]
	ASSIGN,			// :=
	PLUS,
	MINUS,
	MULTIPLY,
	DIVIDE,
	COMMA,			// ,
	UNDERANGE,		// ..
	DOT,			// .
	RETURN,
	EPSILON,
	LEXERR			// Not a lex element
};

struct Token
{
	Token(LexType le) : lex(le){ line = -1; sem = ""; }
	Token(int li, LexType le, CString se) : line(li), lex(le), sem(se)
	{
		/*string s = "";
		stringstream ss;
		ss << li;
		ss >> s;
		s = "line " + s;
		LogUtil::Debug(s + ' ' + se); */
	}
	Token() { line = -1; lex = LexType::LEXERR; sem = ""; }
	int line;
	LexType lex;
	CString sem;
};

class LexicalAnalyzer
{
public:
	LexicalAnalyzer();
	~LexicalAnalyzer();
	CString getNextChar();
	void ungetNextChar();
	CString mOrignalSrcCode;
	void getTokenList();

	vector<Token> mTokenList;
	int mSrcPtr, mCurLine;

	map<LexType, CString> mLex2String;
	map<CString, LexType> mString2Lex;
	map<CString, LexType> mReservedWords;

	void Lex2File();
	bool isDelimiter(CString ch);
	bool isSingleDelimiter(CString ch);
	bool isReservedWord(CString word);
	bool isAlpha(CString ch);
	bool isDigit(CString ch);
	bool isAlNum(CString ch);
};

