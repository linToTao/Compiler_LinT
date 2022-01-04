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
	mTokenPtr = 0;
	mCurLine = 1;
	InitMap();
}

LL1SyntaxParser::LL1SyntaxParser(vector<Token> tokens)
{
	for (int i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].lex != LexType::LEXERR)
			mTokenList.push_back(tokens[i]);
	}
	mTokenPtr = 0;
	mCurLine = 1;
	InitMap();
}

LL1SyntaxParser::~LL1SyntaxParser()
{
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

void LL1SyntaxParser::Parse()
{
	stack<StackItem> stk;
	StackItem item(NodeType::Program, Token());

	stk.push(item);
	mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("Program pushed!"))));

	while (stk.size() != 0 || GetCurToken().lex != LexType::LEXEOF)
	{

		if (stk.size() != 0 && stk.top().nodeType == NodeType::EMPTY)
		{
			stk.pop();
			continue;
		}
		if (stk.size() == 0)
		{
			mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Unexpeted token %s"), mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
			return;
		}
		else if (GetCurToken().lex == LexType::LEXEOF)
		{
			StackItem item = stk.top();
			if (item.nodeType == NodeType::Terminal)
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mLexicalAnalyzer.mLex2String[item.token.lex])));
			else
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("Missing %s"), mNodeType2Str[item.nodeType])));
			return;
		}

		StackItem item = stk.top();
		if (item.nodeType == NodeType::Terminal)
		{
			if (item.token.lex != GetCurToken().lex)
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LERROR, Utils::FormatCString(_T("%s and %s couldn't be matched!"), mLexicalAnalyzer.mLex2String[item.token.lex], mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
				return;
			}
			else
			{
				mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s and %s matched!"), mLexicalAnalyzer.mLex2String[item.token.lex], mLexicalAnalyzer.mLex2String[GetCurToken().lex])));
				NextToken(); mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s popped!"), mLexicalAnalyzer.mLex2String[stk.top().token.lex])));
				stk.pop();
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
				vector<StackItem> items = mLL1Map[item.nodeType][GetCurToken().lex];
				for (int i = items.size() - 1; i >= 0; i--)
				{
					stk.push(items[i]);
					if (items[i].nodeType == NodeType::Terminal)
						mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s pushed!"), mLexicalAnalyzer.mLex2String[items[i].token.lex])));
					else
						mParseLog.push_back(ParseLog(mCurLine, LogType::LINFO, Utils::FormatCString(_T("%s pushed!"), mNodeType2Str[items[i].nodeType])));
				}
				//NextToken();
			}
		}
	}
	

}

void LL1SyntaxParser::InitMap()
{
	mLL1Map[NodeType::Program][LexType::PROGRAM] = Get1();
	mLL1Map[NodeType::ProgramHead][LexType::PROGRAM] = Get2();
	mLL1Map[NodeType::ProgramName][LexType::IDENTIFIER] = Get3();
	mLL1Map[NodeType::DeclarePart][LexType::TYPE] = Get4();
	mLL1Map[NodeType::DeclarePart][LexType::VAR] = Get4();
	mLL1Map[NodeType::DeclarePart][LexType::PROCEDURE] = Get4();
	mLL1Map[NodeType::DeclarePart][LexType::BEGIN] = Get4();
	mLL1Map[NodeType::TypeDec][LexType::TYPE] = Get6();
	mLL1Map[NodeType::TypeDec][LexType::VAR] = Get5();
	mLL1Map[NodeType::TypeDec][LexType::PROCEDURE] = Get5();
	mLL1Map[NodeType::TypeDec][LexType::BEGIN] = Get5();
	mLL1Map[NodeType::TypeDeclaration][LexType::TYPE] = Get7();
	mLL1Map[NodeType::TypeDecList][LexType::IDENTIFIER] = Get8();
	mLL1Map[NodeType::TypeDecMore][LexType::VAR] = Get9();
	mLL1Map[NodeType::TypeDecMore][LexType::PROCEDURE] = Get9();
	mLL1Map[NodeType::TypeDecMore][LexType::BEGIN] = Get9();
	mLL1Map[NodeType::TypeDecMore][LexType::IDENTIFIER] = Get10();
	mLL1Map[NodeType::TypeId][LexType::IDENTIFIER] = Get11();
	mLL1Map[NodeType::TypeDef][LexType::INTEGER] = Get12();
	mLL1Map[NodeType::TypeDef][LexType::CHARACTER] = Get12();
	mLL1Map[NodeType::TypeDef][LexType::ARRAY] = Get13();
	mLL1Map[NodeType::TypeDef][LexType::RECORD] = Get13();
	mLL1Map[NodeType::TypeDef][LexType::IDENTIFIER] = Get14();
	mLL1Map[NodeType::BaseType][LexType::INTEGER] = Get15();
	mLL1Map[NodeType::BaseType][LexType::CHARACTER] = Get16();
	mLL1Map[NodeType::StructureType][LexType::ARRAY] = Get17();
	mLL1Map[NodeType::StructureType][LexType::RECORD] = Get18();
	mLL1Map[NodeType::ArrayType][LexType::ARRAY] = Get19();
	mLL1Map[NodeType::RecType][LexType::RECORD] = Get22();
	mLL1Map[NodeType::FieldDecList][LexType::INTEGER] = Get23();
	mLL1Map[NodeType::FieldDecList][LexType::CHARACTER] = Get23();
	mLL1Map[NodeType::FieldDecList][LexType::ARRAY] = Get24();
	mLL1Map[NodeType::FieldDecMore][LexType::INTEGER] = Get26();
	mLL1Map[NodeType::FieldDecMore][LexType::CHARACTER] = Get26();
	mLL1Map[NodeType::FieldDecMore][LexType::ARRAY] = Get26();
	mLL1Map[NodeType::FieldDecMore][LexType::END] = Get25();
	mLL1Map[NodeType::IdList][LexType::IDENTIFIER] = Get27();
	mLL1Map[NodeType::IdMore][LexType::SEMICOLON] = Get28();
	mLL1Map[NodeType::IdMore][LexType::COMMA] = Get29();
	mLL1Map[NodeType::VarDec][LexType::VAR] = Get31();
	mLL1Map[NodeType::VarDec][LexType::PROCEDURE] = Get30();
	mLL1Map[NodeType::VarDec][LexType::BEGIN] = Get30();
	mLL1Map[NodeType::VarDeclaration][LexType::VAR] = Get32();
	mLL1Map[NodeType::VarDecList][LexType::INTEGER] = Get33();
	mLL1Map[NodeType::VarDecList][LexType::CHARACTER] = Get33();
	mLL1Map[NodeType::VarDecList][LexType::ARRAY] = Get33();
	mLL1Map[NodeType::VarDecList][LexType::RECORD] = Get33();
	mLL1Map[NodeType::VarDecList][LexType::IDENTIFIER] = Get33();
	mLL1Map[NodeType::VarDecMore][LexType::INTEGER] = Get35();
	mLL1Map[NodeType::VarDecMore][LexType::CHARACTER] = Get35();
	mLL1Map[NodeType::VarDecMore][LexType::RECORD] = Get35();
	mLL1Map[NodeType::VarDecMore][LexType::ARRAY] = Get35();
	mLL1Map[NodeType::VarDecMore][LexType::IDENTIFIER] = Get35();
	mLL1Map[NodeType::VarDecMore][LexType::PROCEDURE] = Get34();
	mLL1Map[NodeType::VarDecMore][LexType::BEGIN] = Get34();
	mLL1Map[NodeType::VarIdList][LexType::IDENTIFIER] = Get36();
	mLL1Map[NodeType::VarIdMore][LexType::SEMICOLON] = Get37();
	mLL1Map[NodeType::VarIdMore][LexType::COMMA] = Get38();
	mLL1Map[NodeType::ProcDec][LexType::PROCEDURE] = Get40();
	mLL1Map[NodeType::ProcDec][LexType::BEGIN] = Get39();
	mLL1Map[NodeType::ProcDeclaration][LexType::PROCEDURE] = Get41();
	mLL1Map[NodeType::ProcDecMore][LexType::PROCEDURE] = Get43();
	mLL1Map[NodeType::ProcDecMore][LexType::BEGIN] = Get42();
	mLL1Map[NodeType::ParamList][LexType::INTEGER] = Get46();
	mLL1Map[NodeType::ParamList][LexType::CHARACTER] = Get46();
	mLL1Map[NodeType::ParamList][LexType::ARRAY] = Get46();
	mLL1Map[NodeType::ParamList][LexType::RECORD] = Get46();
	mLL1Map[NodeType::ParamList][LexType::VAR] = Get46();
	mLL1Map[NodeType::ParamList][LexType::IDENTIFIER] = Get46();
	mLL1Map[NodeType::ParamList][LexType::RPARENTHESIS] = Get45();
	mLL1Map[NodeType::ParamDecList][LexType::INTEGER] = Get47();
	mLL1Map[NodeType::ParamDecList][LexType::CHARACTER] = Get47();
	mLL1Map[NodeType::ParamDecList][LexType::ARRAY] = Get47();
	mLL1Map[NodeType::ParamDecList][LexType::RECORD] = Get47();
	mLL1Map[NodeType::ParamDecList][LexType::VAR] = Get47();
	mLL1Map[NodeType::ParamDecList][LexType::IDENTIFIER] = Get47();
	mLL1Map[NodeType::ParamMore][LexType::SEMICOLON] = Get49();
	mLL1Map[NodeType::ParamMore][LexType::RPARENTHESIS] = Get48();
	mLL1Map[NodeType::Param][LexType::INTEGER] = Get50();
	mLL1Map[NodeType::Param][LexType::CHARACTER] = Get50();
	mLL1Map[NodeType::Param][LexType::ARRAY] = Get50();
	mLL1Map[NodeType::Param][LexType::RECORD] = Get50();
	mLL1Map[NodeType::Param][LexType::IDENTIFIER] = Get50();
	mLL1Map[NodeType::Param][LexType::VAR] = Get51();
	mLL1Map[NodeType::FormList][LexType::IDENTIFIER] = Get52();
	mLL1Map[NodeType::FidMore][LexType::SEMICOLON] = Get53();
	mLL1Map[NodeType::FidMore][LexType::RPARENTHESIS] = Get53();
	mLL1Map[NodeType::FidMore][LexType::COMMA] = Get54();
	mLL1Map[NodeType::ProcDecPart][LexType::TYPE] = Get55();
	mLL1Map[NodeType::ProcDecPart][LexType::PROCEDURE] = Get55();
	mLL1Map[NodeType::ProcDecPart][LexType::VAR] = Get55();
	mLL1Map[NodeType::ProcDecPart][LexType::BEGIN] = Get55();
	mLL1Map[NodeType::ProcBody][LexType::BEGIN] = Get56();
	mLL1Map[NodeType::ProgramBody][LexType::BEGIN] = Get57();
	mLL1Map[NodeType::StmList][LexType::IF] = Get58();
	mLL1Map[NodeType::StmList][LexType::WHILE] = Get58();
	mLL1Map[NodeType::StmList][LexType::READ] = Get58();
	mLL1Map[NodeType::StmList][LexType::WRITE] = Get58();
	mLL1Map[NodeType::StmList][LexType::RETURN] = Get58();
	mLL1Map[NodeType::StmList][LexType::IDENTIFIER] = Get58();
	mLL1Map[NodeType::StmMore][LexType::END] = Get59();
	mLL1Map[NodeType::StmMore][LexType::RECORD] = Get59();
	mLL1Map[NodeType::StmMore][LexType::ELSE] = Get59();
	mLL1Map[NodeType::StmMore][LexType::FI] = Get59();
	mLL1Map[NodeType::StmMore][LexType::ENDWHILE] = Get59();
	mLL1Map[NodeType::StmMore][LexType::SEMICOLON] = Get60();
	mLL1Map[NodeType::Stm][LexType::IF] = Get61();
	mLL1Map[NodeType::Stm][LexType::WHILE] = Get62();
	mLL1Map[NodeType::Stm][LexType::READ] = Get63();
	mLL1Map[NodeType::Stm][LexType::WRITE] = Get64();
	mLL1Map[NodeType::Stm][LexType::RETURN] = Get65();
	mLL1Map[NodeType::Stm][LexType::IDENTIFIER] = Get66();
	mLL1Map[NodeType::AssCall][LexType::DOT] = Get67();
	mLL1Map[NodeType::AssCall][LexType::LSQUAREBRACKET] = Get67();
	mLL1Map[NodeType::AssCall][LexType::ASSIGN] = Get67();
	mLL1Map[NodeType::AssCall][LexType::LPARENTHESIS] = Get68();
	mLL1Map[NodeType::AssignmentRest][LexType::DOT] = Get69();
	mLL1Map[NodeType::AssignmentRest][LexType::ASSIGN] = Get69();
	mLL1Map[NodeType::AssignmentRest][LexType::LSQUAREBRACKET] = Get69();
	mLL1Map[NodeType::ConditionalStm][LexType::IF] = Get70();
	mLL1Map[NodeType::LoopStm][LexType::WHILE] = Get71();
	mLL1Map[NodeType::InputStm][LexType::READ] = Get72();
	mLL1Map[NodeType::OutputStm][LexType::WRITE] = Get74();
	mLL1Map[NodeType::ReturnStm][LexType::RETURN] = Get75();
	mLL1Map[NodeType::CallStmRest][LexType::LPARENTHESIS] = Get76();
	mLL1Map[NodeType::ActParamList][LexType::IDENTIFIER] = Get78();
	mLL1Map[NodeType::ActParamList][LexType::LPARENTHESIS] = Get78();
	mLL1Map[NodeType::ActParamList][LexType::UINTEGER] = Get78();
	mLL1Map[NodeType::ActParamList][LexType::RPARENTHESIS] = Get77();
	mLL1Map[NodeType::ActParamMore][LexType::RPARENTHESIS] = Get79();
	mLL1Map[NodeType::ActParamMore][LexType::COMMA] = Get80();
	mLL1Map[NodeType::RelExp][LexType::IDENTIFIER] = Get81();
	mLL1Map[NodeType::RelExp][LexType::LPARENTHESIS] = Get81();
	mLL1Map[NodeType::RelExp][LexType::UINTEGER] = Get81();
	mLL1Map[NodeType::OtherRelE][LexType::LT] = Get82();
	mLL1Map[NodeType::OtherRelE][LexType::MINUS] = Get82();


	mLL1Map[NodeType::Exp][LexType::UINTEGER] = Get83();
	mLL1Map[NodeType::Exp][LexType::IDENTIFIER] = Get83();
	mLL1Map[NodeType::Exp][LexType::LPARENTHESIS] = Get83();
	mLL1Map[NodeType::OtherTerm][LexType::END] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::THEN] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::ELSE] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::FI] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::DO] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::ENDWHILE] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::SEMICOLON] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::COMMA] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::RPARENTHESIS] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::RSQUAREBRACKET] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::LT] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::EQU] = Get84();
	mLL1Map[NodeType::OtherTerm][LexType::PLUS] = Get85();
	mLL1Map[NodeType::OtherTerm][LexType::MINUS] = Get85();

	mLL1Map[NodeType::Term][LexType::UINTEGER] = Get86();
	mLL1Map[NodeType::Term][LexType::IDENTIFIER] = Get86();
	mLL1Map[NodeType::Term][LexType::LPARENTHESIS] = Get86();

	mLL1Map[NodeType::OtherFactor][LexType::END] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::THEN] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::ELSE] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::FI] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::DO] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::ENDWHILE] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::SEMICOLON] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::COMMA] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::RPARENTHESIS] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::RSQUAREBRACKET] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::LT] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::EQU] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::PLUS] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::MINUS] = Get87();
	mLL1Map[NodeType::OtherFactor][LexType::MULTIPLY] = Get88();
	mLL1Map[NodeType::OtherFactor][LexType::DIVIDE] = Get88();

	mLL1Map[NodeType::Factor][LexType::UINTEGER] = Get90();
	mLL1Map[NodeType::Factor][LexType::IDENTIFIER] = Get91();
	mLL1Map[NodeType::Factor][LexType::LPARENTHESIS] = Get89();
	mLL1Map[NodeType::Variable][LexType::IDENTIFIER] = Get92();
	mLL1Map[NodeType::VariMore][LexType::END] = Get93();
	mLL1Map[NodeType::VariMore][LexType::THEN] = Get93();
	mLL1Map[NodeType::VariMore][LexType::ELSE] = Get93();
	mLL1Map[NodeType::VariMore][LexType::FI] = Get93();
	mLL1Map[NodeType::VariMore][LexType::DO] = Get93();
	mLL1Map[NodeType::VariMore][LexType::ENDWHILE] = Get93();
	mLL1Map[NodeType::VariMore][LexType::DOT] = Get95();
	mLL1Map[NodeType::VariMore][LexType::SEMICOLON] = Get93();
	mLL1Map[NodeType::VariMore][LexType::COMMA] = Get93();
	mLL1Map[NodeType::VariMore][LexType::RPARENTHESIS] = Get93();
	mLL1Map[NodeType::VariMore][LexType::LSQUAREBRACKET] = Get94();
	mLL1Map[NodeType::VariMore][LexType::RSQUAREBRACKET] = Get93();
	mLL1Map[NodeType::VariMore][LexType::LT] = Get93();
	mLL1Map[NodeType::VariMore][LexType::EQU] = Get93();
	mLL1Map[NodeType::VariMore][LexType::PLUS] = Get93();
	mLL1Map[NodeType::VariMore][LexType::MINUS] = Get93();
	mLL1Map[NodeType::VariMore][LexType::MULTIPLY] = Get93();
	mLL1Map[NodeType::VariMore][LexType::DIVIDE] = Get93();
	mLL1Map[NodeType::VariMore][LexType::ASSIGN] = Get93();

	mLL1Map[NodeType::FieldVar][LexType::IDENTIFIER] = Get96();
	mLL1Map[NodeType::FieldVarMore][LexType::END] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::THEN] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::ELSE] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::FI] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::DO] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::ENDWHILE] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::SEMICOLON] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::COMMA] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::RPARENTHESIS] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::LSQUAREBRACKET] = Get98();
	mLL1Map[NodeType::FieldVarMore][LexType::RSQUAREBRACKET] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::LT] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::EQU] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::PLUS] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::MINUS] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::MULTIPLY] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::DIVIDE] = Get97();
	mLL1Map[NodeType::FieldVarMore][LexType::ASSIGN] = Get97();

	mLL1Map[NodeType::CmpOp][LexType::LT] = Get99();
	mLL1Map[NodeType::CmpOp][LexType::EQU] = Get100();
	mLL1Map[NodeType::AddOp][LexType::PLUS] = Get101();
	mLL1Map[NodeType::AddOp][LexType::MINUS] = Get102();

	mLL1Map[NodeType::MultOp][LexType::MULTIPLY] = Get103();
	mLL1Map[NodeType::MultOp][LexType::DIVIDE] = Get104();

	mNodeType2Str[NodeType::Program] = _T("Program");
	mNodeType2Str[NodeType::ProgramHead] = _T("ProgramHead");
	mNodeType2Str[NodeType::ProgramName] = _T("ProgramName");
	mNodeType2Str[NodeType::ProgramBody] = _T("ProgramBody");
	mNodeType2Str[NodeType::StmList] = _T("StmList");
	mNodeType2Str[NodeType::Stm] = _T("Stm");
	mNodeType2Str[NodeType::StmMore] = _T("StmMore");
	mNodeType2Str[NodeType::DeclarePart] = _T("DeclarePart");
	mNodeType2Str[NodeType::TypeDec] = _T("TypeDec");
	mNodeType2Str[NodeType::EMPTY] = _T("EMPTY");
	mNodeType2Str[NodeType::TypeDecList] = _T("TypeDecList");
	mNodeType2Str[NodeType::TypeId] = _T("TypeId");
	mNodeType2Str[NodeType::TypeDef] = _T("TypeDef");
	mNodeType2Str[NodeType::TypeDecMore] = _T("TypeDecMore");
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

}

vector<StackItem> LL1SyntaxParser::Get1()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProgramHead, Token()));
	item.push_back(StackItem(NodeType::DeclarePart, Token()));
	item.push_back(StackItem(NodeType::ProgramBody, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DOT, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get2()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PROGRAM, _T(""))));
	item.push_back(StackItem(NodeType::ProgramName, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get3()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get4()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDec, Token()));
	item.push_back(StackItem(NodeType::VarDec, Token()));
	item.push_back(StackItem(NodeType::ProcDec, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get5()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get6()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDeclaration, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get7()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::TYPE, _T(""))));
	item.push_back(StackItem(NodeType::TypeDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get8()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeId, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::EQU, _T(""))));
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::TypeDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get9()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get10()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get11()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get12()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::BaseType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get13()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::StructureType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get14()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get15()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::INTEGER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get16()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::CHARACTER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get17()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ArrayType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get18()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::RecType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get19()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ARRAY, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UNDERANGE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::OF, _T(""))));
	item.push_back(StackItem(NodeType::BaseType, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get22()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RECORD, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::END, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get23()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::BaseType, Token()));
	item.push_back(StackItem(NodeType::IdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get24()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ArrayType, Token()));
	item.push_back(StackItem(NodeType::IdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::FieldDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get25()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get26()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::FieldDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get27()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::IdMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get28()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get29()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::IdList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get30()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get31()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VarDeclaration, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get32()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::VAR, _T(""))));
	item.push_back(StackItem(NodeType::VarDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get33()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::VarIdList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::VarDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get34()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get35()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VarDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get36()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::VarIdMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get37()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get38()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::VarIdList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get39()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get40()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProcDeclaration, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get41()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PROCEDURE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::ParamList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::ProcDecPart, Token()));
	item.push_back(StackItem(NodeType::ProcBody, Token()));
	item.push_back(StackItem(NodeType::ProcDecMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get42()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get43()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProcDeclaration, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get44()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get45()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get46()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ParamDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get47()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Param, Token()));
	item.push_back(StackItem(NodeType::ParamMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get48()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get49()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::ParamDecList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get50()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get51()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::VAR, _T(""))));
	item.push_back(StackItem(NodeType::TypeDef, Token()));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get52()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::FidMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get53()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get54()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::FormList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get55()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::DeclarePart, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get56()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ProgramBody, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get57()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::BEGIN, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::END, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get58()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Stm, Token()));
	item.push_back(StackItem(NodeType::StmMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get59()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get60()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::SEMICOLON, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get61()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ConditionalStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get62()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::LoopStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get63()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::InputStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get64()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::OutputStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get65()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::ReturnStm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get66()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::AssCall, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get67()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::AssignmentRest, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get68()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::CallStmRest, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get69()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::VariMore, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ASSIGN, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get70()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IF, _T(""))));
	item.push_back(StackItem(NodeType::RelExp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::THEN, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ELSE, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::FI, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get71()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::WHILE, _T(""))));
	item.push_back(StackItem(NodeType::RelExp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DO, _T(""))));
	item.push_back(StackItem(NodeType::StmList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::ENDWHILE, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get72()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::READ, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get73()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get74()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::WRITE, _T(""))));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get75()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RETURN, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get76()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::ActParamList, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get77()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get78()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::ActParamMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get79()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get80()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::COMMA, _T(""))));
	item.push_back(StackItem(NodeType::ActParamList, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get81()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::OtherRelE, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get82()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::CmpOp, Token()));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get83()
{
	vector<StackItem> item;
		item.push_back(StackItem(NodeType::Term, Token()));
		item.push_back(StackItem(NodeType::OtherTerm, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get84()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get85()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::AddOp, Token()));
	item.push_back(StackItem(NodeType::Exp, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get86()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Factor, Token()));
	item.push_back(StackItem(NodeType::OtherFactor, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get87()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get88()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::MultOp, Token()));
	item.push_back(StackItem(NodeType::Term, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get89()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LPARENTHESIS, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RPARENTHESIS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get90()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::UINTEGER, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get91()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Variable, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get92()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::VariMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get93()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get94()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get95()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DOT, _T(""))));
	item.push_back(StackItem(NodeType::FieldVar, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get96()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::IDENTIFIER, _T(""))));
	item.push_back(StackItem(NodeType::FieldVarMore, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get97()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::EMPTY, Token()));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get98()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LSQUAREBRACKET, _T(""))));
	item.push_back(StackItem(NodeType::Exp, Token()));
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::RSQUAREBRACKET, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get99()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::LT, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get100()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::EQU, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get101()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::PLUS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get102()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::MINUS, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get103()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::MULTIPLY, _T(""))));
	return item;
}

vector<StackItem> LL1SyntaxParser::Get104()
{
	vector<StackItem> item;
	item.push_back(StackItem(NodeType::Terminal, Token(-1, LexType::DIVIDE, _T(""))));
	return item;
}
