/**
	递归下降语法分析器
*/
#pragma once

#include <vector>
#include <map>
#include "SemanticParser.h"
#include "SyntaxParserCommon.h"
#include "LexicalAnalyzer.h"

using namespace std;
typedef enum { ProK, PheadK, DecK, TypeK, VarK, ProcDecK, StmLK, StmtK, ExpK }
NodeKind;
enum  DecKind { ArrayK, CharK, IntegerK, RecordK, IdK } ;
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK } StmtKind;
typedef enum { OpK, ConstK, VariK } ExpKind;
enum ExpType  { Void, Integer, Boolean } ;
union Kind {//记录语法树节点的具体类型，为共用体结构。

	DecKind dec; /*记录语法树节点的声明类型，当 nodekind = DecK 时有效，
				取 值 ArrayK, CharK, IntegerK, RecordK, IdK，为语法树节点声明类型。*/
	StmtKind stmt;/*记录语法树节点的语句类型，当 nodekind = StmtK 时有效，
					 取值 IfK,WhileK,AssignK,ReadK,WriteK,CallK,ReturnK，为语 法树节点语句类型。 */
	ExpKind  exp;/*记录语法树节点的表达式类型，当 nodekind=ExpK 时有效,
				取值 OpK,ConstK,VariK(书上是idk)，为语法树节点表达式类型*/
};
struct ArrayAttr// 记录数组类型的属性。 
{
	int  low;//整数类型变量，记录数组的下界。 
	int up;//整数类型变量，记录数组的上界。 
	DecKind childType;//记录数组的成员类型。
};
typedef enum {
	valparamtype, varparamtype
} Paramt;
struct ProAttr {
	Paramt paramt;
};
typedef enum {
	IdV, ArrayMembV, FieldMembV
} Varkind;
struct ExpAttr
{
	LexType op;
	//string op;
	CString val = _T("");
	//string val = "";
	Varkind varkind;
	ExpType type;         /* 用于类型检查  */
	//Type type;
};

struct Attr {//记录语法树节点其他属性,为结构体类型。 
	ArrayAttr*arrayattr = new ArrayAttr;
	ProAttr*proattr = new ProAttr;
	ExpAttr*expattr = new ExpAttr;
	CString type_name = _T("");//类型名
	
};

struct  symbtable;
struct TreeNode {
	TreeNode*child[3] = { NULL };// 指向子语法树节点指针，为语法树节点指针类型。 
	TreeNode*sibling = NULL;// 指向兄弟语法树节点指针，为语法树节点指针类型。 
	int  lineno = 0;//记录源程序行号，为整数类型。 
	NodeKind  nodekind;//记录语法树节点类型，
							  //取值 ProK, PheadK, TypeK, VarK, ProcDecK, StmLK, DecK, StmtK, ExpK,为语法树节点类型。 
	Kind *kind = new Kind();

	int idnum = 0;/*记录一个节点中的标志符的个数.*/
	CString name[100] = { _T("") };
	//string *name = new string[100];/*字符串数组，数组成员是节点中的标志符的名字。*/
	struct symbtable * table[100];//指针数组，数组成员是节点中的各个标志符在符号表中的入口。
	string type_name = "";//记录类型名，当节点为声明类型，且类型是由类型标志符表示时有效。
	Attr*attr = new Attr;
};




class RSyntaxParser
{
public:
	RSyntaxParser();
	RSyntaxParser(vector<Token> tokens);
	~RSyntaxParser();
	/* 静态变量indentno在函数printTree中  用于存储当前子树缩进格数,初始为0 */
	int indentno = 0;
	int Errorflag = 1;//1  right              0  false
	bool Error = true;
	//vector<RTreeNode*> mChildNodes;
	vector<Token> mTokenList;
	vector<ParseLog> mParseLog;
	//map<NodeType, CString> mNodeType2Str;
	CString temp_name = _T("");
	//string temp_name = "";
	int mTokenPtr;
	int mCurLine;
	LexicalAnalyzer mLexicalAnalyzer;

	TreeNode* mSyntaxTree;

	void NextToken();
	Token GetCurToken();
	bool Match(LexType type);

	//void InitMap();

	TreeNode* Parse();
	TreeNode* Program();
	TreeNode* ProgramHead();
	TreeNode* DeclarePart();
	TreeNode* ProcDec();
	TreeNode* ProcDeclaration();
	//TreeNode* ProcDecMore();
	TreeNode* ProcDecPart();
	TreeNode* ProcBody();
	void ParamList(TreeNode* t);
	TreeNode* ParamDecList();
	TreeNode* ParamMore();
	TreeNode* Param();
	void FormList(TreeNode* t);
	void FidMore(TreeNode* t);
	TreeNode* VarDec();
	TreeNode* VarDeclaration();
	TreeNode* VarDecList();
	void VarIdMore(TreeNode* t);
	void VarIdList(TreeNode* t);
	TreeNode* TypeDecMore();
	TreeNode* StmList();
	TreeNode* StmMore();
	TreeNode* Stm();
	TreeNode* AssCall();
	TreeNode* AssignmentRest();
	TreeNode* ConditionalStm();
	TreeNode* LoopStm();
	TreeNode* InputStm();
	TreeNode* OutputStm();
	TreeNode* ReturnStm();
	TreeNode* CallStmRest();
	TreeNode* ActParamList();
	TreeNode* ActParamMore();
	//TreeNode* RelExp();
	//TreeNode* OtherRelE();
	TreeNode* Exp();
	//TreeNode* OtherTerm();
	TreeNode* Term();
	TreeNode* Factor();
	//TreeNode* OtherFactor();
	TreeNode* Variable();
	void VariMore(TreeNode* t);
	TreeNode* FieldVar();
	void FieldVarMore(TreeNode* t);
	//TreeNode* CmpOp();
	//TreeNode* AddOp();
	//TreeNode* MultOp();
	TreeNode* ProgramBody();
	//TreeNode* ProgramName();
	TreeNode* VarDecMore();
	TreeNode* TypeDec();
	TreeNode* TypeDeclaration();
	TreeNode* TypeDecList();
	void TypeId(TreeNode* t);
	
	void TypeDef(TreeNode* t);
	void BaseType(TreeNode* t);
	void StructureType(TreeNode* t);
	void ArrayType(TreeNode* t);
	void RecType(TreeNode* t);
	TreeNode* FieldDecList();
	TreeNode* FieldDecMore();
	void IdList(TreeNode* t);
	void IdMore(TreeNode* t);
	

	//TreeNode* GetMatchedTerminal(Token t);

	CString GetSyntaxTreeStr(CString lftstr, CString append, TreeNode* r);
	TreeNode* newDecNode1(NodeKind kind);
	TreeNode* newDecNode2();
	TreeNode* newProcNode();
	TreeNode* newStmlNode();
	TreeNode* newStmtNode(StmtKind kind);
	TreeNode* newExpNode(ExpKind kind);
	TreeNode* Simple_exp();


	CString Treemessage = _T("");
	void printTree(TreeNode* t);
	void printTab(int tabnum);
	void printSpaces();
	void ReleaseTree(TreeNode* r);
};

