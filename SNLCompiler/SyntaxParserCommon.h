#pragma once

enum LogType
{
	LERROR,
	LINFO,
	LDEBUG
};

// 语句块类型
enum NodeType
{
	Program,		// Program 标志块，是整个程序块，相当于开始符
	ProgramHead,		// Program Head 标志块，例如 program bubble
	ProgramName,
	ProgramBody,
	StmList,
	Stm,
	StmMore,
	DeclarePart,
	TypeDec,		// Type Declare 标志块，类型声明
	//EMPTY,
	TypeDecList,
	TypeId,
	TypeDef,
	TypeDecMore,
	TypeDeclaration,
	BaseType,
	StructureType,
	ArrayType,
	RecType,
	FieldDecList,
	IdList,
	FieldDecMore,
	IdMore,
	VarDec,		// Var Declare 标志块，变量声明
	VarDeclaration,
	VarDecList,
	VarIdList,
	VarIdMore,
	VarDecMore,
	ProcDec,
	ProcDeclaration,
	ProcDecMore,
	ParamList,
	ParamDecList,
	Param,
	ParamMore,
	FormList,
	FidMore,
	ProcDecPart,
	ProcBody,
	AssCall,
	AssignmentRest,
	ConditionalStm,
	LoopStm,
	InputStm,
	OutputStm,
	ReturnStm,
	CallStmRest,
	ActParamList,
	ActParamMore,
	Exp,
	OtherTerm,
	Term,
	OtherFactor,
	Factor,
	Variable,
	VariMore,
	FieldVar,
	FieldVarMore,
	RelExp,
	OtherRelE,
	CmpOp,
	AddOp,
	MultOp,
	Terminal	// 终极符
};

struct ParseLog
{
	ParseLog(int li, LogType t, CString lg) : line(li), type(t), log(lg) {}
	int line;
	LogType type;
	CString log;
};
