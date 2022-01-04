#pragma once
#include <vector>
#include <map>
//#include"RSyntaxParser.h"
#include "SyntaxParserCommon.h"

#include "LexicalAnalyzer.h"
typedef  enum { typeKind, varKind, procKind }IdKind;
/*变量的类别。dir表直接变量(值参)，indir表示间接变量(变参)*/
typedef  enum { dir, indir }AccessKind;
/*形参表的结构定义*/
typedef struct  paramTable
{
	struct symbtable   *  entry;/*指向该形参所在符号表中的地址入口*/
	struct paramTable  *  next;
}ParamTable;

struct typeIR;

typedef struct
{
	struct typeIR  * idtype; /*指向标识符的类型内部表示*/
	IdKind    kind; /*标识符的类型*/
	union
	{
		struct
		{
			AccessKind   access;   /*判断是变参还是值参*/
			int          level;
			int          off;
			bool         isParam;  /*判断是参数还是普通变量*/
		}VarAttr;/*变量标识符的属性*/
		struct
		{
			int         level;     /*该过程的层数*/
			ParamTable  * param;   /*参数表*/
			int         mOff;    /*过程活动记录的大小*/

			int         nOff;      /*sp到display表的偏移量*/
			int         procEntry; /*过程的入口地址*/
			int         codeEntry;/*过程入口标号,用于中间代码生成*/

		}ProcAttr;/*过程名标识符的属性*/
	}More;/*标识符的不同类型有不同的属性*/
}AttributeIR;

/*标识符的属性结构定义*/
typedef  enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;
//struct typeIR;
/*域类型单元结构定义*/
typedef struct fieldchain
{
	string   id;              /*变量名*/
	int    off;                 /*所在记录中的偏移*/
	struct typeIR  *  UnitType; /*域中成员的类型*/
	struct fieldchain  * Next;
}fieldChain;
/*类型的内部结构定义*/
typedef   struct  typeIR
{
	int size;   /*类型所占空间大小*/
	TypeKind kind;
	union
	{
		struct
		{
			struct typeIR * indexTy;
			struct typeIR * elemTy;
			int    low;     /*记录数组类型的下界*/
			int    up;      /*记录数组类型的上界*/
		}ArrayAttr;
		fieldChain * body;  /*记录类型中的域链*/
	} More;
}TypeIR;

/*符号表的结构定义*/
typedef struct  symbtable
{
	string  idName;
	AttributeIR  attrIR;
	struct symbtable  *  next;
}SymbTable;
struct TreeNode;
enum DecKind;
class RSyntaxParser;
class SemanticParser {
public:
	//SemanticParser();
	//~SemanticParser();
	int Error;//错误追踪标志,
	CString SMessage=_T("");
	RSyntaxParser *syntaxparser=nullptr;
	vector<ParseLog> mParseLog;
	/*scope栈的层数*/
	int Level;
	/*在同层的变量偏移*/
	int Off;
	/*主程序display表的偏移*/
	int StoreNoff;
	/*各层的displayOff*/
	int  savedOff;
	SymbTable * scope[1000];   /*scope栈*/
	TypeIR * intPtr; /*该指针一直指向整数类型的内部表示*/
	TypeIR * charPtr; /*该指针一直指向字符类型的内部表示*/
	TypeIR * boolPtr; /*该指针一直指向布尔类型的内部表示*/
	int Analyze(TreeNode * t);


	//////////
	//符号表实现
	////////////
	SymbTable * NewTable(void);
	void CreatTable(void);
	void DestroyTable(void);
	//登记标识符和属性
	int Enter(string id, AttributeIR * attribP, SymbTable ** entry);

	//寻找表项地址
	int FindEntry(string id, SymbTable ** entry);

	//属性查询
	AttributeIR FindAttr(SymbTable * entry);

	//是否相容
	int Compat(TypeIR * tp1, TypeIR * tp2);

	//创建空类型内部表示
	TypeIR * NewTy(TypeKind kind);

	//创建当前空记录类型中域的链表
	fieldChain * NewBody(void);

	//创建当前空形参链表
	ParamTable * NewParam(void);

	//错误提示
	void ErrorPrompt(int line, string name, string message);

	void printTab(int tabnum);
	
	//是否找到记录的域名
	bool  FindField(string Id, fieldChain  *head, fieldChain  **Entry);

	//记录类型域表
	void   PrintFieldChain(fieldChain  *currentP);

	//符号表的一层
	void  PrintOneLayer(int level);

	// 打印生成的符号表
	void   PrintSymbTable();

	
	///////////
	//分析函数
	//////////


	//初始化 int char bool
	void initialize(void);

	//类型分析
	TypeIR  * TypeProcess(TreeNode * t, DecKind deckind);

	//在符号表中寻找已定义的类型名字
	TypeIR * nameType(TreeNode * t);

	//处理数组类型的内部表示
	TypeIR  * arrayType(TreeNode * t);

	//处理记录类型的内部表示 
	TypeIR * recordType(TreeNode * t);
	
	//处理类型声明
	void TypeDecPart(TreeNode * t);
	
	//处理变量声明
	void VarDecPart(TreeNode * t);
	
	
	//void varDecList(TreeNode * t);
	
	//处理过程声明
	void  procDecPart(TreeNode * t);
	
	//处理函数头
	SymbTable  *  HeadProcess(TreeNode * t);
	
	//处理函数头参数声明
	ParamTable  * ParaDecList(TreeNode * t);
	
	//处理执行体
	void Body(TreeNode * t);
	
	//处理语句状态 
	void statement(TreeNode * t);
	
	//处理表达式
	TypeIR * Expr(TreeNode * t, AccessKind  *  Ekind);
	
	//处理数组变量的下标
	TypeIR * arrayVar(TreeNode * t);
	
	//处理记录变量
	TypeIR * recordVar(TreeNode * t);
	
	//处理赋值语句
	void assignstatement(TreeNode * t);
	
	//处理函数调用语句
	void callstatement(TreeNode * t);
	
	//处理条件语句
	void ifstatment(TreeNode * t);
	
	//处理循环语句
	void whilestatement(TreeNode * t);
	
	//处理输入语句
	void readstatement(TreeNode * t);
	
	//处理输出语句
	void writestatement(TreeNode * t);
	
	//处理函数返回语句
	void returnstatement(TreeNode * t);
};
