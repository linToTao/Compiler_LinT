#include "pch.h"
#include "RSyntaxParser.h"
#include "LexicalAnalyzer.h"
#define INDENT indentno+=4
#define UNINDENT indentno-=4

RSyntaxParser::RSyntaxParser()
{
	mLexicalAnalyzer.getTokenList();
	mLexicalAnalyzer.Lex2File();
	for (int i = 0; i < mLexicalAnalyzer.mTokenList.size(); i++)
	{
		if (mLexicalAnalyzer.mTokenList[i].lex != LexType::LEXERR)
			mTokenList.push_back(mLexicalAnalyzer.mTokenList[i]);
	}

	mTokenPtr = 0;
	mCurLine = mTokenList.size() == 0 ? 0 : mTokenList[0].line;
	mSyntaxTree = NULL;
	Error = false;
	Errorflag = 1;
	//InitMap();
}

RSyntaxParser::RSyntaxParser(vector<Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].lex != LexType::LEXERR)
			mTokenList.push_back(tokens[i]);
	}

	mTokenPtr = 0;
	mCurLine = mTokenList.size() == 0 ? 0 : mTokenList[0].line;
	mSyntaxTree = NULL;

	//InitMap();
}


RSyntaxParser::~RSyntaxParser()
{
	ReleaseTree(mSyntaxTree);
}


// Token指针往后移一个
void RSyntaxParser::NextToken()
{
	if (mTokenPtr < mTokenList.size())
		mTokenPtr++;
}


Token RSyntaxParser::GetCurToken()
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


// 语法分析入口
TreeNode* RSyntaxParser::Parse()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Now begin parsing. Good luck!")));
	//LogUtil::Info(_T("Now begin parsing. Good luck!"));
	TreeNode* r = Program();
	if (GetCurToken().lex != LexType::LEXEOF)
	{
		mParseLog.push_back(ParseLog(GetCurToken().line, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		////LogUtil::Error(_T("The code ends too early!"));
	}
	mSyntaxTree = r;
	return r;
}


// Program ::= ProgramHead DeclarePart ProgramBody
TreeNode* RSyntaxParser::Program()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Program]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Program in line %d"), mCurLine));
	TreeNode*  prog= new TreeNode();
	prog->nodekind = ProK;
	prog->lineno = mCurLine;
	for (int i = 0; i < 10; i++)
	{
		//prog->name[i] = "";
		prog->table[i] = NULL;
	}


	TreeNode* t = ProgramHead();
	TreeNode* q = DeclarePart();
	TreeNode* s = ProgramBody();

	prog->child[0]=t;
	prog->child[1]=q;
	prog->child[2]=s;

	if (prog != NULL) {
		if (t == NULL)
		{
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramHead]")));
			////LogUtil::Error(Utils::FormatCString(_T("Missing ProgramHead near line %d"), mCurLine));
		}else
			prog->child[0] = t;

		if (q == NULL)
		{
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [DeclarePart]")));
			////LogUtil::Error(Utils::FormatCString(_T("Missing DeclarePart near line %d"), mCurLine));
		}else
			prog->child[1] = q;

		if (s == NULL)
		{
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramBody]")));
			////LogUtil::Error(Utils::FormatCString(_T("Missing ProgramBody near line %d"), mCurLine));
		}else
			prog->child[2] = s;
	}
	
	Match(LexType::DOT);
	//{
	//	/*CString s;
	//	s.Format(_T("Missing <DOT> in line %d"), mCurLine);*/
	//	////LogUtil::Error(Utils::FormatCString(_T("Missing <DOT> near line %d"), mCurLine));
	//	prog->mChilds.push_back(GetMatchedTerminal(t));
	//}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Program] Finished")));
	return prog;
}


// ProgramHead ::= T-<PROGRAM> ProgramName
TreeNode* RSyntaxParser::ProgramHead()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramHead]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramHead in line %d"), mCurLine));
	TreeNode* ph = new TreeNode();
	if (ph != NULL) {
		ph->nodekind = PheadK;
		ph->lineno = mCurLine;
		for (int i = 0; i < 10; i++)
		{
			//ph->name[i] = "";
			ph->table[i] = NULL;
		}
	}
	Match(LexType::PROGRAM);
	Token t = GetCurToken();
	

	if (ph != NULL && t.lex == LexType::IDENTIFIER) {
		ph->lineno = mCurLine;
		ph->name[0] = t.sem;
	}
	
	Match(LexType::IDENTIFIER);


	/*TreeNode* pn = ProgramName();
	ph->mChilds.push_back(pn);
	if (pn == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProgramName]")));
		//LogUtil::Error(Utils::FormatCString(_T("Missing ProgramName near line %d"), mCurLine));
	}


	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramHead] Finished")));*/

	return ph;
}


// ProgramName ::= T-<IDENTIFIER>
//TreeNode* RSyntaxParser::ProgramName()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramName]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramName in line %d"), mCurLine));
//	TreeNode* pn = new TreeNode();
//	pn->mNodeType = NodeType::ProgramName;
//	pn->mLine = mCurLine;
//	
//	Token t = GetCurToken();
//
//	if (Match(LexType::IDENTIFIER))
//	{
//		pn->mChilds.push_back(GetMatchedTerminal(t));
//	}
//	else
//	{
//		////LogUtil::Error(Utils::FormatCString(_T("Missing <IDENTIFIER> near line %d"), mCurLine));
//	}
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramName] Finished")));
//	return pn;
//}

// DeclarePart ::= TypeDec VarDec ProcDec
TreeNode* RSyntaxParser::newDecNode1(NodeKind kind) {
	TreeNode * t = new TreeNode;

	if (t != NULL) {
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind = kind;
		/* 指定新语法树节点t成员:源代码行号lineno为RSyntaxParser::mCurLine */
		t->lineno = mCurLine;
		for (int i = 0; i < 10; i++)
		{
			//t->name[i] = "";
			t->table[i] = NULL;
		}
	}

		
	return t;
}
TreeNode * RSyntaxParser::newDecNode2()
{
	
	TreeNode * t = new TreeNode;
	if (t != NULL) {
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind = DecK;
		t->lineno = mCurLine;

		t->idnum = 0;
		for (int i = 0; i < 10; i++)
		{
			//t->name[i] = "";
			t->table[i] = NULL;
		}
	}
	
	
	return t;

}

TreeNode* RSyntaxParser::DeclarePart()
{
	// 在 DeclarePart 中可以声明自定义类型（数组等）、变量、过程
	// DeclarePart 本身只是一个标记
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [DeclarePart]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing DeclarePart in line %d"), mCurLine));

	TreeNode * typeP = newDecNode1(TypeK);
	TreeNode * pp = typeP;
	if (typeP != NULL)
	{
		typeP->lineno = 0;
		TreeNode * tp1 = TypeDec();
		if (tp1 != NULL)
			typeP->child[0] = tp1;
		else
		{
			free(typeP);
			typeP = NULL;
		}
	}
	TreeNode * varP = newDecNode1(VarK);
	if (varP != NULL)
	{
		varP->lineno = 0;
		TreeNode * tp2 = VarDec();
		if (tp2 != NULL)
			varP->child[0] = tp2;
		else
		{
			free(varP);
			varP = NULL;
		}
	}

	TreeNode * s = ProcDec();



	if (varP == NULL) {
		varP = s;
	}
	if (typeP == NULL) {
		pp = typeP = varP;
	}

	if (typeP != varP) {
		typeP->sibling = varP;
		typeP = varP;
	}
	if (varP != s) {
		varP->sibling = s;
		varP = s;
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[DeclarePart] Finished")));
	return pp;





	/*TreeNode* dp = new TreeNode();
	dp->mNodeType = NodeType::DeclarePart;
	dp->mLine = mCurLine;

	TreeNode* td = TypeDec();
	dp->mChilds.push_back(td);
	if (td == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [TypeDec]")));
		//LogUtil::Error(Utils::FormatCString(_T("Missing TypeDec near line %d"), mCurLine));
	}

	TreeNode* vd = VarDec();
	dp->mChilds.push_back(vd);
	if (vd == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [VarDec]")));
		//LogUtil::Error(Utils::FormatCString(_T("Missing VarDec near line %d"), mCurLine));
	}


	TreeNode* pd = ProcDec();
	dp->mChilds.push_back(pd);
	if (vd == NULL)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, _T("Missing [ProcDec]")));
		//LogUtil::Error(Utils::FormatCString(_T("Missing ProcDec near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[DeclarePart] Finished")));
	return dp;
*/

}

TreeNode* RSyntaxParser::TypeDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDec]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDec in line %d"), mCurLine));
	TreeNode * t = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case TYPE: t = TypeDeclaration(); break;
	case VAR:
	case PROCEDURE:
	case BEGIN: break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDec] Finished")));
	return t;
	
	//mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDec]")));
	////LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDec in line %d"), mCurLine));
	//TreeNode* td = new TreeNode();
	//td->mNodeType = NodeType::TypeDec;
	//td->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::TYPE)	// 向前看 1 个
	//{
	//	td->mChilds.push_back(TypeDeclaration());
	//}
	//else if (GetCurToken().lex == LexType::VAR || GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	td->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDec] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("TypeDec is EPSILON near line %d"), mCurLine));
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	//mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDec] Finished")));
	//return td;

}


// TypeDec ::= 
//			ε Expected {VAR, PROCEDURE, BEGIN}
//			| TypeDeclaration Expected { TYPE }



// TypeDeclaration ::= T-<TYPE> TypeDecList
TreeNode* RSyntaxParser::TypeDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDeclaration]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDeclaration in line %d"), mCurLine));
	

	

	Match(LexType::TYPE);
	
	TreeNode* td = TypeDecList();
	//if (td == NULL)
		////LogUtil::Error(Utils::FormatCString(_T("Missing TypeDecList near line %d"), mCurLine));


	

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDeclaration] Finished")));
	return td;
}


// 
// TypeDecList ::= TypeId T-<EQU> TypeDef T-<SEMICOLON> TypeDecMore
TreeNode* RSyntaxParser::TypeDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDecList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDecList in line %d"), mCurLine));
	TreeNode* tdl = newDecNode2();

	if (tdl != NULL) {

		
		tdl->lineno = mCurLine;
		TypeId(tdl);
		Match(LexType::EQU);
		TypeDef(tdl);
		Match(LexType::SEMICOLON);
		TreeNode* p = TypeDecMore();
		if (p != NULL)
			tdl->sibling = p;
		
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecList] Finished")));
	return tdl;
	/*TreeNode* ti = TypeId();
	tdl->mChilds.push_back(ti);
	if (ti == NULL)
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing TypeId near line %d"), mCurLine));
	}

	Token t = GetCurToken();

	if (Match(LexType::EQU))
	{
		tdl->mChilds.push_back(GetMatchedTerminal(t));
	}

	TreeNode* tdf = TypeDef();
	tdl->mChilds.push_back(tdf);
	if (tdf == NULL)
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing TypeDef near line %d"), mCurLine));
	}

	t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		tdl->mChilds.push_back(GetMatchedTerminal(t));
	}

	TreeNode* tdm = TypeDecMore();
	tdl->mChilds.push_back(tdm);
	if (tdm == NULL)
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing TypeDecMore near line %d"), mCurLine));
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecList] Finished")));
	return tdl;*/
}




// TypeDecMore ::= 	ε { VAR, PROCEDURE, BEGIN }
//					| TypeDecList { IDENTIFIER }
TreeNode* RSyntaxParser::TypeDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDecMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDecMore in line %d"), mCurLine));


	TreeNode * tdm = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case VAR:
	case PROCEDURE:
	case BEGIN:
		break;
	case IDENTIFIER:
		tdm = TypeDecList();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	return tdm;


	//TreeNode* tdm = new TreeNode();
	//tdm->mNodeType = NodeType::TypeDecMore;
	//tdm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::VAR || GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	tdm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("TypeDecMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::IDENTIFIER)
	//{
	//	tdm->mChilds.push_back(TypeDecList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	//mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDecMore] Finished")));
	//return tdm;
}

// TypeId :: = T-<IDENTIFIER>
void RSyntaxParser::TypeId(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeId]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeId in line %d"), mCurLine));
	int tnum = (t->idnum);
	Token token = GetCurToken();
	if ((token.lex == LexType::IDENTIFIER) && (t != NULL))
	{
		t->name[tnum]= token.sem;
		tnum = tnum + 1;
	}
	t->idnum = tnum;
	Match(LexType::IDENTIFIER);
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeId] Finished")));

	/*mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeId]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeId in line %d"), mCurLine));
	TreeNode* ti = new TreeNode();
	ti->mNodeType = NodeType::TypeId;
	ti->mLine = mCurLine;
	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		ti->mChilds.push_back(GetMatchedTerminal(t));
	}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeId] Finished")));
	return ti;*/
}




// TypeDef ::=
//			BaseType { INTEGER, CHARACTER }
//			| StructureType { ARRAY, RECORD }
//			| T-<IDENTIFIER> { IDENTIFIER }
void RSyntaxParser::TypeDef(TreeNode * t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [TypeDef]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing TypeDef in line %d"), mCurLine));


	

	if (t != NULL) {
		Token token = GetCurToken();
		switch (token.lex)
		{
		case INTEGER:
		case CHARACTER:    BaseType(t); break;
		case ARRAY:
		case RECORD:  StructureType(t); break;
		case IDENTIFIER:
			

			t->kind->dec = IdK;
			t->attr->type_name=token.sem;
			


			Match(IDENTIFIER);
			break;
		default:
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
			Errorflag = 0;
			Error = true;
			////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
			NextToken();
			break;
		}
	}



	//TreeNode* tdf = new TreeNode();
	//tdf->mNodeType = NodeType::TypeDef;
	//tdf->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER)	// 向前看 1 个
	//{
	//	tdf->mChilds.push_back(BaseType());
	//}
	//else if (GetCurToken().lex == LexType::ARRAY || GetCurToken().lex == LexType::RECORD)
	//{
	//	tdf->mChilds.push_back(StructureType());
	//}
	//else if (GetCurToken().lex == LexType::IDENTIFIER)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::IDENTIFIER))
	//	{
	//		tdf->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[TypeDef] Finished")));
	
}


// BaseType ::=
//				T-<INTEGER> { INTEGER }
//				| T-<CHAR> { CHARACTER }
void RSyntaxParser::BaseType(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [BaseType]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing BaseType in line %d"), mCurLine));

	Token token = GetCurToken();
	switch (token.lex)
	{
	case INTEGER:  
		Match(INTEGER);
		t->kind->dec = IntegerK;
		break;
	case CHARACTER:    
		Match(CHARACTER);
		t->kind->dec = CharK;
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		
		NextToken();
		break;
	}




	//TreeNode* bt = new TreeNode();
	//bt->mNodeType = NodeType::BaseType;
	//bt->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::INTEGER)	// 向前看 1 个
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::INTEGER))
	//	{
	//		bt->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else if (GetCurToken().lex == LexType::CHARACTER)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::CHARACTER))
	//	{
	//		bt->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[BaseType] Finished")));
	
}


// StructureType ::=
//					ArrayType { ARRAY }
//					| RecType { RECORD }
void RSyntaxParser::StructureType(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StructureType]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing StructureType in line %d"), mCurLine));
	Token token = GetCurToken();

	switch (token.lex)
	{
	case ARRAY:
		ArrayType(t);
		break;
	case RECORD:
		t->kind->dec = RecordK;
		RecType(t);
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}



	//TreeNode* st = new TreeNode();
	//st->mNodeType = NodeType::StructureType;
	//st->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::ARRAY)	// 向前看 1 个
	//{
	//	st->mChilds.push_back(ArrayType());
	//}
	//else if (GetCurToken().lex == LexType::RECORD)
	//{
	//	st->mChilds.push_back(RecType());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StructureType] Finished")));
	
}

// ArrayType ::= T-<ARRAY> T-<[low..top]> T-<OF> BaseType
void RSyntaxParser::ArrayType(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ArrayType]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ArrayType in line %d"), mCurLine));

	Match(ARRAY);
	Match(LexType::LSQUAREBRACKET);
	Token token = GetCurToken();
	if (token.lex == LexType::UINTEGER)
	{
		t->attr->arrayattr->low = _ttoi(token.sem);
		
	}
	Match(LexType::UINTEGER);
	Match(UNDERANGE);
	if (token.lex == UINTEGER)
	{
		t->attr->arrayattr->up = _ttoi(token.sem);
	}
	Match(LexType::UINTEGER);
	Match(LexType::RSQUAREBRACKET);
	Match(OF);
	BaseType(t);
	t->attr->arrayattr->childType = t->kind->dec;
	t->kind->dec = ArrayK;

	//TreeNode* at = new TreeNode();
	//at->mNodeType = NodeType::ArrayType;
	//at->mLine = mCurLine;

	//Token t = GetCurToken();

	//if (Match(LexType::ARRAY))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//t = GetCurToken();

	//if (Match(LexType::LSQUAREBRACKET))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//// TODO: Distinct lower bound and upper bound
	//t = GetCurToken();

	//if (Match(LexType::UINTEGER))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//t = GetCurToken();

	//if (Match(LexType::UNDERANGE))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//t = GetCurToken();

	//if (Match(LexType::UINTEGER))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//t = GetCurToken();

	//if (Match(LexType::RSQUAREBRACKET))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//t = GetCurToken();

	//if (Match(LexType::OF))
	//{
	//	at->mChilds.push_back(GetMatchedTerminal(t));
	//}

	//at->mChilds.push_back(BaseType());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ArrayType] Finished")));
	
}


// RecType ::= T-<RECORD> FieldDecList T-<END>
void RSyntaxParser::RecType(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [RecType]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing RecType in line %d"), mCurLine));
	//Token token = GetCurToken();
	TreeNode * p = NULL;
	Match(RECORD);
	p = FieldDecList();
	if (p != NULL)
		t->child[0] = p;
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));

	}
	Match(END);


	/*TreeNode* rt = new TreeNode();
	rt->mNodeType = NodeType::RecType;
	rt->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::RECORD))
	{
		rt->mChilds.push_back(GetMatchedTerminal(t));
	}

	rt->mChilds.push_back(FieldDecList());

	t = GetCurToken();

	if (Match(LexType::END))
	{
		rt->mChilds.push_back(GetMatchedTerminal(t));
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[RecType] Finished")));
	
}




// FieldDecList ::=
//				BaseType IdList T-<SEMICOLON> FieldDecMore { INTEGER, CHARACTER }
//				| ArrayType IdList T-<SEMICOLON> FieldDecMore { ARRAY }
TreeNode* RSyntaxParser::FieldDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldDecList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FieldDecList in line %d"), mCurLine));
	TreeNode* fd = newDecNode2();
	TreeNode*p = NULL;
	if (fd  != NULL)
	{
		fd->lineno = mCurLine;
		Token token = GetCurToken();
		switch (token.lex) {
		case INTEGER:
		case LexType::CHARACTER:
			BaseType(fd);
			IdList(fd);
			Match(SEMICOLON);
			p = FieldDecMore();
			break;
		case ARRAY:
			ArrayType(fd);
			IdList(fd);
			Match(SEMICOLON);
			p = FieldDecMore();
			break;
		default:
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
			Errorflag = 0;
			Error = true;
			////LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
			NextToken();
			break;

		}
		fd->sibling = p;

	}



	//fd->mNodeType = NodeType::FieldDecList;
	//fd->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER)	// 向前看 1 个
	//{
	//	fd->mChilds.push_back(BaseType());
	//	fd->mChilds.push_back(IdList());

	//	Token t = GetCurToken();

	//	if (Match(LexType::SEMICOLON))
	//	{
	//		fd->mChilds.push_back(GetMatchedTerminal(t));
	//	}

	//	fd->mChilds.push_back(FieldDecMore());
	//}
	//else if (GetCurToken().lex == LexType::ARRAY)
	//{
	//	fd->mChilds.push_back(ArrayType());
	//	fd->mChilds.push_back(IdList());

	//	Token t = GetCurToken();

	//	if (Match(LexType::SEMICOLON))
	//	{
	//		fd->mChilds.push_back(GetMatchedTerminal(t));
	//	}

	//	fd->mChilds.push_back(FieldDecMore());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecList] Finished")));
	return fd;
}

// FieldDecMore ::= 
//					ε { END }
//					| FieldDecList { INTEGER, CHARACTER, ARRAY }
TreeNode* RSyntaxParser::FieldDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldDecMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FieldDecMore in line %d"), mCurLine));
	
	TreeNode* fdm = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case END: break;
	case INTEGER:
	case CHARACTER:
	case ARRAY:
		fdm = FieldDecList();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	
	//TreeNode* fdm = new TreeNode();
	//fdm->mNodeType = NodeType::FieldDecMore;
	//fdm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::END)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	fdm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("FieldDecMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER || GetCurToken().lex == LexType::ARRAY)
	//{
	//	fdm->mChilds.push_back(FieldDecList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldDecMore] Finished")));
	return fdm;
}



// IdList ::= T-<IDENTIFIER> IdMore
void RSyntaxParser::IdList(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [IdList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing IdList in line %d"), mCurLine));
	Token token = GetCurToken();
	if(token.lex == LexType::IDENTIFIER)
	{
		t->name[(t->idnum)]=token.sem;
		Match(IDENTIFIER);
		t->idnum = (t->idnum) + 1;
	}
	IdMore(t);

	/*TreeNode* idl = new TreeNode();
	idl->mNodeType = NodeType::IdList;
	idl->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		idl->mChilds.push_back(GetMatchedTerminal(t));
	}

	idl->mChilds.push_back(IdMore());
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdList] Finished")));
	
}


// IdMore ::= 
//			ε { SEMICOLON }
//			| T-<COMMA> IdList { COMMA }
void RSyntaxParser::IdMore(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [IdMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing IdMore in line %d"), mCurLine));
	Token token = GetCurToken();
	switch (token.lex)
	{
	case SEMICOLON:  break;
	case COMMA:
		Match(COMMA);
		IdList(t);
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}

	//TreeNode* idm = new TreeNode();
	//idm->mNodeType = NodeType::IdMore;
	//idm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::SEMICOLON)	
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	idm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("IdMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::COMMA)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::COMMA))
	//	{
	//		idm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	idm->mChilds.push_back(IdList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[IdMore] Finished")));
	
}


// VarDec ::=
//				ε { PROCEDURE, BEGIN }
//				| VarDeclaration { VAR }
TreeNode* RSyntaxParser::VarDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDec]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarDec in line %d"), mCurLine));
	TreeNode* vd = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case PROCEDURE:
	case BEGIN:     break;
	case VAR:
		vd = VarDeclaration();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	




	//vd->mNodeType = NodeType::VarDec;
	//vd->mLine = mCurLine;


	//if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	vd->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDec] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("VarDec is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::VAR)
	//{
	//	vd->mChilds.push_back(VarDeclaration());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDec] Finished")));
	return vd;
}


// VarDeclaration ::= T-<VAR> VarDecList
TreeNode* RSyntaxParser::VarDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDeclaration]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarDeclaration in line %d"), mCurLine));
	Match(VAR);
	TreeNode * t = VarDecList();
	if (t == NULL)
	{
		//LogUtil::Error(Utils::FormatCString(_T("Missing VarDecList near line %d"), mCurLine));
	}
	return t;
	
	
	/*TreeNode* vd = new TreeNode();
	vd->mNodeType = NodeType::VarDeclaration;
	vd->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::VAR))
	{
		vd->mChilds.push_back(GetMatchedTerminal(t));

		TreeNode* vdl = VarDecList();
		if (vdl == NULL)
			//LogUtil::Error(Utils::FormatCString(_T("Missing VarDecList near line %d"), mCurLine));
		vd->mChilds.push_back(vdl);
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));

		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDeclaration] Finished")));
	return vd;*/
}

// VarDecList ::= TypeDef VarIdList T-<SEMICOLON> VarDecMore
TreeNode* RSyntaxParser::VarDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDecList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarDecList in line %d"), mCurLine));
	
	TreeNode * vdl = newDecNode2();
	TreeNode * p = NULL;
	if (vdl != NULL)
	{
		vdl->lineno = mCurLine;
		TypeDef(vdl);
		VarIdList(vdl);
		Match(SEMICOLON);
		p = VarDecMore();
		vdl->sibling = p;
	}
	return vdl;
	
	/*TreeNode* vdl = new TreeNode();
	vdl->mNodeType = NodeType::VarDecList;
	vdl->mLine = mCurLine;

	vdl->mChilds.push_back(TypeDef());
	vdl->mChilds.push_back(VarIdList());

	Token t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		vdl->mChilds.push_back(GetMatchedTerminal(t));
		vdl->mChilds.push_back(VarDecMore());
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecList] Finished")));
	return vdl;
}


// VarDecMore ::= 
//				ε { PROCEDURE, BEGIN }
//				| VarDecList { INTEGER, CHARACTER, RECORD, ARRAY, IDENTIFIER }
TreeNode* RSyntaxParser::VarDecMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarDecMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarDecMore in line %d"), mCurLine));
	TreeNode * vdm = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case PROCEDURE:
	case BEGIN:
		break;
	case INTEGER:
	case CHARACTER:
	case ARRAY:
	case RECORD:
	case IDENTIFIER:
		vdm = VarDecList();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	
	
	//TreeNode* vdm = new TreeNode();
	//vdm->mNodeType = NodeType::VarDecMore;
	//vdm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::PROCEDURE || GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	vdm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("VarDecMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (
	//	GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER
	//	|| GetCurToken().lex == LexType::RECORD || GetCurToken().lex == LexType::ARRAY
	//	|| GetCurToken().lex == LexType::IDENTIFIER)
	//{
	//	vdm->mChilds.push_back(VarDecList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarDecMore] Finished")));
	return vdm;
}



// VarIdList ::= T-<IDENTIFIER> VarIdMore
void RSyntaxParser::VarIdList(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarIdList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarIdList in line %d"), mCurLine));
	//TreeNode* vil = new TreeNode();
	Token token = GetCurToken();
	if (token.lex == LexType::IDENTIFIER)
	{
		t->name[(t->idnum)]=token.sem;
		Match(IDENTIFIER);
		t->idnum = (t->idnum) + 1;
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
	}
	VarIdMore(t);


	/*vil->mNodeType = NodeType::VarIdList;
	vil->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		vil->mChilds.push_back(GetMatchedTerminal(t));
	}

	vil->mChilds.push_back(VarIdMore());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdList] Finished")));
	
}

// VarIdMore :: = 
//				ε { SEMICOLON }
//				| T-<COMMA> VarIdList { COMMA }
void RSyntaxParser::VarIdMore(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VarIdMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VarIdMore in line %d"), mCurLine));
	
	Token token = GetCurToken();
	switch (token.lex)
	{
	case SEMICOLON:  break;
	case COMMA:
		Match(COMMA);
		VarIdList(t);
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	//TreeNode* vim = new TreeNode();
	//vim->mNodeType = NodeType::VarIdMore;
	//vim->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	vim->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("VarIdMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::COMMA)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::COMMA))
	//	{
	//		vim->mChilds.push_back(GetMatchedTerminal(t));
	//		vim->mChilds.push_back(VarIdList());
	//	}
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VarIdMore] Finished")));
	
}




// ProcDec ::= 
//				ε { BEGIN }
//				| ProcDeclaration { PROCEDURE }
TreeNode* RSyntaxParser::ProcDec()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDec]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDec in line %d"), mCurLine));
	TreeNode* pd = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case BEGIN:   break;
	case PROCEDURE:
		pd = ProcDeclaration();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}


	//pd->mNodeType = NodeType::ProcDec;
	//pd->mLine = mCurLine;


	//if (GetCurToken().lex == LexType::BEGIN)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	pd->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDec] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("ProcDec is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::PROCEDURE)
	//{
	//	pd->mChilds.push_back(ProcDeclaration());
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDec] Finished")));
	return pd;
}

/* 书上算法26：少了 ProcDecMore */
// ProcDeclaration ::= 
//						T-<PROCEDURE>
//						ProcName(ParamList)
//						ProcDecPart
//						ProcBody
//						          ProcDecMore
TreeNode* RSyntaxParser::newProcNode() {
	TreeNode * t = new TreeNode;
	

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t != NULL)
	{
		
		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind = ProcDecK;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = mCurLine;
		t->idnum = 0;
		for (int i = 0; i < 10; i++)
		{
			t->name[i]= "";
			t->table[i] = NULL;
		}
	}
	/* 函数返回语句类型语法树节点指针t */
	return t;

}


TreeNode* RSyntaxParser::ProcDeclaration()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDeclaration]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDeclaration in line %d"), mCurLine));
	TreeNode* pd = newProcNode();
	Match(PROCEDURE);
	if (pd != NULL)
	{
		pd->lineno = mCurLine;
		Token token = GetCurToken();
		if (token.lex == IDENTIFIER)
		{
			
			pd->name[0]=token.sem;
			(pd->idnum)++;
			Match(IDENTIFIER);
		}
		Match(LPARENTHESIS);
		ParamList(pd);
		Match(RPARENTHESIS);
		Match(SEMICOLON);
		pd->child[1] = ProcDecPart();
		pd->child[2] = ProcBody();
		pd->sibling = ProcDec();
	}





	/*pd->mNodeType = NodeType::ProcDeclaration;
	pd->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::PROCEDURE))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}
	
	if (GetCurToken().lex == LexType::IDENTIFIER)
	{
		t = GetCurToken();

		if (Match(LexType::IDENTIFIER))
		{
			pd->mChilds.push_back(GetMatchedTerminal(t));
		}
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}

	pd->mChilds.push_back(ParamList());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::SEMICOLON))
	{
		pd->mChilds.push_back(GetMatchedTerminal(t));
	}
	pd->mChilds.push_back(ProcDecPart());
	pd->mChilds.push_back(ProcBody());
	pd->mChilds.push_back(ProcDecMore());*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDeclaration] Finished")));
	return pd;
}


/* 书上算法少了此条产生式 */
// ProcDecMore ::=
//					ε { BEGIN }
//					| ProcDeclaration {PROCEDURE}
//TreeNode* RSyntaxParser::ProcDecMore()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDecMore]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDecMore in line %d"), mCurLine));
//	TreeNode* pdm = new TreeNode();
//	pdm->mNodeType = NodeType::ProcDecMore;
//	pdm->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::BEGIN)
//	{
//		TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
//		pdm->mChilds.push_back(n);
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecMore] is EPSILON")));
//		//LogUtil::Info(Utils::FormatCString(_T("ProcDecMore is EPSILON near line %d"), mCurLine));
//	}
//	else if (GetCurToken().lex == LexType::PROCEDURE)
//	{
//		pdm->mChilds.push_back(ProcDeclaration());
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecMore] Finished")));
//	return pdm;
//}

// ParamList :: = 
//				ε { RPAREN }
//				| ParamDecList { INTEGER, CHAR, ARRAY, RECORD, ID, VAR }
void RSyntaxParser::ParamList(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ParamList in line %d"), mCurLine));

	TreeNode * p = NULL;
	Token token = GetCurToken();
	switch (token.lex)
	{
	case LexType::RPARENTHESIS:  break;
	case INTEGER:
	case CHARACTER:
	case ARRAY:
	case RECORD:
	case IDENTIFIER:
	case VAR:
		p = ParamDecList();
		t->child[0] = p;
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	//TreeNode* pl = new TreeNode();
	//pl->mNodeType = NodeType::ParamList;
	//pl->mLine = mCurLine;


	//if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	pl->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamList] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("ParamList is EPSILON near line %d"), mCurLine));
	//}
	//else if (
	//	GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER
	//	|| GetCurToken().lex == LexType::ARRAY || GetCurToken().lex == LexType::RECORD
	//	|| GetCurToken().lex == LexType::IDENTIFIER || GetCurToken().lex == LexType::VAR
	//	)
	//{
	//	pl->mChilds.push_back(ParamDecList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamList] Finished")));
	
}


// ParamDecList ::= Param ParamMore
TreeNode* RSyntaxParser::ParamDecList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamDecList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ParamDecList in line %d"), mCurLine));
	TreeNode* t = Param();
	TreeNode * p = ParamMore();
	if (p != NULL)
	{
		t->sibling = p;
	}
	
	/*TreeNode* vd = new TreeNode();
	vd->mNodeType = NodeType::ParamDecList;
	vd->mLine = mCurLine;
	vd->mChilds.push_back(Param());
	vd->mChilds.push_back(ParamMore());
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamDecList] Finished")));
	return t;
}


// ParamMore ::= 
//				ε { RPAREN }
//				| T-<SEMICOLON> ParamDecList { SEMICOLON }
TreeNode* RSyntaxParser::ParamMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ParamMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ParamMore in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * pm = NULL;
	switch (token.lex)
	{
	case RPARENTHESIS:   break;
	case SEMICOLON:
		Match(SEMICOLON);
		pm = ParamDecList();
		//if (pm == NULL)
			//LogUtil::Error(Utils::FormatCString(_T("Missing ParamDecList near line %d"), mCurLine));
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	//TreeNode* pm = new TreeNode();
	//pm->mNodeType = NodeType::ParamMore;
	//pm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	pm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("ParamMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::SEMICOLON)	// 向前看 1 个
	//{
	//	Token t = GetCurToken();
	//	if (Match(LexType::SEMICOLON))
	//	{
	//		pm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	pm->mChilds.push_back(ParamDecList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ParamMore] Finished")));
	return pm;
}

// Param ::= 
//			TypeDef FormList { INTEGER, CHARARCTER, RECORD, ARRAY, IDENTIFIER }
//			| T-<VAR> TypeDef FormList { VAR }
TreeNode* RSyntaxParser::Param()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Param]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Param in line %d"), mCurLine));
	TreeNode* p = newDecNode2();
	Token token = GetCurToken();
	if (p != NULL) {
		p->lineno = mCurLine;

		switch (token.lex)
		{
		case INTEGER:
		case CHARACTER:
		case ARRAY:
		case RECORD:
		case IDENTIFIER:
			p->attr->proattr->paramt=Paramt::valparamtype;
			
			TypeDef(p);
			FormList(p);
			break;
		case VAR:
			Match(VAR);
			p->attr->proattr->paramt = varparamtype;
			
			TypeDef(p);
			FormList(p);
			break;
		default:
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
			Errorflag = 0;
			Error = true;
			//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
			NextToken();
			break;
		}
	}
	
	
	
	//p->mNodeType = NodeType::Param;
	//p->mLine = mCurLine;

	//if (
	//	GetCurToken().lex == LexType::INTEGER || GetCurToken().lex == LexType::CHARACTER
	//	|| GetCurToken().lex == LexType::RECORD || GetCurToken().lex == LexType::ARRAY
	//	|| GetCurToken().lex == LexType::IDENTIFIER)	// 向前看 1 个
	//{
	//	p->mChilds.push_back(TypeDef());
	//	p->mChilds.push_back(FormList());
	//}
	//else if (GetCurToken().lex == LexType::VAR)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::VAR))
	//	{
	//		p->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	p->mChilds.push_back(TypeDef());
	//	p->mChilds.push_back(FormList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Param] Finished")));
	return p;
}


// FormList ::= T-<IDENTIFIER> FidMore
void RSyntaxParser::FormList(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FormList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FormList in line %d"), mCurLine));
	Token token = GetCurToken();
	if (token.lex == IDENTIFIER)
	{
		t->name[(t->idnum)]=token.sem;
		t->idnum = (t->idnum) + 1;
		Match(IDENTIFIER);
	}
	FidMore(t);



	//TreeNode* fl = new TreeNode();
	//fl->mNodeType = NodeType::FormList;
	//fl->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::IDENTIFIER)	// 向前看 1 个
	//{
	//	Token t = GetCurToken();
	//	if (Match(LexType::IDENTIFIER))
	//	{
	//		fl->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//fl->mChilds.push_back(FidMore());

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FormList] Finished")));
	//return fl;
}


// FidMore ::= 
//				ε { SEMICOLON, RPARENTHESIS }
//				| T-<COMMA> FormList { COMMA }
void RSyntaxParser::FidMore(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FidMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FidMore in line %d"), mCurLine));
	Token token = GetCurToken();
	switch (token.lex)
	{
	case SEMICOLON:
	case RPARENTHESIS:   break;
	case COMMA:
		Match(COMMA);
		FormList(t);
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	//TreeNode* fm = new TreeNode();
	//fm->mNodeType = NodeType::FidMore;
	//fm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	fm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FidMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("FidMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	//{
	//	Token t = GetCurToken();
	//	if (Match(LexType::COMMA))
	//	{
	//		fm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	fm->mChilds.push_back(FormList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FidMore] Finished")));
	//return fm;
}




// ProcDecPart ::= DeclarePart
TreeNode* RSyntaxParser::ProcDecPart()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcDecPart]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProcDecPart in line %d"), mCurLine));
	
	TreeNode * pdp = DeclarePart();
	/*TreeNode* pdp = new TreeNode();
	pdp->mNodeType = NodeType::ProcDecPart;
	pdp->mLine = mCurLine;
	
	pdp->mChilds.push_back(DeclarePart());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcDecPart] Finished")));
	return pdp;
}

// ProcBody ::= ProgramBody
TreeNode* RSyntaxParser::ProcBody()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProcBody]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProcBody in line %d"), mCurLine));
	TreeNode* pb = ProgramBody();
	//if(pb==NULL)
		//LogUtil::Error(Utils::FormatCString(_T("Missing ProcBody near line %d"), mCurLine));

	/*pb->mNodeType = NodeType::ProcBody;
	pb->mLine = mCurLine;

	pb->mChilds.push_back(ProgramBody());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProcBody] Finished")));
	return pb;
}
TreeNode* RSyntaxParser::newStmlNode() {
	
	TreeNode * t =new TreeNode;
	

	
	if (t != NULL)
	{
		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind = StmLK;
		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = mCurLine;
		t->idnum = 0;
		for (int i = 0; i < 10; i++)
		{
			t->name[i]= "";
			t->table[i] = NULL;
		}
	}
	/*函数返回语句类型语法树节点指针t*/
	return t;
}

// ProgramBody ::= T-<BEGIN> StmList T-<END>
TreeNode* RSyntaxParser::ProgramBody()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ProgramBody]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ProgramBody in line %d"), mCurLine));
	TreeNode* pb = newStmlNode();
	Match(BEGIN);
	if (pb != NULL)
	{
		pb->lineno = mCurLine;
		pb->child[0] = StmList();
	}
	Match(END);
	

	/*pb->mNodeType = NodeType::ProgramBody;
	pb->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::BEGIN))
	{
		pb->mChilds.push_back(GetMatchedTerminal(t));
	}

	pb->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::END))
	{
		pb->mChilds.push_back(GetMatchedTerminal(t));
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ProgramBody] Finished")));
	return pb;
}



// StmList ::= Stm StmMore
TreeNode* RSyntaxParser::StmList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StmList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing StmList in line %d"), mCurLine));
	TreeNode * t = Stm();
	TreeNode * p = StmMore();
	
		if (t != NULL&&p != NULL)
			t->sibling = p;
	

	/*TreeNode* sl = new TreeNode();
	sl->mNodeType = NodeType::StmList;
	sl->mLine = mCurLine;

	sl->mChilds.push_back(Stm());
	sl->mChilds.push_back(StmMore());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmList] Finished")));
	return t;
}


/* 书上算法37：StmMore ::= ε 的 Preict 集少了 ELSE、FI */
// StmMore ::=
//				ε { ELSE, FI, END, ENDWHILE }
//				| T-<SEMICOLON> StmList { SEMICOLON }
TreeNode* RSyntaxParser::StmMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [StmMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing StmMore in line %d"), mCurLine));
	Token token = GetCurToken();

	TreeNode * t = NULL;
	switch (token.lex)
	{
	case ELSE:
	case FI:
	case END:
	case ENDWHILE: break;
	case SEMICOLON:
		Match(SEMICOLON);
		t = StmList();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	//TreeNode* sm = new TreeNode();
	//sm->mNodeType = NodeType::StmMore;
	//sm->mLine = mCurLine;

	//if (
	//	GetCurToken().lex == LexType::END || GetCurToken().lex == LexType::ENDWHILE
	//	|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	sm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("StmMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::SEMICOLON)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::SEMICOLON))
	//	{
	//		sm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	sm->mChilds.push_back(StmList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[StmMore] Finished")));
	return t;
}

// Stm ::=
//			ConditionalStm { IF }
//			| LoopStm { WHILE }
//			| InputStm { READ }
//			| OutputStm { WRITE }
//			| ReturnStm { RETURN }
//			| T-<IDENTIFIER> AssCall { IDENTIFIER }
TreeNode* RSyntaxParser::Stm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Stm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Stm in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * t = NULL;
	switch (token.lex)
	{
	case IF:
		t = ConditionalStm();
		break;
	case WHILE:
		t = LoopStm();
		break;
	case READ:
		t = InputStm();
		break;
	case WRITE:
		t = OutputStm();
		break;
	case RETURN:
		t = ReturnStm();
		break;
	case IDENTIFIER:
		temp_name = token.sem;
		Match(IDENTIFIER);
		t = AssCall();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	

	//TreeNode* s = new TreeNode();
	//s->mNodeType = NodeType::Stm;
	//s->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::IF)	// 向前看 1 个
	//{
	//	s->mChilds.push_back(ConditionalStm());
	//}
	//else if (GetCurToken().lex == LexType::WHILE)
	//{
	//	s->mChilds.push_back(LoopStm());
	//}
	//else if (GetCurToken().lex == LexType::READ)
	//{
	//	s->mChilds.push_back(InputStm());
	//}
	//else if (GetCurToken().lex == LexType::WRITE)
	//{
	//	s->mChilds.push_back(OutputStm());
	//}
	//else if (GetCurToken().lex == LexType::RETURN)
	//{
	//	s->mChilds.push_back(ReturnStm());
	//}
	//else if (GetCurToken().lex == LexType::IDENTIFIER)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::IDENTIFIER))
	//	{
	//		s->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	s->mChilds.push_back(AssCall());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Stm] Finished")));
	return t;
}

/* 书上产生式67：AssCall ::= AssignmentRest 的 Predict 集少了 LSQUAREBRACKET、DOT */
/* 算法39：AssCall ::= CallStmRest Predict 集应为 { LPARENTHESIS } 而不是 { RPARENTHESIS } */
// AssCall ::= 
//				AssignmentRest { ASSIGN, LSQUAREBRACKET, DOT }
//				| CallStmRest { LPARENTHESIS }
TreeNode* RSyntaxParser::AssCall()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AssCall]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing AssCall in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * t = NULL;
	switch (token.lex)
	{
	case ASSIGN:
	case LSQUAREBRACKET:
	case DOT:
		t = AssignmentRest();
		break;
	case LPARENTHESIS:
		t = CallStmRest();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	return t;
	
	//TreeNode* ac = new TreeNode();
	//ac->mNodeType = NodeType::AssCall;
	//ac->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::LSQUAREBRACKET || GetCurToken().lex == LexType::DOT)	// 向前看 1 个
	//{
	//	ac->mChilds.push_back(AssignmentRest());
	//}
	//else if (GetCurToken().lex == LexType::LPARENTHESIS)
	//{
	//	ac->mChilds.push_back(CallStmRest());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AssCall] Finished")));
	return t;
}

TreeNode* RSyntaxParser::newStmtNode(StmtKind  kind) {
	TreeNode * t = new TreeNode;
	if (t != NULL)
	 {
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind =StmtK;
		/* 指定新语法树节点t成员:语句类型kind.stmt为函数给定参数kind */
		t->kind->stmt = kind;
		
		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = mCurLine;
		t->idnum = 0;
		for (int i = 0; i < 10; i++)
		{
			t->name[i]= "";
			t->table[i] = NULL;
		}
	}
	/* 函数返回语句类型语法树节点指针t */
	return t;
}
TreeNode* RSyntaxParser::newExpNode(ExpKind  kind) {
	TreeNode * t = new TreeNode;
	if (t != NULL)
	{
		for (int i = 0; i < 3; i++) t->child[i] = NULL;
		t->nodekind = ExpK;
		/* 指定新语法树节点t成员:语句类型kind.stmt为函数给定参数kind */
		t->kind->exp = kind;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = mCurLine;
		t->attr->expattr->varkind = IdV;
		t->attr->expattr->type = Void;
		for (int i = 0; i < 10; i++)
		{
			//t->name[i] = "";
			t->table[i] = NULL;
		}
	}
	/* 函数返回语句类型语法树节点指针t */
	return t;
}

/* 书上算法40：产生式少了 VariMore */
// AssignmentRest ::= VariMore T-<ASSIGN> Exp
TreeNode* RSyntaxParser::AssignmentRest()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AssignmentRest]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing AssignmentRest in line %d"), mCurLine));
	
	TreeNode * t = newStmtNode(AssignK);
	/* 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名，*
	/* 第二个儿子结点记录赋值语句的右侧表达式*/
	if (t != NULL)
	{
		t->lineno = mCurLine;
		/*处理第一个儿子结点，为变量表达式类型节点*/
		TreeNode * child1 = newExpNode(VariK);
		if (child1 != NULL)
		{
			child1->lineno = mCurLine;
			child1->name[0]=temp_name;
			(child1->idnum)++;
			VariMore(child1);
			t->child[0] = child1;
		}
		/*赋值号匹配*/
		Match(ASSIGN);

		/*处理第二个儿子节点*/
		t->child[1] = Exp();
	}
	
	/*TreeNode* ar = new TreeNode();
	ar->mNodeType = NodeType::AssignmentRest;
	ar->mLine = mCurLine;

	ar->mChilds.push_back(VariMore());

	Token t = GetCurToken();

	if (Match(LexType::ASSIGN))
	{
		ar->mChilds.push_back(GetMatchedTerminal(t));
	}
	ar->mChilds.push_back(Exp());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AssignmentRest] Finished")));
	return t;
}


/* 书上算法41：Exp 应改为 RelExp，StmL 应改为 StmList */
// ConditionalStm :: = T-<IF> RelExp T-<THEN> StmList T-<ELSE> StmList T-<FI>
TreeNode* RSyntaxParser::ConditionalStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ConditionalStm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ConditionalStm in line %d"), mCurLine));
	
	TreeNode * t = newStmtNode(IfK);
	Match(IF);
	if (t != NULL)
	{
		t->lineno = mCurLine;
		t->child[0] = Exp();
	}
	Match(THEN);
	if (t != NULL)  
		t->child[1] = StmList();
	Token token = GetCurToken();
	if (token.lex == ELSE)
	{
		Match(ELSE);
		if (t != NULL)
			t->child[2] = StmList();
	}
	Match(FI);
	
	
	
	/*TreeNode* cs = new TreeNode();
	cs->mNodeType = NodeType::ConditionalStm;
	cs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IF))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(RelExp());

	t = GetCurToken();

	if (Match(LexType::THEN))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::ELSE))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::FI))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ConditionalStm] Finished")));
	return t;
}


/* 书上算法42：Exp 应改为 RelExp */
// LoopStm :: = T-<WHILE> RelExp T-<DO> StmList T-<ENDWHILE>
TreeNode* RSyntaxParser::LoopStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [LoopStm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing LoopStm in line %d"), mCurLine));
	
	
	TreeNode * t = newStmtNode(WhileK);
	Match(WHILE);
	if (t != NULL)
	{
		t->lineno = mCurLine;
		t->child[0] = Exp();
		Match(DO);
		t->child[1] = StmList();
		Match(ENDWHILE);
	}
	
	
	/*TreeNode* ls = new TreeNode();
	ls->mNodeType = NodeType::LoopStm;
	ls->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::WHILE))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}

	ls->mChilds.push_back(RelExp());

	t = GetCurToken();

	if (Match(LexType::DO))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}

	ls->mChilds.push_back(StmList());

	t = GetCurToken();

	if (Match(LexType::ENDWHILE))
	{
		ls->mChilds.push_back(GetMatchedTerminal(t));
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[LoopStm] Finished")));
	return t;
}

// InputStm ::= T-<READ> T-<LPARENTHESIS> T-<IDENTIFIER> T-<RPARENTHESIS>
TreeNode* RSyntaxParser::InputStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [InputStm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing InputStm in line %d"), mCurLine));
	
	
	TreeNode * t = newStmtNode(ReadK);
	Match(READ);
	Match(LPARENTHESIS);
	Token token = GetCurToken();
	if ((t != NULL) && (token.lex == IDENTIFIER))
	{
		t->lineno =mCurLine;
		t->name[0]=token.sem;
		(t->idnum)++;
	}
	Match(IDENTIFIER);
	Match(RPARENTHESIS);
	
	
	/*TreeNode* is = new TreeNode();
	is->mNodeType = NodeType::InputStm;
	is->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::READ))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		is->mChilds.push_back(GetMatchedTerminal(t));
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[InputStm] Finished")));
	return t;
}


// OutputStm ::= T-<WRITE> T-<LPARENTHESIS> Exp T-<RPARENTHESIS>
TreeNode* RSyntaxParser::OutputStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OutputStm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing OutputStm in line %d"), mCurLine));
	//Token token = GetCurToken();
	TreeNode * t = newStmtNode(WriteK);
	Match(WRITE);
	Match(LPARENTHESIS);
	if (t != NULL)
	{
		t->lineno = mCurLine;
		t->child[0] = Exp();
	}
	Match(RPARENTHESIS);
	
	
	
	/*TreeNode* os = new TreeNode();
	os->mNodeType = NodeType::OutputStm;
	os->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::WRITE))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}

	t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}

	os->mChilds.push_back(Exp());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		os->mChilds.push_back(GetMatchedTerminal(t));
	}
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OutputStm] Finished")));
	return t;
}


// ReturnStm ::= T-<RETURN>
TreeNode* RSyntaxParser::ReturnStm()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ReturnStm]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ReturnStm in line %d"), mCurLine));
	
	TreeNode * t = newStmtNode(ReturnK);
	Match(RETURN);
	if (t != NULL)
		t->lineno = mCurLine;
	
	/*TreeNode* rs = new TreeNode();
	rs->mNodeType = NodeType::ReturnStm;
	rs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::RETURN))
	{
		rs->mChilds.push_back(GetMatchedTerminal(t));
	}
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ReturnStm] Finished")));
	return t;
}


// CallStmRest ::= T-<LPARENTHESIS> ActParamList T-<RPARENTHESIS>
TreeNode* RSyntaxParser::CallStmRest()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [CallStmRest]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing CallStmRest in line %d"), mCurLine));
	
	TreeNode * t = newStmtNode(CallK);
	Match(LPARENTHESIS);
	/*函数调用时，其子节点指向实参*/
	if (t != NULL)
	{
		t->lineno = mCurLine;
		/*函数名的结点也用表达式类型结点*/
		TreeNode * child0 = newExpNode(VariK);
		if (child0 != NULL)
		{
			child0->lineno = mCurLine;
			child0->name[0]= temp_name;
			(child0->idnum)++;
			t->child[0] = child0;
		}
		t->child[1] = ActParamList();
	}
	Match(RPARENTHESIS);
	
	
	/*TreeNode* cs = new TreeNode();
	cs->mNodeType = NodeType::CallStmRest;
	cs->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::LPARENTHESIS))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}

	cs->mChilds.push_back(ActParamList());

	t = GetCurToken();

	if (Match(LexType::RPARENTHESIS))
	{
		cs->mChilds.push_back(GetMatchedTerminal(t));
	}*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[CallStmRest] Finished")));
	return t;
}


// ActParamList ::=
//				ε { RPARENTHESIS }
//				| Exp ActParamMore { IDENTIFIER, UINTEGER }
TreeNode* RSyntaxParser::ActParamList()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ActParamList]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamList in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * t = NULL;
	switch (token.lex)
	{
	case RPARENTHESIS:        break;
	case IDENTIFIER:
	case UINTEGER:
		t = Exp();
		if (t != NULL)
			t->sibling = ActParamMore();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	
	//TreeNode* apl = new TreeNode();
	//apl->mNodeType = NodeType::ActParamList;
	//apl->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	apl->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamList] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("ActParamList is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::IDENTIFIER || GetCurToken().lex == LexType::UINTEGER)
	//{
	//	apl->mChilds.push_back(Exp());
	//	apl->mChilds.push_back(ActParamMore());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamList] Finished")));
	return t;
}


// ActParamMore ::=
//				ε { RPARENTHESIS }
//				| T-<COMMA> ActParamList { COMMA }
TreeNode* RSyntaxParser::ActParamMore()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [ActParamMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing ActParamMore in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * t = NULL;
	switch (token.lex)
	{
	case RPARENTHESIS:   break;
	case COMMA:
		Match(COMMA);
		t = ActParamList();
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	
	
	//TreeNode* apm = new TreeNode();
	//apm->mNodeType = NodeType::ActParamMore;
	//apm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::RPARENTHESIS)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	apm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("ActParamMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::COMMA)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::COMMA))
	//	{
	//		apm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	apm->mChilds.push_back(ActParamList());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[ActParamMore] Finished")));
	return t;
}

/* 书上算法部分貌似少了该条产生式 */
// RelExp ::= Exp OtherRelE
//TreeNode* RSyntaxParser::RelExp()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [RelExp]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing RelExp in line %d"), mCurLine));
//	TreeNode* re = new TreeNode();
//	re->mNodeType = NodeType::RelExp;
//	re->mLine = mCurLine;
//
//	re->mChilds.push_back(Exp());
//	re->mChilds.push_back(OtherRelE());
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[RelExp] Finished")));
//	return re;
//}

/* 书上算法部分貌似少了该条产生式 */
// OtherRelE ::= CmpOp Exp
//TreeNode* RSyntaxParser::OtherRelE()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherRelE]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing OtherRelE in line %d"), mCurLine));
//	TreeNode* ore = new TreeNode();
//	ore->mNodeType = NodeType::OtherRelE;
//	ore->mLine = mCurLine;
//
//	ore->mChilds.push_back(CmpOp());
//	ore->mChilds.push_back(Exp());
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherRelE] Finished")));
//	return ore;
//}

// Exp ::= Term OtherTerm
TreeNode* RSyntaxParser::Simple_exp() {
	
	/* 调用元处理函数term(),函数返回语法树节点指针给t */
	TreeNode * t = Term();
	Token token = GetCurToken();
	/* 当前单词token为加法运算符单词PLUS或MINUS */
	while ((token.lex == PLUS) || (token.lex == MINUS))
	{
		/* 创建新OpK表达式类型语法树节点，新语法树节点指针赋给p */
		TreeNode * p = newExpNode(OpK);
		/* 语法树节点p创建成功,初始化p第一子节点成员child[0] *
	   * 返回语法树节点指针给p的运算符成员attr.op */
		if (p != NULL)
		{
			p->lineno = mCurLine;
			p->child[0] = t;
			//map<LexType, CString> ::iterator it = mLexicalAnalyzer.mLex2String.find(token.lex);
			p->attr->expattr->op = token.lex;
			//p->attr.ExpAttr.op = token.Lex;
			/* 将函数返回值t赋成语法树节点p */
			t = p;
			/* 当前单词token与指定加法运算单词(为PLUS或MINUS)匹配 */
			Match(token.lex);
			/* 调用元处理函数term(),函数返回语法树节点指针给t的第二子节点成员child[1] */
			t->child[1] = Term();
		}
		token = GetCurToken();
	}
	/* 函数返回表达式类型语法树节点t */
	return t;

}



TreeNode* RSyntaxParser::Exp()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Exp]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Exp in line %d"), mCurLine));
	
	TreeNode * t = Simple_exp();
	Token token = GetCurToken();
	if ((token.lex == LT) || (token.lex == EQU))
	{
		
		TreeNode * p = newExpNode(OpK);
		
		if (p != NULL)
		{
			p->lineno = mCurLine;
			p->child[0] = t;
			//map<LexType, CString> ::iterator it = mLexicalAnalyzer.mLex2String.find(token.lex);
			p->attr->expattr->op = token.lex;
			//p->attr.ExpAttr.op = token.Lex;
			/* 将新的表达式类型语法树节点p作为函数返回值t */
			t = p;
		}
		
		Match(token.lex);
		
		if (t != NULL)
			t->child[1] = Simple_exp();
	}
	/*TreeNode* exp = new TreeNode();
	exp->mNodeType = NodeType::Exp;
	exp->mLine = mCurLine;

	exp->mChilds.push_back(Term());
	exp->mChilds.push_back(OtherTerm());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Exp] Finished")));
	return t;
}

// OtherTerm ::= 
//				ε { LT , EQU, RSQUAREBRACKET, THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON,COMMA }
//				| AddOp Exp { PLUS, MINUS }
//TreeNode* RSyntaxParser::OtherTerm()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherTerm]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing OtherTerm in line %d"), mCurLine));
//	TreeNode* ot = new TreeNode();
//	ot->mNodeType = NodeType::OtherTerm;
//	ot->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::LT || GetCurToken().lex == LexType::EQU
//		|| GetCurToken().lex == LexType::RSQUAREBRACKET || GetCurToken().lex == LexType::THEN
//		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
//		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
//		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
//		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
//	{
//		TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
//		ot->mChilds.push_back(n);
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherTerm] is EPSILON")));
//		//LogUtil::Info(Utils::FormatCString(_T("OtherTerm is EPSILON near line %d"), mCurLine));
//	}
//	else if (GetCurToken().lex == LexType::PLUS || GetCurToken().lex == LexType::MINUS)
//	{
//		ot->mChilds.push_back(AddOp());
//		ot->mChilds.push_back(Exp());
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherTerm] Finished")));
//	return ot;
//}

// Term ::= Factor OtherFactor
TreeNode* RSyntaxParser::Term()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Term]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Term in line %d"), mCurLine));
	
	TreeNode * t = Factor();
	Token token = GetCurToken();
	/* 当前单词token为乘法运算符单词TIMES或OVER */
	while ((token.lex == LexType::MULTIPLY) || (token.lex == LexType::DIVIDE))
	{
		/* 创建新的OpK表达式类型语法树节点,新节点指针赋给p */
		TreeNode * p = newExpNode(OpK);
		/* 新语法树节点p创建成功,初始化第一个子节点成员child[0]为t *
		 * 将当前单词token赋值给语法树节点p的运算符成员attr.op */
		if (p != NULL)
		{
			p->lineno = mCurLine;
			p->child[0] = t;
			//map<LexType, CString> ::iterator it = mLexicalAnalyzer.mLex2String.find(token.lex);
			p->attr->expattr->op = token.lex;
			//p->attr.ExpAttr.op = token.Lex;
			t = p;
		}
		/* 当前单词token与指定乘法运算符单词(为TIMES或OVER)匹配 */
		Match(token.lex);
		/* 调用因子处理函数factor(),函数返回语法树节点指针赋给p第二个子节点成员child[1] */
		p->child[1] = Factor();
		token = GetCurToken();
	}

	/*TreeNode* t = new TreeNode();
	t->mNodeType = NodeType::Term;
	t->mLine = mCurLine;
	
	t->mChilds.push_back(Factor());
	t->mChilds.push_back(OtherFactor());*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Term] Finished")));
	return t;
}


// Factor ::= 
//			T-<LPARENTHESIS> Exp T-<RPARENTHESIS>	{ LPARENTHESIS }
//			| T-<UINTEGER>	{ UINTEGER }
//			| Variable { IDENTIFIER }
TreeNode* RSyntaxParser::Factor()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Factor]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Factor in line %d"), mCurLine));
	Token token = GetCurToken();
	TreeNode * t = NULL;
	switch (token.lex)
	{
	case UINTEGER:
		/* 创建新的ConstK表达式类型语法树节点,赋值给t */
		t = newExpNode(ConstK);
		
		if ((t != NULL) && (token.lex == UINTEGER))
		{
			t->lineno = mCurLine;
			t->attr->expattr->val = token.sem;
			//t->attr.ExpAttr.val = atoi();
		}
		
		Match(UINTEGER);
		break;
		
	case IDENTIFIER:

		
		t = Variable();
		break;
		
	case LPARENTHESIS:
		
		Match(LPARENTHESIS);
		
		t = Exp();
		
		Match(RPARENTHESIS);
		break;
		/* 当前单词token为其它单词 */
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	/* 函数返回表达式类型语法树节点t */



	//TreeNode* f = new TreeNode();
	//f->mNodeType = NodeType::Factor;
	//f->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::LPARENTHESIS)	// 向前看 1 个
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::LPARENTHESIS))
	//	{
	//		f->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	f->mChilds.push_back(Exp());

	//	t = GetCurToken();

	//	if (Match(LexType::RPARENTHESIS))
	//	{
	//		f->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else if (GetCurToken().lex == LexType::UINTEGER)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::UINTEGER))
	//	{
	//		f->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else if (GetCurToken().lex == LexType::IDENTIFIER)
	//{
	//	f->mChilds.push_back(Variable());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Factor] Finished")));
	return t;
}


//// OtherFactor ::= 
////				ε { PLUS, MINUS, LT, EQU, RSQUAREBRACKET, THEN , ELSE , FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
////				| MultOp Term { MULTIPLY, DIVIDE}
//TreeNode* RSyntaxParser::OtherFactor()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [OtherFactor]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing OtherFactor in line %d"), mCurLine));
//	TreeNode* of = new TreeNode();
//	of->mNodeType = NodeType::OtherFactor;
//	of->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::PLUS || GetCurToken().lex == LexType::MINUS
//		|| GetCurToken().lex == LexType::LT || GetCurToken().lex == LexType::EQU
//		|| GetCurToken().lex == LexType::RSQUAREBRACKET || GetCurToken().lex == LexType::THEN
//		|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
//		|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
//		|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
//		|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
//	{
//		TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
//		of->mChilds.push_back(n);
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherFactor] is EPSILON")));
//		//LogUtil::Info(Utils::FormatCString(_T("OtherFactor is EPSILON near line %d"), mCurLine));
//	}
//	else if (GetCurToken().lex == LexType::MULTIPLY || GetCurToken().lex == LexType::DIVIDE)
//	{
//		of->mChilds.push_back(MultOp());
//		of->mChilds.push_back(Term());
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[OtherFactor] Finished")));
//	return of;
//}


// Variable ::= T-<IDENTIFIER> VariMore
TreeNode* RSyntaxParser::Variable()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [Variable]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing Variable in line %d"), mCurLine));
	
	TreeNode * t = newExpNode(VariK);
	Token token = GetCurToken();
	if ((t != NULL) && (token.lex == IDENTIFIER))
	{
		t->lineno = mCurLine;
		t->name[0]=token.sem;
		(t->idnum)++;
	}
	Match(IDENTIFIER);
	VariMore(t);



	/*TreeNode* v = new TreeNode();
	v->mNodeType = NodeType::Variable;
	v->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		v->mChilds.push_back(GetMatchedTerminal(t));
	}
	v->mChilds.push_back(VariMore());*/

	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[Variable] Finished")));
	return t;
}

/* 书上产生式93：Predict 集少了 RSQUAREBRACKET */
// VariMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA, RSQUAREBRACKET }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
//				| T-<DOT> FieldVar	{DOT}
void RSyntaxParser::VariMore(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [VariMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing VariMore in line %d"), mCurLine));
	Token token = GetCurToken();

	switch (token.lex)
	{
	case ASSIGN:
	case MULTIPLY:
	case EQU:
	case LT:
	case PLUS:
	case MINUS:
	case DIVIDE:
	case RPARENTHESIS:
	case RSQUAREBRACKET:
	case SEMICOLON:
	case COMMA:
	case THEN:
	case ELSE:
	case FI:
	case DO:
	case ENDWHILE:
	case END:
		break;
	case LSQUAREBRACKET:
		Match(LSQUAREBRACKET);
		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->child[0] = Exp();
		t->attr->expattr->varkind = ArrayMembV;
		
		
		t->child[0]->attr->expattr->varkind = IdV;
		Match(RSQUAREBRACKET);
		break;
	case DOT:
		Match(DOT);
		/*第一个儿子指向域成员变量结点*/
		t->child[0] = FieldVar();
		t->attr->expattr->varkind = FieldMembV;
		t->child[0]->attr->expattr->varkind = IdV;
		
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}
	//TreeNode* vm = new TreeNode();
	//vm->mNodeType = NodeType::VariMore;
	//vm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::MULTIPLY
	//	|| GetCurToken().lex == LexType::DIVIDE || GetCurToken().lex == LexType::PLUS
	//	|| GetCurToken().lex == LexType::MINUS || GetCurToken().lex == LexType::LT
	//	|| GetCurToken().lex == LexType::EQU || GetCurToken().lex == LexType::THEN
	//	|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
	//	|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
	//	|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
	//	|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA
	//	|| GetCurToken().lex == LexType::RSQUAREBRACKET)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	vm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VariMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("VariMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::LSQUAREBRACKET)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::LSQUAREBRACKET))
	//	{
	//		vm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	vm->mChilds.push_back(Exp());
	//	t = GetCurToken();

	//	if (Match(LexType::RSQUAREBRACKET))
	//	{
	//		vm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else if (GetCurToken().lex == LexType::DOT)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::DOT))
	//	{
	//		vm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	vm->mChilds.push_back(FieldVar());
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[VariMore] Finished")));
	//return vm;
}

// FieldVar ::= T-<IDENTIFIER> VariMore
TreeNode* RSyntaxParser::FieldVar()
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldVar]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FieldVar in line %d"), mCurLine));


	

	TreeNode * t = newExpNode(VariK);
	Token token = GetCurToken();
	if ((t != NULL) && (token.lex == IDENTIFIER))
	{
		t->lineno = mCurLine;
		t->name[0]=token.sem;
		(t->idnum)++;
	}
	Match(IDENTIFIER);
	FieldVarMore(t);
	


	/*TreeNode* fv = new TreeNode();
	fv->mNodeType = NodeType::FieldVar;
	fv->mLine = mCurLine;

	Token t = GetCurToken();

	if (Match(LexType::IDENTIFIER))
	{
		fv->mChilds.push_back(GetMatchedTerminal(t));
	}
	fv->mChilds.push_back(VariMore());
*/
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVar] Finished")));
	return t;
}


// FieldVarMore ::= 
//				ε { ASSIGN, MULTIPLY, DIVIDE, PLUS, MINUS, LT, EQU,THEN, ELSE, FI, DO, ENDWH, RPARENTHESIS, END, SEMICOLON, COMMA }
//				| T-<LSQUAREBRACKET> Exp T-<RSQUAREBRACKET> { LSQUAREBRACKET }
void RSyntaxParser::FieldVarMore(TreeNode* t)
{
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [FieldVarMore]")));
	//LogUtil::Info(Utils::FormatCString(_T("Parsing FieldVarMore in line %d"), mCurLine));
	Token token = GetCurToken();

	switch (token.lex)
	{
	case ASSIGN:
	case LexType::MULTIPLY:
	case EQU:
	case LT:
	case PLUS:
	case MINUS:
	case DIVIDE:
	case RPARENTHESIS:
	case SEMICOLON:
	case COMMA:
	case THEN:
	case ELSE:
	case FI:
	case DO:
	case ENDWHILE:
	case END:
		break;
	case LSQUAREBRACKET:
		Match(LSQUAREBRACKET);
		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->child[0] = Exp();
		t->child[0]->attr->expattr->varkind = ArrayMembV;
		
		Match(RSQUAREBRACKET);
		break;
	default:
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		Errorflag = 0;
		Error = true;
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		NextToken();
		break;
	}


	//TreeNode* fvm = new TreeNode();
	//fvm->mNodeType = NodeType::FieldVarMore;
	//fvm->mLine = mCurLine;

	//if (GetCurToken().lex == LexType::ASSIGN || GetCurToken().lex == LexType::MULTIPLY
	//	|| GetCurToken().lex == LexType::DIVIDE || GetCurToken().lex == LexType::PLUS
	//	|| GetCurToken().lex == LexType::MINUS || GetCurToken().lex == LexType::LT
	//	|| GetCurToken().lex == LexType::EQU || GetCurToken().lex == LexType::THEN
	//	|| GetCurToken().lex == LexType::ELSE || GetCurToken().lex == LexType::FI
	//	|| GetCurToken().lex == LexType::DO || GetCurToken().lex == LexType::ENDWHILE
	//	|| GetCurToken().lex == LexType::RPARENTHESIS || GetCurToken().lex == LexType::END
	//	|| GetCurToken().lex == LexType::SEMICOLON || GetCurToken().lex == LexType::COMMA)	// 向前看 1 个
	//{
	//	TreeNode* n = GetMatchedTerminal(Token(LexType::EPSILON));
	//	fvm->mChilds.push_back(n);
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVarMore] is EPSILON")));
	//	//LogUtil::Info(Utils::FormatCString(_T("FieldVarMore is EPSILON near line %d"), mCurLine));
	//}
	//else if (GetCurToken().lex == LexType::LSQUAREBRACKET)
	//{
	//	Token t = GetCurToken();

	//	if (Match(LexType::LSQUAREBRACKET))
	//	{
	//		fvm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//	fvm->mChilds.push_back(Exp());
	//	t = GetCurToken();

	//	if (Match(LexType::RSQUAREBRACKET))
	//	{
	//		fvm->mChilds.push_back(GetMatchedTerminal(t));
	//	}
	//}
	//else
	//{
	//	mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
	//	//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
	//	NextToken();
	//}
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[FieldVarMore] Finished")));
	//return fvm;
}


///* 书上算法部分少了这一条 */
//// CmpOp ::= 
////			T-<LT>	{ LT }
////			|T-<EQU>	{ EQU }
//TreeNode* RSyntaxParser::CmpOp()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [CmpOp]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing CmpOp in line %d"), mCurLine));
//	TreeNode* co = new TreeNode();
//	co->mNodeType = NodeType::CmpOp;
//	co->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::LT)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::LT))
//		{
//			co->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else if (GetCurToken().lex == LexType::EQU)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::EQU))
//		{
//			co->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}	
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[CmpOp] Finished")));
//	return co;
//}
//
//// AddOp ::= 
////			T-<PLUS>	{ PLUS }
////			|T-<MINUS>	{ MINUS }
//TreeNode* RSyntaxParser::AddOp()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [AddOp]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing AddOp in line %d"), mCurLine));
//	TreeNode* ao = new TreeNode();
//	ao->mNodeType = NodeType::AddOp;
//	ao->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::PLUS)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::PLUS))
//		{
//			ao->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else if (GetCurToken().lex == LexType::MINUS)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::MINUS))
//		{
//			ao->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[AddOp] Finished")));
//	return ao;
//}
//
//// MultOp ::= 
////			T-<MULTIPLY> { MULTIPLY }
////			|T-<DIVIDE>	{ DIVIDE }
//TreeNode* RSyntaxParser::MultOp()
//{
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("Parsing [MultOp]")));
//	//LogUtil::Info(Utils::FormatCString(_T("Parsing MultOp in line %d"), mCurLine));
//	TreeNode* mo = new TreeNode();
//	mo->mNodeType = NodeType::MultOp;
//	mo->mLine = mCurLine;
//
//	if (GetCurToken().lex == LexType::MULTIPLY)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::MULTIPLY))
//		{
//			mo->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else if (GetCurToken().lex == LexType::DIVIDE)
//	{
//		Token t = GetCurToken();
//
//		if (Match(LexType::DIVIDE))
//		{
//			mo->mChilds.push_back(GetMatchedTerminal(t));
//		}
//	}
//	else
//	{
//		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
//		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
//		NextToken();
//	}	
//
//	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, _T("[MultOp] Finished")));
//	return mo;
//}



bool RSyntaxParser::Match(LexType type)
{
	if (GetCurToken().lex == type)
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s [%s] Matched"), mLexicalAnalyzer.mLex2String[type], GetCurToken().sem)));
		//LogUtil::Info(Utils::FormatCString(_T("%s [%s] matched near line %d"), mLexicalAnalyzer.mLex2String[type], GetCurToken().sem, mCurLine));
		NextToken();
		mCurLine = GetCurToken().line;
		return true;
	}
	else
	{
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpected %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
		mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mLexicalAnalyzer.mLex2String[type])));
		//LogUtil::Error(Utils::FormatCString(_T("Unexpected %s near line %d"), mLexicalAnalyzer.mLex2String[GetCurToken().lex], mCurLine));
		//LogUtil::Error(Utils::FormatCString(_T("Missing %s near line %d"), mLexicalAnalyzer.mLex2String[type], mCurLine));
		NextToken();
		mCurLine = GetCurToken().line;



		Errorflag = 0;
		Error = true;
		///////////////
		//exit(0);/////暂用该方法推出编译程序,有时间优化
		//////////////////


		return false;
	}
}

//TreeNode* RSyntaxParser::GetMatchedTerminal(Token t)
//{
//	TreeNode* r = new TreeNode();
//	r->mNodeType = NodeType::Terminal;
//	r->mLine = mCurLine;
//	r->mToken = t;
//	return r;
//}



void RSyntaxParser::ReleaseTree(TreeNode* r)
{
	if (r == NULL)
		return;
	for (int i = 0; i < 3; i++)
	{
		ReleaseTree(r->child[i]);
	}
	delete r;
	r = NULL;
}

//void RSyntaxParser::InitMap()
//{
//	mNodeType2Str[NodeType::Program] = _T("Program");
//	mNodeType2Str[NodeType::ProgramHead] = _T("ProgramHead");
//	mNodeType2Str[NodeType::ProgramName] = _T("ProgramName");
//	mNodeType2Str[NodeType::ProgramBody] = _T("ProgramBody");
//	mNodeType2Str[NodeType::StmList] = _T("StmList");
//	mNodeType2Str[NodeType::Stm] = _T("Stm");
//	mNodeType2Str[NodeType::StmMore] = _T("StmMore");
//	mNodeType2Str[NodeType::DeclarePart] = _T("DeclarePart");
//	mNodeType2Str[NodeType::TypeDec] = _T("TypeDec");
//	//mNodeType2Str[NodeType::EMPTY] = _T("EMPTY");
//	mNodeType2Str[NodeType::TypeDecList] = _T("TypeDecList");
//	mNodeType2Str[NodeType::TypeId] = _T("TypeId");
//	mNodeType2Str[NodeType::TypeDef] = _T("TypeDef");
//	mNodeType2Str[NodeType::TypeDecMore] = _T("TypeDecMore");
//	mNodeType2Str[NodeType::TypeDeclaration] = _T("TypeDeclaration");
//	mNodeType2Str[NodeType::BaseType] = _T("BaseType");
//	mNodeType2Str[NodeType::StructureType] = _T("StructureType");
//	mNodeType2Str[NodeType::ArrayType] = _T("ArrayType");
//	mNodeType2Str[NodeType::RecType] = _T("RecType");
//	mNodeType2Str[NodeType::FieldDecList] = _T("FieldDecList");
//	mNodeType2Str[NodeType::IdList] = _T("IdList");
//	mNodeType2Str[NodeType::FieldDecMore] = _T("FieldDecMore");
//	mNodeType2Str[NodeType::IdMore] = _T("IdMore");
//	mNodeType2Str[NodeType::VarDec] = _T("VarDec");
//	mNodeType2Str[NodeType::VarDeclaration] = _T("VarDeclaration");
//	mNodeType2Str[NodeType::VarDecList] = _T("VarDecList");
//	mNodeType2Str[NodeType::VarIdList] = _T("VarIdList");
//	mNodeType2Str[NodeType::VarIdMore] = _T("VarIdMore");
//	mNodeType2Str[NodeType::VarDecMore] = _T("VarDecMore");
//	mNodeType2Str[NodeType::ProcDec] = _T("ProcDec");
//	mNodeType2Str[NodeType::ProcDeclaration] = _T("ProcDeclaration");
//	mNodeType2Str[NodeType::ProcDecMore] = _T("ProcDecMore");
//	mNodeType2Str[NodeType::ParamList] = _T("ParamList");
//	mNodeType2Str[NodeType::ParamDecList] = _T("ParamDecList");
//	mNodeType2Str[NodeType::Param] = _T("Param");
//	mNodeType2Str[NodeType::ParamMore] = _T("ParamMore");
//	mNodeType2Str[NodeType::FormList] = _T("FormList");
//	mNodeType2Str[NodeType::FidMore] = _T("FidMore");
//	//mNodeType2Str[NodeType::ProcDecPart] = _T("ProcDecPart");
//	mNodeType2Str[NodeType::ProcBody] = _T("ProcBody");
//	mNodeType2Str[NodeType::AssCall] = _T("AssCall");
//	mNodeType2Str[NodeType::AssignmentRest] = _T("AssignmentRest");
//	mNodeType2Str[NodeType::ConditionalStm] = _T("ConditionalStm");
//	mNodeType2Str[NodeType::LoopStm] = _T("LoopStm");
//	mNodeType2Str[NodeType::InputStm] = _T("InputStm");
//	mNodeType2Str[NodeType::OutputStm] = _T("OutputStm");
//	mNodeType2Str[NodeType::ReturnStm] = _T("ReturnStm");
//	mNodeType2Str[NodeType::CallStmRest] = _T("CallStmRest");
//	mNodeType2Str[NodeType::ActParamList] = _T("ActParamList");
//	mNodeType2Str[NodeType::ActParamMore] = _T("ActParamMore");
//	mNodeType2Str[NodeType::Exp] = _T("Exp");
//	mNodeType2Str[NodeType::OtherTerm] = _T("OtherTerm");
//	mNodeType2Str[NodeType::Term] = _T("Term");
//	mNodeType2Str[NodeType::OtherFactor] = _T("OtherFactor");
//	mNodeType2Str[NodeType::Factor] = _T("Factor");
//	mNodeType2Str[NodeType::Variable] = _T("Variable");
//	mNodeType2Str[NodeType::VariMore] = _T("VariMore");
//	mNodeType2Str[NodeType::FieldVar] = _T("FieldVar");
//	mNodeType2Str[NodeType::FieldVarMore] = _T("FieldVarMore");
//	mNodeType2Str[NodeType::RelExp] = _T("RelExp");
//	mNodeType2Str[NodeType::OtherRelE] = _T("OtherRelE");
//	mNodeType2Str[NodeType::CmpOp] = _T("CmpOp");
//	mNodeType2Str[NodeType::AddOp] = _T("AddOp");
//	mNodeType2Str[NodeType::MultOp] = _T("MultOp");
//	mNodeType2Str[NodeType::Terminal] = _T("Terminal");
//}
void RSyntaxParser::printTree(TreeNode * tree)
{
	CString temp = _T("");
	/*int a = 1231231;
	int b = 133;

	temp.Format(_T("%d  %d  \r\n"), a, b);
	Treemessage += temp;

	a = 111;
	b = 222;

	temp.Format(_T("%d  %d  \r\n"), a, b);
	Treemessage += temp;*/



	int i;
	/* 增量缩进宏,每次进入语法树节点都进行增量缩进 */
	INDENT;
	/* 函数参数给定语法树节点指针tree非NULL(空) */
	while (tree != NULL)
	{
		///*打印行号*/
		//if (tree->lineno == 0)
		//	printTab(9);
		//else
		//	switch ((int)(tree->lineno / 10))
		//	{
		//	case 0:
		//		temp.Format(_T("line:%d"), tree->lineno);
		//		
		//		Treemessage += temp;
		//		printTab(3);
		//		break;
		//	case 1:
		//	case 2:
		//	case 3:
		//	case 4:
		//	case 5:
		//	case 6:
		//	case 7:
		//	case 8:
		//	case 9:
		//		temp.Format(_T("line:%d"), tree->lineno);
		//		Treemessage += temp; 
		//		printTab(2);
		//		break;
		//	default:
		//		Treemessage += _T("line:%d", tree->lineno);
		//		printTab(1);
		//	}
		/* 调用函数printSpaces,打印相应的空格,进行缩进 */
		printSpaces();

		switch (tree->nodekind)
		{
		case ProK:
			Treemessage += _T("ProK  ", ""); break;
		case PheadK:
		{
			Treemessage += _T("PheadK  ", "");
			Treemessage +=tree->name[0];
		}
		break;
		case DecK:
		{ 
			Treemessage += _T("DecK  ", "");
			if (tree->attr->proattr->paramt == varparamtype)
			
				Treemessage += _T("var param:  ", "");
			if (tree->attr->proattr->paramt == valparamtype)
			
				Treemessage += _T("value param:  ", "");
		
			switch (tree->kind->dec)
			{
			case  ArrayK:
			{
				Treemessage += _T("ArrayK  ", "");
				temp = _T("");
				temp.Format(_T("%d  "), tree->attr->arrayattr->up);
				Treemessage += temp;
				temp = _T("");
				temp.Format(_T("%d  "), tree->attr->arrayattr->low);
				Treemessage +=temp;

				if (tree->attr->arrayattr->childType == CharK)
					Treemessage += _T("Chark  ", "");
				else if (tree->attr->arrayattr->childType == IntegerK)
					Treemessage += _T("IntegerK  ", "");
			}; break;
			case  CharK:
				Treemessage += _T("CharK  ", ""); break;
			case  IntegerK:
				Treemessage += _T("IntegerK  ", ""); break;
			case  RecordK:
				Treemessage += _T("RecordK  ", ""); break;
			case  IdK:
				Treemessage += _T("IdK  ", "");
				Treemessage += tree->attr->type_name;
				Treemessage += _T("  ");
				
				break;
			default:
				Treemessage += _T("error1!");
				Error = TRUE;
			};
			if (tree->idnum != 0)
				for (int i = 0; i <= (tree->idnum); i++)
				{
					Treemessage += tree->name[i];
					
					Treemessage += _T("  ");
				}
			else
			{
				Treemessage += _T("wrong!no var!\r\n");
				Error = TRUE;
			}
		} break;
		case TypeK:
			Treemessage += _T("TypeK  ", ""); break;

		case VarK:
			Treemessage += _T("VarK  ", "");
			if (tree->table[0] != NULL) {
				temp = _T("");
				temp.Format(_T("%d  %d  "), tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);
				Treemessage += temp;
			}
			break;

		case ProcDecK:
			Treemessage += _T("ProcDecK  ", "");
			Treemessage += tree->name[0];

			Treemessage += _T("  ");
			
			if (tree->table[0] != NULL) {
				temp.Format(_T("%d %d %d  "), tree->table[0]->attrIR.More.ProcAttr.mOff, tree->table[0]->attrIR.More.ProcAttr.nOff, tree->table[0]->attrIR.More.ProcAttr.level);
				Treemessage += temp;
			}
			break;

		case StmLK:
			Treemessage += _T("StmLk  ", ""); break;

		case StmtK:
		{ 
			Treemessage += _T("StmtK  ", "");
			switch (tree->kind->stmt)
			{
			case IfK:
				Treemessage += _T("If  ", ""); break;
			case WhileK:
				Treemessage += _T("While  ", ""); break;

			case AssignK:
				Treemessage += _T("Assign  ", "");
				break;

			case ReadK:
				Treemessage += _T("Read  ", "");
				
				Treemessage += tree->name[0];
				Treemessage += _T("  ");
				if (tree->table[0] != NULL) {
					temp.Format( _T("%d   %d  "), tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);
					Treemessage += temp;
				}break;

			case WriteK:
				Treemessage += _T("Write  ", ""); break;

			case CallK:
				Treemessage += _T("Call  ", "");
				Treemessage += tree->name[0];
				Treemessage += _T("  ");
				break;

			case ReturnK:
				Treemessage += _T("Return  ", ""); break;

			default:
				Treemessage += _T("error2!");
				Error = TRUE;
			}
		}; break;
		case ExpK:
		{ 
			Treemessage += _T("ExpK  ", "");
			switch (tree->kind->exp)
			{
			case OpK:
			{
				Treemessage += _T("Op  ", "");
				switch (tree->attr->expattr->op)
				{
				case EQU:   Treemessage += _T("=  ", ""); break;
				case LT:   Treemessage += _T("<  ", ""); break;
				case PLUS: Treemessage += _T("+  ", ""); break;
				case MINUS:Treemessage += _T("-  ", ""); break;
				case LexType::MULTIPLY:Treemessage += _T("*  ", ""); break;
				case LexType::DIVIDE: Treemessage += _T("/  ", ""); break;
				default:
					Treemessage += _T("error3!");
					Error = TRUE;
				}
				
				if (tree->attr->expattr->varkind == ArrayMembV)
				{
					
					Treemessage += _T("ArrayMember  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");
				}
			}; break;
			case ConstK:
				Treemessage += _T("Const  ", "");
				switch (tree->attr->expattr->varkind)
				{
					
				case IdV:
					Treemessage += _T("Id  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");
					//Treemessage += _T("%s  ", tree->name[0]);
					break;
				case FieldMembV:
					Treemessage += _T("FieldMember  ");
					
					Treemessage += tree->name[0];
					Treemessage += _T("  ");
					//Treemessage += _T("%s  ", tree->name[0]);
					break;
				case ArrayMembV:
					Treemessage += _T("ArrayMember  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");
					//Treemessage += _T("%s  ", tree->name[0]);
					break;
				default:
					Treemessage += _T("var type error!");
					Error = TRUE;
				}
				
				 Treemessage += tree->attr->expattr->val;
				 Treemessage += _T("  ");
				break;
			case VariK:
				Treemessage += _T("Vari  ", "");
				switch (tree->attr->expattr->varkind)
				{
					
				case IdV:
					Treemessage += _T("Id  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");

					//Treemessage += _T("%s  ", tree->name[0]);
					break;
				case FieldMembV:
					Treemessage += _T("FieldMember  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");

					//fprintf(listing, "%s  ", tree->name[0]);
					break;
				case ArrayMembV:
					Treemessage += _T("ArrayMember  ");
					Treemessage += tree->name[0];
					Treemessage += _T("  ");

					//fprintf(listing, "%s  ", tree->name[0]);
					break;
				default:
					Treemessage += _T("var type error!");
					Error = TRUE;
				}
				if (tree->table[0] != NULL) {
					temp.Format(_T("%d   %d  "), tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);
					Treemessage += temp;
				}
				break;
			default:
				Treemessage += _T("error4!");
				Error = TRUE;
			}
		}; break;
		default:
			Treemessage += _T("error5!");
			Error = TRUE;
		}

		Treemessage += _T("\r\n");

		/* 对语法树结点tree的各子结点递归调用printTree过程 *
	 * 缩进写入列表文件listing    */
		for (i = 0; i < 3; i++)
			printTree(tree->child[i]);
		/* 对语法树结点tree的兄弟节点递归调用printTree过程 *
		 * 缩进写入列表文件listing    */
		tree = tree->sibling;
	}
	/* 减量缩进宏,每次退出语法树节点时减量缩进 */
	UNINDENT;
}
void RSyntaxParser::printTab(int tabnum)
{
	for (int i = 0; i < tabnum; i++)
		Treemessage+=_T(" ");
}
void RSyntaxParser::printSpaces()
{
	
	/* 按给定缩进量indentno打印空格进行缩进 *
	 * 其中缩进量indentno总能保持非负 */
	for (int i = 0; i < indentno; i++)
		Treemessage += _T(" ");
		//fprintf(listing, " ");

}
CString RSyntaxParser::GetSyntaxTreeStr(CString lftstr, CString append, TreeNode* r)
{

	CString b = append;
	/*if (r->mNodeType == NodeType::Terminal)
		b += mLexicalAnalyzer.mLex2String[r->mToken.lex];
	else
		b += mNodeType2Str[r->mNodeType];
	b += _T("\r\n");;

	vector<TreeNode*> childs = r->mChilds;
	if (childs.size() > 0)
	{
		for (int i = 0; i < childs.size() - 1; i++)
			b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      | "), _T("      |-"), childs[i]));
		b += (lftstr + GetSyntaxTreeStr(lftstr + _T("      "), _T("      |-"), childs[childs.size() - 1]));
	}*/
	return b;
}



