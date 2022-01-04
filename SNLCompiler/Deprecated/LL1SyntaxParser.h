#pragma once

#include <vector>
#include <map>
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

using namespace std;

struct StackItem
{
	StackItem(NodeType n, Token t): nodeType(n), token(t){}
	NodeType nodeType;
	Token token;	// 当nodeType为Terminal（终极符）时该项有效
};

class LL1SyntaxParser
{
public:
	LL1SyntaxParser();
	LL1SyntaxParser(vector<Token> tokens);
	~LL1SyntaxParser();

	void NextToken();

	void Parse();
	void InitMap();

	map<NodeType, map<LexType, vector<StackItem>>> mLL1Map;
	int mTokenPtr;

	Token GetCurToken();
	vector<ParseLog> mParseLog;
	int mCurLine;
	map<NodeType, CString> mNodeType2Str;

	LexicalAnalyzer mLexicalAnalyzer;
	vector<Token> mTokenList;
	vector<StackItem> Get1();
	vector<StackItem> Get2();
	vector<StackItem> Get3();
	vector<StackItem> Get4();
	vector<StackItem> Get5();
	vector<StackItem> Get6();
	vector<StackItem> Get7();
	vector<StackItem> Get8();
	vector<StackItem> Get9();
	vector<StackItem> Get10();
	vector<StackItem> Get11();
	vector<StackItem> Get12();
	vector<StackItem> Get13();
	vector<StackItem> Get14();
	vector<StackItem> Get15();
	vector<StackItem> Get16();
	vector<StackItem> Get17();
	vector<StackItem> Get18();
	vector<StackItem> Get19();
	vector<StackItem> Get22();
	vector<StackItem> Get23();
	vector<StackItem> Get24();
	vector<StackItem> Get25();
	vector<StackItem> Get26();
	vector<StackItem> Get27();
	vector<StackItem> Get28();
	vector<StackItem> Get29();
	vector<StackItem> Get30();
	vector<StackItem> Get31();
	vector<StackItem> Get32();
	vector<StackItem> Get33();
	vector<StackItem> Get34();
	vector<StackItem> Get35();
	vector<StackItem> Get36();
	vector<StackItem> Get37();
	vector<StackItem> Get38();
	vector<StackItem> Get39();
	vector<StackItem> Get40();
	vector<StackItem> Get41();
	vector<StackItem> Get42();
	vector<StackItem> Get43();
	vector<StackItem> Get44();
	vector<StackItem> Get45();
	vector<StackItem> Get46();
	vector<StackItem> Get47();
	vector<StackItem> Get48();
	vector<StackItem> Get49();
	vector<StackItem> Get50();
	vector<StackItem> Get51();
	vector<StackItem> Get52();
	vector<StackItem> Get53();
	vector<StackItem> Get54();
	vector<StackItem> Get55();
	vector<StackItem> Get56();
	vector<StackItem> Get57();
	vector<StackItem> Get58();
	vector<StackItem> Get59();
	vector<StackItem> Get60();
	vector<StackItem> Get61();
	vector<StackItem> Get62();
	vector<StackItem> Get63();
	vector<StackItem> Get64();
	vector<StackItem> Get65();
	vector<StackItem> Get66();
	vector<StackItem> Get67();
	vector<StackItem> Get68();
	vector<StackItem> Get69();
	vector<StackItem> Get70();
	vector<StackItem> Get71();
	vector<StackItem> Get72();
	vector<StackItem> Get73();
	vector<StackItem> Get74();
	vector<StackItem> Get75();
	vector<StackItem> Get76();
	vector<StackItem> Get77();
	vector<StackItem> Get78();
	vector<StackItem> Get79();
	vector<StackItem> Get80();
	vector<StackItem> Get81();
	vector<StackItem> Get82();
	vector<StackItem> Get83();
	vector<StackItem> Get84();
	vector<StackItem> Get85();
	vector<StackItem> Get86();
	vector<StackItem> Get87();
	vector<StackItem> Get88();
	vector<StackItem> Get89();
	vector<StackItem> Get90();
	vector<StackItem> Get91();
	vector<StackItem> Get92();
	vector<StackItem> Get93();
	vector<StackItem> Get94();
	vector<StackItem> Get95();
	vector<StackItem> Get96();
	vector<StackItem> Get97();
	vector<StackItem> Get98();
	vector<StackItem> Get99();
	vector<StackItem> Get100();
	vector<StackItem> Get101();
	vector<StackItem> Get102();
	vector<StackItem> Get103();
	vector<StackItem> Get104();
};

