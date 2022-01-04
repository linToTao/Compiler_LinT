#pragma once

#include <vector>
#include <map>
#include <set>
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

#define STR_EPS CString(_T("<EPSILON>"))
#define EPS_ITEM SyntaxItem(LexType::EPSILON)
#define PRODUCTION_FILENAME _T("../TestCase/SNL_Core/syntax_list.txt")

using namespace std;

class LL1TreeNode
{
public:
	LL1TreeNode()
	{
		//mSemName = _T("");
		mLine = -1;
		mNodeType = NodeType::Terminal;
		mChilds = vector<LL1TreeNode*>();
	}
	vector<LL1TreeNode*> mChilds;
	int mLine;
	NodeType mNodeType;
	Token mToken;
};

struct SyntaxItem
{
	SyntaxItem() 
	{
		nodeType = NodeType::Terminal;
		token = LexType::LEXERR;
	}
	SyntaxItem(NodeType n)
	{
		nodeType = n;
		token = LexType::LEXERR;
	}
	SyntaxItem(LexType t)
	{
		nodeType = NodeType::Terminal;
		token = t;
	}
	SyntaxItem(NodeType n, LexType t): nodeType(n), token(t){}
	bool operator < (const SyntaxItem& item) const
	{
		// 散列函数
		return nodeType * 100000 + token < item.nodeType * 100000 + item.token;
	}
	NodeType nodeType;
	LexType token;	// 当nodeType为Terminal（终极符）时该项有效，否则，该项应为LEXERR
	void SetNodeType(NodeType n) { nodeType = n; token = LexType::LEXERR; }
	void SetLexType(LexType t) { nodeType = Terminal; token = t; }
};

struct Production
{
	SyntaxItem mLeft;
	int no;
	vector<SyntaxItem> mRights;
};

class LL1SyntaxParser
{
public:
	LL1SyntaxParser();
	LL1SyntaxParser(vector<Token> tokens);
	~LL1SyntaxParser();

	void NextToken();
	Token GetCurToken();

	void InitMap();
	CString GetSyntaxTreeStr(CString lftstr, CString append, LL1TreeNode* r);
	void ReleaseTree(LL1TreeNode* r);
	void ReadProuctions();
	vector<CString> SplitString(CString str, CString pattern);		// 读取产生式文件时用于分词

	set<SyntaxItem> GetSyntaxListFirst(vector<SyntaxItem> items);
	void GetFirstSet();
	void GetFollowSet();
	void GetPredictSet();

	void Parse();	// LL1 驱动程序

	int mTokenPtr;
	int mCurLine;
	LL1TreeNode* mSyntaxTree;

	LexicalAnalyzer mLexicalAnalyzer;
	vector<Token> mTokenList;

	vector<ParseLog> mParseLog;
	map<NodeType, CString> mNodeType2Str;
	map<CString, NodeType> mStr2NodeType;

	set<SyntaxItem> mNonTerminals;		// 非终极符
	set<SyntaxItem> mTerminals;			// 终极符
	SyntaxItem mBeginSyntax;

	vector<Production> mProductions;							// 产生式
	map<int, vector<SyntaxItem>> mId2Rights;					// 根据产生式 id 编号获取产生式右部
	map<NodeType, map<LexType, vector<SyntaxItem>>> mLL1Map;	// LL1 状态转换表

	map<SyntaxItem, set<SyntaxItem>> mFirstSet;		// First 集
	map<SyntaxItem, set<SyntaxItem>> mFollowSet;	// Follow 集
	map<int, set<SyntaxItem>> mPredictSet;			// Predict 集
};

