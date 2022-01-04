#include "pch.h"
#include "LL1SyntaxParser.h"
#include <stack>


LL1SyntaxParser::LL1SyntaxParser()
{
	mLexicalAnalyzer.getTokenList();
	mLexicalAnalyzer.Lex2File();
	for (int i = 0; i < mLexicalAnalyzer.mTokenList.size(); i++)
	{
		if (mLexicalAnalyzer.mTokenList[i].lex != LexType::LEXERR)
			mTokenList.push_back(mLexicalAnalyzer.mTokenList[i]);
	}
	InitMap();
	mTokenPtr = 0;
	mCurLine = 1;
	mSyntaxTree = NULL;
}

LL1SyntaxParser::LL1SyntaxParser(vector<Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].lex != LexType::LEXERR)
			mTokenList.push_back(tokens[i]);
	}
	InitMap();
	mTokenPtr = 0;
	mCurLine = 1;
	mSyntaxTree = NULL;
}

LL1SyntaxParser::~LL1SyntaxParser()
{
	ReleaseTree(mSyntaxTree);
}

void LL1SyntaxParser::NextToken()
{
	if (mTokenPtr < mTokenList.size())
	{
		mTokenPtr++;
		mCurLine = mTokenList[mTokenPtr].line;
	}
}

Token LL1SyntaxParser::GetCurToken()
{
	if (mTokenPtr == mTokenList.size())
	{
		Token t;
		t.line = mCurLine;
		return t;
	}
	else
		return mTokenList[mTokenPtr];
}

void LL1SyntaxParser::ReadProuctions()
{
	// 读取文法产生式，并转换为内部表示
	CStdioFile infile(PRODUCTION_FILENAME, CFile::modeRead, NULL);

	CString line;
	while (infile.ReadString(line))
	{
		if (line.GetLength() == 0 || line[0] == '/')
			continue;
		CString snum = line.Left(line.Find(_T(" ")));
		line = line.Mid(snum.GetLength(), line.GetLength() - snum.GetLength());
		line.TrimLeft();
		int num = Utils::Cstr2Int(snum);
		vector<CString> sp1 = SplitString(line, _T(" ::= "));
		vector<CString> sp2 = SplitString(sp1[1], _T(" "));
		LogUtil::Debug(snum);

		Production p;
		p.no = num;
		SyntaxItem item(mStr2NodeType[sp1[0]]);
		p.mLeft = item;
		mNonTerminals.insert(item);

		if (mBeginSyntax.nodeType == NodeType::Terminal)
			mBeginSyntax.SetNodeType(item.nodeType);

		for (int i = 0; i < sp2.size(); i++)
		{
			if (sp2[i] == STR_EPS)
			{
				item.SetLexType(LexType::EPSILON);
			}
			else if (sp2[i][0] == '<')
			{
				item.SetLexType(mLexicalAnalyzer.mString2Lex[sp2[i]]);
			}
			else
			{
				if (mStr2NodeType.find(sp2[i]) == mStr2NodeType.end())
				{
					mParseLog.push_back(ParseLog(-1, LogType::LERROR, Utils::FormatCString(_T("Unrecognized symbol %s in syntax file"), sp2[i])));
				}
				item.SetNodeType(mStr2NodeType[sp2[i]]);
			}
			if (item.nodeType == NodeType::Terminal)
				mTerminals.insert(item);
			p.mRights.push_back(item);
		}
		mProductions.push_back(p);
		mId2Rights[p.no] = p.mRights;
	}

	infile.Close();
}

vector<CString> LL1SyntaxParser::SplitString(CString str, CString pattern)
{
	vector<CString> res;

	int pos;

	while ((pos = str.Find(pattern)) != -1)
	{
		CString t = str.Left(pos);
		res.push_back(t);
		str = str.Mid(t.GetLength(), str.GetLength() - t.GetLength());
		str = str.Mid(pattern.GetLength(), str.GetLength() - pattern.GetLength());
		str = str.TrimLeft();
	}
	res.push_back(str);
	return res;
}

void LL1SyntaxParser::GetFirstSet()
{
	for (set<SyntaxItem>::iterator it = mTerminals.begin(); it != mTerminals.end(); it++)
	{
		if (mFirstSet.find(*it) == mFirstSet.end())
			mFirstSet[*it] = set<SyntaxItem>();
		mFirstSet[*it].insert(*it);
	}

	bool stop = false;
	int prev_sz = 0;

	while (!stop)
	{
		stop = true;
		for (int i = 0; i < mProductions.size(); i++)
		{
			SyntaxItem item = mProductions[i].mLeft;
			set<SyntaxItem> tmpst = mFirstSet[item];
			if (mFirstSet.find(item) == mFirstSet.end())
				mFirstSet[item] = set<SyntaxItem>();
			if (mProductions[i].mRights[0].nodeType == NodeType::Terminal)	// EPS or Terminal
			{
				tmpst.insert(mProductions[i].mRights[0]);
			}
			else
			{
				for (int j = 0; j < mProductions[i].mRights.size(); j++)
				{
					if (mFirstSet[mProductions[i].mRights[j]].find(EPS_ITEM) == mFirstSet[mProductions[i].mRights[j]].end())
					{
						// 右部有一个符号的 First 集不含 EPS
						tmpst.erase(EPS_ITEM);
						tmpst.insert(mFirstSet[mProductions[i].mRights[j]].begin(), mFirstSet[mProductions[i].mRights[j]].end());
						
						break;
					}
					else
					{
						// 收集右部的 First，包括 EPS
						tmpst.insert(mFirstSet[mProductions[i].mRights[j]].begin(), mFirstSet[mProductions[i].mRights[j]].end());
					}
				}
				// 若能执行到此，说明该产生式右部都能推导出 EPS
			}
			prev_sz = mFirstSet[item].size();
			mFirstSet[item].insert(tmpst.begin(), tmpst.end());
			if (stop)
				stop = prev_sz == mFirstSet[item].size();
		}
	}
}

void LL1SyntaxParser::GetFollowSet()
{
	if (mFollowSet.find(mBeginSyntax) == mFollowSet.end())
		mFollowSet[mBeginSyntax] = set<SyntaxItem>();
	mFollowSet[mBeginSyntax].insert(SyntaxItem(LexType::LEXEOF));
	for (set<SyntaxItem>::iterator it = mNonTerminals.begin(); it != mNonTerminals.end(); it++)
	{
		SyntaxItem item = *it;
		if (mFollowSet.find(item) == mFollowSet.end())
			mFollowSet[item] = set<SyntaxItem>();
	}

	bool stop = false;
	int prev_sz = 0;

	while (!stop)
	{
		stop = true;
		for (int i = 0; i < mProductions.size(); i++)
		{
			for (int j = 0; j < mProductions[i].mRights.size(); j++)
			{
				// A->xBy
				if (j == mProductions[i].mRights.size() - 1 && mProductions[i].mRights[j].nodeType != NodeType::Terminal)
				{
					// 在产生式右部末尾，A->aB<EPS>
					prev_sz = mFollowSet[mProductions[i].mRights[j]].size();
					mFollowSet[mProductions[i].mRights[j]].insert(mFollowSet[mProductions[i].mLeft].begin(), mFollowSet[mProductions[i].mLeft].end());
					if (stop)
						stop = prev_sz == mFollowSet[mProductions[i].mRights[j]].size();
					continue;
				}
				if (mProductions[i].mRights[j].nodeType == NodeType::Terminal)
					continue;
				set<SyntaxItem> firsty = GetSyntaxListFirst(vector<SyntaxItem>(mProductions[i].mRights.begin() + j + 1, mProductions[i].mRights.end()));
				if (firsty.find(EPS_ITEM) != firsty.end())
				{
					// firsty 包含 EPS，Follow(B) = Follow(B) + firsty - {EPS} + Follow(A)
					firsty.erase(EPS_ITEM);
					prev_sz = mFollowSet[mProductions[i].mRights[j]].size();
					mFollowSet[mProductions[i].mRights[j]].insert(firsty.begin(), firsty.end());
					mFollowSet[mProductions[i].mRights[j]].insert(mFollowSet[mProductions[i].mLeft].begin(), mFollowSet[mProductions[i].mLeft].end());
					if (stop)
						stop = prev_sz == mFollowSet[mProductions[i].mRights[j]].size();
				}
				else
				{
					// firsty 不包含 EPS，Follow(B) = Follow(B) + firsty
					prev_sz = mFollowSet[mProductions[i].mRights[j]].size();
					mFollowSet[mProductions[i].mRights[j]].insert(firsty.begin(), firsty.end());
					if (stop)
						stop = prev_sz == mFollowSet[mProductions[i].mRights[j]].size();
				}
			}
		}
	}
}

void LL1SyntaxParser::GetPredictSet()
{
	for (int i = 0; i < mProductions.size(); i++)
	{
		mPredictSet[mProductions[i].no] = set<SyntaxItem>();
		set<SyntaxItem> first_beta = GetSyntaxListFirst(vector<SyntaxItem>(mProductions[i].mRights.begin(), mProductions[i].mRights.end()));
		if (first_beta.find(EPS_ITEM) == first_beta.end())
		{
			// 对于 A->beta，若 First(beta) 不含 EPS，则 Predict(A->beta) = First(beta)
			mPredictSet[mProductions[i].no].insert(first_beta.begin(), first_beta.end());
		}
		else
		{
			// 否则，Predict(A->beta) = First(beta) - {EPS} + Follow(A)
			first_beta.erase(EPS_ITEM);
			mPredictSet[mProductions[i].no].insert(first_beta.begin(), first_beta.end());
			mPredictSet[mProductions[i].no].insert(mFollowSet[mProductions[i].mLeft].begin(), mFollowSet[mProductions[i].mLeft].end());
		}
	}
}

set<SyntaxItem> LL1SyntaxParser::GetSyntaxListFirst(vector<SyntaxItem> items)
{
	set<SyntaxItem> st;

	for (int i = 0; i < items.size(); i++)
	{
		if (mFirstSet[items[i]].find(EPS_ITEM) == mFirstSet[items[i]].end())
		{
			// Xi 不能推导出 EPS
			st.erase(EPS_ITEM);
			st.insert(mFirstSet[items[i]].begin(), mFirstSet[items[i]].end());
			return st;
		}
		else
		{
			// 收集所有 First，包括 EPS
			st.insert(mFirstSet[items[i]].begin(), mFirstSet[items[i]].end());
		}
	}
	// 若能执行到此，说明任意 Xi 都能推导出 EPS
	return st;
}


void LL1SyntaxParser::Parse()
{
	stack<SyntaxItem> stk;
	SyntaxItem item(NodeType::Program, LexType::LEXERR);

	stk.push(item);
	mSyntaxTree = new LL1TreeNode();

	mSyntaxTree->mNodeType = item.nodeType;
	mSyntaxTree->mLine = mCurLine;

	stack<LL1TreeNode*> s;
	s.push(mSyntaxTree);

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("Program pushed!"))));
	
	while (stk.size() != 0 || GetCurToken().lex != LexType::LEXEOF)
	{
		if (stk.size() != 0 && stk.top().nodeType == NodeType::Terminal && stk.top().token == LexType::EPSILON)
		{
			stk.pop();
			s.pop();
			continue;
		}
		if (stk.size() == 0)
		{
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpeted token %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
			return;
		}
		else if (GetCurToken().lex == LexType::LEXEOF)
		{
			SyntaxItem item = stk.top();
			if (item.nodeType == NodeType::Terminal)
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mLexicalAnalyzer.mLex2String[item.token])));
			else
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mNodeType2Str[item.nodeType])));
			return;
		}

		SyntaxItem item = stk.top();
		if (item.nodeType == NodeType::Terminal)
		{
			if (item.token != GetCurToken().lex)
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("%s and %s couldn't be matched!"), mLexicalAnalyzer.mLex2String[item.token], mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
				return;
			}
			else
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s and %s matched!"), mLexicalAnalyzer.mLex2String[item.token], mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
				NextToken(); mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s popped!"), mLexicalAnalyzer.mLex2String[stk.top().token])));
				stk.pop();
				s.pop();
			}
		}
		else
		{
			if (mLL1Map.find(item.nodeType) == mLL1Map.end() || mLL1Map[item.nodeType].find(GetCurToken().lex) == mLL1Map[item.nodeType].end())
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpeted token %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
				return;
			}
			else
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s popped!"), mNodeType2Str[stk.top().nodeType])));
				stk.pop();
				LL1TreeNode* r = s.top();
				s.pop();
				vector<SyntaxItem> items = mLL1Map[item.nodeType][GetCurToken().lex];
				for (int i = items.size() - 1; i >= 0; i--)
				{
					stk.push(items[i]);
					LL1TreeNode* rp = new LL1TreeNode();
					
					if (items[i].nodeType == NodeType::Terminal)
					{
						rp->mNodeType = items[i].nodeType;
						rp->mToken = Token(items[i].token);
						rp->mLine = mCurLine;
						r->mChilds.push_back(rp);
						mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s pushed!"), mLexicalAnalyzer.mLex2String[items[i].token])));
					}
					else
					{
						rp->mNodeType = items[i].nodeType;
						rp->mLine = mCurLine;
						r->mChilds.push_back(rp);
						mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s pushed!"), mNodeType2Str[items[i].nodeType])));
					}
					s.push(rp);
				}
				//NextToken();
			}
		}
	}
}

void LL1SyntaxParser::InitMap()
{
	mNodeType2Str[NodeType::Program] = _T("Program");
	mNodeType2Str[NodeType::ProgramHead] = _T("ProgramHead");
	mNodeType2Str[NodeType::ProgramName] = _T("ProgramName");
	mNodeType2Str[NodeType::ProgramBody] = _T("ProgramBody");
	mNodeType2Str[NodeType::StmList] = _T("StmList");
	mNodeType2Str[NodeType::Stm] = _T("Stm");
	mNodeType2Str[NodeType::StmMore] = _T("StmMore");
	mNodeType2Str[NodeType::DeclarePart] = _T("DeclarePart");
	mNodeType2Str[NodeType::TypeDec] = _T("TypeDec");
	mNodeType2Str[NodeType::TypeDecList] = _T("TypeDecList");
	mNodeType2Str[NodeType::TypeId] = _T("TypeId");
	mNodeType2Str[NodeType::TypeDef] = _T("TypeDef");
	mNodeType2Str[NodeType::TypeDecMore] = _T("TypeDecMore");
	mNodeType2Str[NodeType::TypeDeclaration] = _T("TypeDeclaration");
	mNodeType2Str[NodeType::BaseType] = _T("BaseType");
	mNodeType2Str[NodeType::StructureType] = _T("StructureType");
	mNodeType2Str[NodeType::ArrayType] = _T("ArrayType");
	mNodeType2Str[NodeType::RecType] = _T("RecType");
	mNodeType2Str[NodeType::FieldDecList] = _T("FieldDecList");
	mNodeType2Str[NodeType::IdList] = _T("IdList");
	mNodeType2Str[NodeType::FieldDecMore] = _T("FieldDecMore");
	mNodeType2Str[NodeType::IdMore] = _T("IdMore");
	mNodeType2Str[NodeType::VarDec] = _T("VarDec");
	mNodeType2Str[NodeType::VarDeclaration] = _T("VarDeclaration");
	mNodeType2Str[NodeType::VarDecList] = _T("VarDecList");
	mNodeType2Str[NodeType::VarIdList] = _T("VarIdList");
	mNodeType2Str[NodeType::VarIdMore] = _T("VarIdMore");
	mNodeType2Str[NodeType::VarDecMore] = _T("VarDecMore");
	mNodeType2Str[NodeType::ProcDec] = _T("ProcDec");
	mNodeType2Str[NodeType::ProcDeclaration] = _T("ProcDeclaration");
	mNodeType2Str[NodeType::ProcDecMore] = _T("ProcDecMore");
	mNodeType2Str[NodeType::ParamList] = _T("ParamList");
	mNodeType2Str[NodeType::ParamDecList] = _T("ParamDecList");
	mNodeType2Str[NodeType::Param] = _T("Param");
	mNodeType2Str[NodeType::ParamMore] = _T("ParamMore");
	mNodeType2Str[NodeType::FormList] = _T("FormList");
	mNodeType2Str[NodeType::FidMore] = _T("FidMore");
	mNodeType2Str[NodeType::ProcDecPart] = _T("ProcDecPart");
	mNodeType2Str[NodeType::ProcBody] = _T("ProcBody");
	mNodeType2Str[NodeType::AssCall] = _T("AssCall");
	mNodeType2Str[NodeType::AssignmentRest] = _T("AssignmentRest");
	mNodeType2Str[NodeType::ConditionalStm] = _T("ConditionalStm");
	mNodeType2Str[NodeType::LoopStm] = _T("LoopStm");
	mNodeType2Str[NodeType::InputStm] = _T("InputStm");
	mNodeType2Str[NodeType::OutputStm] = _T("OutputStm");
	mNodeType2Str[NodeType::ReturnStm] = _T("ReturnStm");
	mNodeType2Str[NodeType::CallStmRest] = _T("CallStmRest");
	mNodeType2Str[NodeType::ActParamList] = _T("ActParamList");
	mNodeType2Str[NodeType::ActParamMore] = _T("ActParamMore");
	mNodeType2Str[NodeType::Exp] = _T("Exp");
	mNodeType2Str[NodeType::OtherTerm] = _T("OtherTerm");
	mNodeType2Str[NodeType::Term] = _T("Term");
	mNodeType2Str[NodeType::OtherFactor] = _T("OtherFactor");
	mNodeType2Str[NodeType::Factor] = _T("Factor");
	mNodeType2Str[NodeType::Variable] = _T("Variable");
	mNodeType2Str[NodeType::VariMore] = _T("VariMore");
	mNodeType2Str[NodeType::FieldVar] = _T("FieldVar");
	mNodeType2Str[NodeType::FieldVarMore] = _T("FieldVarMore");
	mNodeType2Str[NodeType::RelExp] = _T("RelExp");
	mNodeType2Str[NodeType::OtherRelE] = _T("OtherRelE");
	mNodeType2Str[NodeType::CmpOp] = _T("CmpOp");
	mNodeType2Str[NodeType::AddOp] = _T("AddOp");
	mNodeType2Str[NodeType::MultOp] = _T("MultOp");
	mNodeType2Str[NodeType::Terminal] = _T("Terminal");

	for (map<NodeType, CString>::iterator it = mNodeType2Str.begin(); it != mNodeType2Str.end(); it++)
		mStr2NodeType[it->second] = it->first;

	ReadProuctions();

	GetFirstSet();
	GetFollowSet();
	GetPredictSet();

	for (int i = 0; i < mProductions.size(); i++)
	{
		Production p = mProductions[i];
		set<SyntaxItem> predict = mPredictSet[p.no];
		for (set<SyntaxItem>::iterator it = predict.begin(); it != predict.end(); it++)
		{
			vector<SyntaxItem> rights = mId2Rights[p.no];
			mLL1Map[p.mLeft.nodeType][it->token] = rights;
		}
	}
}


CString LL1SyntaxParser::GetSyntaxTreeStr(CString lftstr, CString append, LL1TreeNode* r)
{

	CString b = append;
	if (r->mNodeType == NodeType::Terminal)
		b += mLexicalAnalyzer.mLex2String[r->mToken.lex];
	else
		b += mNodeType2Str[r->mNodeType];
	b += _T("\r\n");;

	vector<LL1TreeNode*> childs = r->mChilds;
	if (childs.size() > 0)
	{
		for (int i = childs.size() - 1; i > 0; i--)
			b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      | "), _T("      |-"), childs[i]));
		b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      "), _T("      |-"), childs[0]));
	}
	return b;
}

void LL1SyntaxParser::ReleaseTree(LL1TreeNode* r)
{
	if (r == NULL)
		return;
	for (int i = 0; i < r->mChilds.size(); i++)
	{
		ReleaseTree(r->mChilds[i]);
	}
	delete r;
	r = NULL;
}
