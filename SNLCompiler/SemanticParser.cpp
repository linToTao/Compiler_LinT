#include "pch.h"
#include "SemanticParser.h"
#include "LexicalAnalyzer.h"
#include "RSyntaxParser.h"





int SemanticParser::Analyze(TreeNode * t)
{
	
	/*scope栈的层数*/
	Level = -1;
	intPtr = NULL; /*该指针一直指向整数类型的内部表示*/
	charPtr = NULL; /*该指针一直指向字符类型的内部表示*/
	boolPtr = NULL; /*该指针一直指向布尔类型的内部表示*/

	Error = false;
	
	SymbTable * entry = NULL;
	TreeNode * p = NULL;
	TreeNode * pp = t;
	/*创建符号表*/
	CreatTable();
	/*调用类型内部表示初始化函数*/
	initialize();
	/*语法树的声明节点*/
	p = t->child[1];
	while (p != NULL)
	{
		switch (p->nodekind)
		{
		case  TypeK:     TypeDecPart(p->child[0]);  break;
		case  VarK:     VarDecPart(p->child[0]);   break;
		case  ProcDecK:  procDecPart(p);        break;
		default:
			ErrorPrompt(p->lineno, "", "no this node kind in syntax tree!\r\n");
			break;
		}
		p = p->sibling;/*循环处理*/
	}
	/*程序体*/
	t = t->child[2];
	if (t->nodekind == StmLK)
		Body(t);
	/*撤销符号表*/
	if (Level != -1)
		DestroyTable();
	return StoreNoff;
}

SymbTable * SemanticParser::NewTable(void)
{
	
	SymbTable * table =new SymbTable;
	
	if (table == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));
		
		Error = TRUE;
	}
		
	
	table->next = NULL;
	table->attrIR.kind = typeKind;
	table->attrIR.idtype = NULL;
	table->next = NULL;
	table->attrIR.More.VarAttr.isParam = false;
	
	/* 符号表类型指针table不是NULL,内存单元已经成功分配 */
	return table;
}

void SemanticParser::CreatTable(void)
{
	Level = Level + 1;                 /*层数加一*/
	scope[Level] = NULL;           /*申请了新的一层scope栈的空间*/
	Off = 7;           /*偏移初始化  INITOFF */
}

void SemanticParser::DestroyTable(void)
{
	/*如果语义分析跟踪标志为TURE，则将语法分析产生的符号表显示出来*/
/*if ((TraceTable)&&(Error==FALSE))
{
printTable();
getchar();
}*/
	Level = Level - 1;
}

int SemanticParser::Enter(string id, AttributeIR * attribP, SymbTable ** entry)
{
	int present = FALSE;
	int result = 1;
	SymbTable * curentry = scope[Level];
	SymbTable * prentry = scope[Level];
	if (scope[Level] == NULL)
	{
		curentry = NewTable();
		scope[Level] = curentry;
	}
	else
	{
		while (curentry != NULL)
		{
			prentry = curentry;
			result = strcmp(id.c_str(), curentry->idName.c_str());
			if (result == 0)
			{
				SMessage += _T("repetition declaration error !\r\n");
				LogUtil::Error(_T("repetition declaration error !"));
				
				Error = TRUE;
				present = TRUE;
			}
			else
				curentry = (prentry->next);
		}   /*在该层符号表内检查是否有重复定义错误*/

		if (present == FALSE)
		{
			curentry = NewTable();
			prentry->next = curentry;
		}
	}
	/*将标识符名和属性登记到表中*/
	curentry->idName=id;
	curentry->attrIR.idtype = attribP->idtype;
	curentry->attrIR.kind = attribP->kind;
	switch (attribP->kind)
	{
	case  typeKind: break;
	case  varKind:
		curentry->attrIR.More.VarAttr.level = attribP->More.VarAttr.level;
		curentry->attrIR.More.VarAttr.off = attribP->More.VarAttr.off;
		curentry->attrIR.More.VarAttr.access = attribP->More.VarAttr.access;
		break;
	case  procKind:
		curentry->attrIR.More.ProcAttr.level = attribP->More.ProcAttr.level;
		curentry->attrIR.More.ProcAttr.param = attribP->More.ProcAttr.param;
		break;
	default:break;
	}
	(*entry) = curentry;
	return present;
}

int SemanticParser::FindEntry(string id, SymbTable ** entry)
{
	int present = FALSE;    /*返回值*/
	int result = 1;         /*标识符名字比较结果*/
	int lev = Level; /*临时记录层数的变量*/
	SymbTable *  findentry = scope[lev];
	while ((lev != -1) && (present != TRUE))
	{
		while ((findentry != NULL) && (present != TRUE))
		{
			result = strcmp(id.c_str(), findentry->idName.c_str());
			if (result == 0)
				present = TRUE;
			/*如果标识符名字相同，则返回TRUE*/
			else
				findentry = findentry->next;
			/*如果没找到，则继续链表中的查找*/
		}
		if (present != TRUE)
		{
			lev = lev - 1;
			findentry = scope[lev];
		}
	}/*如果在本层中没有查到，则转到上一个局部化区域中继续查找*/
	if (present != TRUE)
	{
		(*entry) = NULL;
	}
	else
		(*entry) = findentry;
	return present;
}

AttributeIR SemanticParser::FindAttr(SymbTable * entry)
{
	AttributeIR attrIr = entry->attrIR;
	return attrIr;
}

int SemanticParser::Compat(TypeIR * tp1, TypeIR * tp2)
{
	int  present;
	if (tp1 != tp2)
		present = FALSE;  /*类型不等*/
	else
		present = TRUE;   /*类型等价*/
	return present;
}

TypeIR * SemanticParser::NewTy(TypeKind kind)
{
	
	TypeIR * table = new TypeIR;
	/* 类型内部表示类型指针table为NULL,
		   未能成功分配内存单元将显示提示错误信息*/
	if (table == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));
		
		Error = TRUE;
	}
	/* 类型内部表示类型指针table不是NULL,内存单元已经成功分配 */
	else
		switch (kind)
		{
		case intTy:
		case charTy:
		case boolTy:
			table->kind = kind;
			table->size = 1;
			break;
		case arrayTy:
			table->kind = arrayTy;
			table->More.ArrayAttr.indexTy = NULL;
			table->More.ArrayAttr.elemTy = NULL;
			break;
		case recordTy:
			table->kind = recordTy;
			table->More.body = NULL;
			break;
		}
	return table;
}

fieldChain * SemanticParser::NewBody(void)
{
	fieldChain * Ptr = (fieldChain *)malloc(sizeof(fieldChain));
	if (Ptr == NULL)
	{
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));

		Error = TRUE;
	}
	else
	{
		Ptr->Next = NULL;
		Ptr->off = 0;
		Ptr->UnitType = NULL;
	}
	return Ptr;
}

ParamTable * SemanticParser::NewParam(void)
{
	ParamTable * Ptr = (ParamTable *)malloc(sizeof(ParamTable));
	if (Ptr == NULL)
	{
		
		SMessage += _T("Out of memory error !\r\n");
		LogUtil::Error(_T("Out of memory error !"));

		Error = TRUE;
	}
	else
	{
		Ptr->entry = NULL;
		Ptr->next = NULL;
	}
	return Ptr;
}

void SemanticParser::ErrorPrompt(int line, string name, string message)
{
	CString temp = _T("");

	temp.Format(_T("-->Line: %d"), line);//"-->Line: %d, %s %s"                      , name.c_str(), message.c_str()
	SMessage += temp;
	SMessage += _T(" ");
	temp = CA2T(name.c_str());
	SMessage += temp;
	SMessage += _T(" ");
	temp = CA2T(message.c_str());
	SMessage += temp;
	//LogUtil::Error(_T(">>>Line: %d, %s %s", line, name.c_str(), message.c_str()));
	
	Error = TRUE;
}

void SemanticParser::printTab(int tabnum)
{
	for (int i = 0; i < tabnum; i++)
		SMessage+=_T(" ");
}

bool SemanticParser::FindField(string Id, fieldChain * head, fieldChain ** Entry)
{
	bool  present = false;
	/*记录当前节点*/
	fieldChain *currentItem = head;
	/*从表头开始查找这个标识符，直到找到或到达表尾*/
	while ((currentItem != NULL) && (present == false))
	{
		if (strcmp(currentItem->id.c_str(), Id.c_str()) == 0)
		{
			present = true;
			if (Entry != NULL)
				(*Entry) = currentItem;
		}
		else  currentItem = currentItem->Next;
	}
	return(present);
}

void SemanticParser::PrintFieldChain(fieldChain * currentP)
{
	CString temp = _T("");
	                                  
	SMessage += _T("\r\n--------------Field  chain--------------------\r\n");
	fieldChain  *t = currentP;
	while (t != NULL)
	{ /*输出标识符名字*/

		//%s  t->id.c_str()
		temp = CA2T(t->id.c_str());
		SMessage += temp;
		SMessage+=_T(":  ");
		/*输出标识符的类型信息*/
		switch (t->UnitType->kind)
		{
		case  intTy:  SMessage += _T("intTy     ");   break;
		case  charTy: SMessage += _T("charTy    ");  break;
		case  arrayTy: SMessage += _T("arrayTy   "); break;
		case  recordTy:SMessage += _T("recordTy  "); break;
		default: SMessage += _T("error  type!  "); break;
		}
		temp.Format(_T("off = %d\r\n"), t->off);
		SMessage += temp;
		t = t->Next;
	}


}

void SemanticParser::PrintOneLayer(int level)
{
	SymbTable  *t = scope[level];


	CString temp = _T("");
	                             
	SMessage += _T("         符号表第");
	temp.Format(_T(" %d "), level+1);
	SMessage += temp;
	SMessage += _T("层:\r\n");
	                            
	
	SMessage += _T("---------------------------------------\r\n");
	//string str = "123123";

	while (t != NULL)
	{ /*输出标识符名字*/
		CString tmp;
		tmp = CA2T(t->idName.c_str());
		SMessage += tmp;
		SMessage += _T(":   ");
		
		AttributeIR  *Attrib = &(t->attrIR);
		/*输出标识符的类型信息，过程标识符除外*/
		if (Attrib->idtype != NULL)  /*过程标识符*/
			switch (Attrib->idtype->kind)
			{
			case  intTy:  SMessage += _T("intTy  ");   break;
			case  charTy: SMessage += _T("charTy  ");  break;
			case  arrayTy: SMessage += _T("arrayTy  "); break;
			case  recordTy:SMessage += _T("recordTy  "); break;
			default: SMessage += _T("error  type!  "); break;
			}
		/*输出标识符的类别，并根据不同类型输出不同其它属性*/
		switch (Attrib->kind)
		{
		case  typeKind:
			SMessage += _T("typekind  "); break;
		case  varKind:
			SMessage += _T("varkind  ");
			temp.Format(_T("Level = %d  "), Attrib->More.VarAttr.level);
			SMessage += temp;
			temp.Format(_T("Offset = %d  "), Attrib->More.VarAttr.off);
			SMessage += temp;

			switch (Attrib->More.VarAttr.access)
			{
			case  dir:  SMessage += _T("dir  "); break;
			case  indir: SMessage += _T("indir  "); break;
			default:SMessage += _T("errorkind  ");  break;
			}
			break;
		case  procKind:
			SMessage += _T("funckind   ");
			temp.Format(_T("Level= %d  "), Attrib->More.ProcAttr.level);
			SMessage += temp;
			temp.Format(_T("Noff= %d  "), Attrib->More.ProcAttr.nOff);
			SMessage += temp;
			break;
		default:SMessage += _T("error  ");
		}
		SMessage += _T("\r\n");
		t = t->next;
	}
	SMessage += _T("---------------------------------------\r\n\r\n");
}

void SemanticParser::PrintSymbTable()
{
	int  level = 0;
	while (scope[level] != NULL)
	{
		PrintOneLayer(level);
		level++;
	}
}

void SemanticParser::initialize(void)
{
	intPtr = NewTy(intTy);
	charPtr = NewTy(charTy);
	boolPtr = NewTy(boolTy);
	/*scope栈的各层指针设为空*/
	for (int i = 0; i < 1000; i++)
		scope[i] = NULL;
}

TypeIR * SemanticParser::TypeProcess(TreeNode * t, DecKind deckind)
{
	TypeIR  *  Ptr = NULL;
	switch (deckind)
	{
	case IdK:
		Ptr = nameType(t); break;         /*类型为自定义标识符*/
	case ArrayK:
		Ptr = arrayType(t); break;        /*类型为数组类型*/
	case RecordK:
		Ptr = recordType(t); break;       /*类型为记录类型*/
	case IntegerK:
		Ptr = intPtr; break;              /*类型为整数类型*/
	case CharK:
		Ptr = charPtr; break;             /*类型为字符类型*/
	}
	return Ptr;
}

TypeIR * SemanticParser::nameType(TreeNode * t)
{
	TypeIR * Ptr = NULL;
	SymbTable * entry = NULL;
	int present;
	/*类型标识符也需要往前层查找*/
	string id = CT2A(t->attr->type_name.GetString());
	present = FindEntry(id, &entry);
	if (present == TRUE)
	{
		/*检查该标识符是否为类型标识符*/
		if (entry->attrIR.kind != typeKind) {
			string name = CT2A(t->attr->type_name.GetString());
			ErrorPrompt(t->lineno, name, "used before typed!\r\n");
		}
		else
			Ptr = entry->attrIR.idtype;
	}
	else/*没有找到该标识符*/
	{
		string name = CT2A(t->attr->type_name);
		ErrorPrompt(t->lineno, name, "type name is not declared!\r\n");
	}
	return Ptr;
}

TypeIR * SemanticParser::arrayType(TreeNode * t)
{
	TypeIR  * Ptr0 = NULL;
	TypeIR  * Ptr1 = NULL;
	TypeIR  * Ptr = NULL;
	/*检查数组上界是否小于下界*/
	if ((t->attr->arrayattr->low) > (t->attr->arrayattr->up))
	{
		ErrorPrompt(t->lineno, "", "array subscript error!\r\n");
		Error = TRUE;
	}
	else
	{
		Ptr0 = TypeProcess(t, IntegerK);
		/*调用类型分析函数，处理下标类型*/
		Ptr1 = TypeProcess(t, t->attr->arrayattr->childType);
		/*调用类型分析函数，处理元素类型*/
		Ptr = NewTy(arrayTy);
		/*指向一新创建的类型信息表*/
		Ptr->size = ((t->attr->arrayattr->up) - (t->attr->arrayattr->low) + 1)*(Ptr1->size);
		/*计算本类型长度*/
		/*填写其他信息*/
		Ptr->More.ArrayAttr.indexTy = Ptr0;
		Ptr->More.ArrayAttr.elemTy = Ptr1;
		Ptr->More.ArrayAttr.low = t->attr->arrayattr->low;
		Ptr->More.ArrayAttr.up = t->attr->arrayattr->up;
	}
	return Ptr;
}

TypeIR * SemanticParser::recordType(TreeNode * t)
{
	TypeIR  * Ptr = NewTy(recordTy);  /*新建记录类型的节点*/
	t = t->child[0];                /*从语法数的儿子节点读取域信息*/
	fieldChain  *  Ptr2 = NULL;
	fieldChain  *  Ptr1 = NULL;
	fieldChain  *  body = NULL;
	while (t != NULL) 
	{
		/*填写ptr2指向的内容节点
		 解决int a,b;类型 */
		for (int i = 0; i < t->idnum; i++)
		{
			
			Ptr2 = NewBody();
			if (body == NULL)
				body = Ptr1 = Ptr2;
			/*填写Ptr2的各个成员内容*/
			Ptr2->id= CT2A(t->name[i].GetString());
			Ptr2->UnitType = TypeProcess(t, t->kind->dec);
			Ptr2->Next = NULL;

			
			if (Ptr2 != Ptr1)/*那么将指针后移*/
			{
				/*计算新申请的单元off*/
				Ptr2->off = (Ptr1->off) + (Ptr1->UnitType->size);
				Ptr1->Next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		//处理兄弟节点
		t = t->sibling;
	}
	/*处理记录类型内部结构*/
	/*取Ptr2的off为最后整个记录的size*/
	Ptr->size = Ptr2->off + (Ptr2->UnitType->size);
	/*将域链链入记录类型的body部分*/
	Ptr->More.body = body;
	return Ptr;
}

void SemanticParser::TypeDecPart(TreeNode * t)
{
	int present = FALSE;
	AttributeIR  attrIr;
	SymbTable * entry = NULL;
	/*添属性作为参数*/
	attrIr.kind = typeKind;
	/*遍历语法树的兄弟节点*/
	while (t != NULL)
	{
		/*调用记录属性函数，返回是否重复声明错和入口地址*/
		string id = CT2A(t->name[0].GetString());
		present = Enter(id, &attrIr, &entry);
		if (present != FALSE)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is repetation declared!\r\n");
			entry = NULL;
		}
		else
			entry->attrIR.idtype = TypeProcess(t, t->kind->dec);
		t = t->sibling;
	}
}

void SemanticParser::VarDecPart(TreeNode * t)
{
	//varDecList(t);
	AttributeIR  attrIr;
	int present = FALSE;
	SymbTable * entry = NULL;
	while (t != NULL) /*循环过程*/
	{
		attrIr.kind = varKind;
		for (int i = 0; i < (t->idnum); i++)
		{
			attrIr.idtype = TypeProcess(t, t->kind->dec);
			/*判断识值参还是变参acess(dir,indir)*/
			if (t->attr->proattr->paramt == varparamtype)
			{
				attrIr.More.VarAttr.access = indir;
				attrIr.More.VarAttr.level = Level;
				/*计算形参的偏移*/
				attrIr.More.VarAttr.off = Off;
				Off = Off + 1;
			}/*如果是变参，则偏移加1*/
			else
			{
				attrIr.More.VarAttr.access = dir;
				attrIr.More.VarAttr.level = Level;
				/*计算值参的偏移*/
				if (attrIr.idtype != NULL)
				{
					attrIr.More.VarAttr.off = Off;
					Off = Off + (attrIr.idtype->size);
				}
			}/*其他情况均为值参，偏移加变量类型的size*/
			/*登记该变量的属性及名字,并返回其类型内部指针*/
			string id = CT2A(t->name[i].GetString());
			present = Enter(id, &attrIr, &entry);
			if (present != FALSE)
			{
				id = CT2A(t->name[i].GetString());
				ErrorPrompt(t->lineno, id, " is defined repetation!\r\n");
			}
			else
				t->table[i] = entry;
		}
		if (t != NULL)
			t = t->sibling;
	}
	/*如果是主程序，则记录此时偏移，用于目标代码生成时的displayOff*/
	if (Level == 0)
	{
		//mainOff = Off;
		/*存储主程序AR的display表的偏移到全局变量*/
		StoreNoff = Off;
	}
	/*如果不是主程序，则记录此时偏移，用于下面填写过程信息表的noff信息*/
	else
		savedOff = Off;
}



void SemanticParser::procDecPart(TreeNode * t)
{
	TreeNode * p = t;
	SymbTable * entry = HeadProcess(t);   /*处理过程头*/
	t = t->child[1];
	/*如果过程内部存在声明部分，则处理声明部分*/
	while (t != NULL)
	{
		switch (t->nodekind)
		{
		case  TypeK:     
			TypeDecPart(t->child[0]);  break;
		case  VarK:     
			VarDecPart(t->child[0]);   break;
			/*如果声明部分有函数声明，则跳出循环，先填写noff和moff等信息，*
			 *再处理函数声明的循环处理，否则无法保存noff和moff的值。      */
		case  ProcDecK:  break;
		default:
			ErrorPrompt(t->lineno, "", "no this node kind in syntax tree!");
			break;
		}
		if (t->nodekind == ProcDecK)
			break;
		else
			t = t->sibling;
	}
	entry->attrIR.More.ProcAttr.nOff = savedOff;
	entry->attrIR.More.ProcAttr.mOff = entry->attrIR.More.ProcAttr.nOff + entry->attrIR.More.ProcAttr.level + 1;
	/*过程活动记录的长度等于nOff加上display表的长度*
	 *diplay表的长度等于过程所在层数加一           */
	 /*处理程序的声明部分*/
	while (t != NULL)
	{
		procDecPart(t);
		t = t->sibling;
	}
	t = p;
	Body(t->child[2]);/*处理Block*/
	/*函数部分结束，删除进入形参时，新建立的符号表*/
	if (Level != -1)
		DestroyTable();/*结束当前scope*/
}

SymbTable * SemanticParser::HeadProcess(TreeNode * t)
{
	AttributeIR attrIr;
	int present = FALSE;
	SymbTable  * entry = NULL;
	/*填属性*/
	attrIr.kind = procKind;
	attrIr.idtype = NULL;
	attrIr.More.ProcAttr.level = Level + 1;
	if (t != NULL)
	{
		/*登记函数的符号表项*/
		string id = CT2A(t->name[0].GetString());
		present = Enter(id, &attrIr, &entry);
		t->table[0] = entry;
		/*处理形参声明表*/
	}
	entry->attrIR.More.ProcAttr.param = ParaDecList(t);
	return entry;
}

ParamTable * SemanticParser::ParaDecList(TreeNode * t)
{
	TreeNode * p = NULL;
	ParamTable  * Ptr1 = NULL;
	ParamTable  * Ptr2 = NULL;
	ParamTable  * head = NULL;
	if (t != NULL)
	{
		if (t->child[0] != NULL)
			p = t->child[0];    /*程序声明节点的第一个儿子节点*/
		CreatTable(); /*进入新的局部化区*/
		Off = 7;                /*子程序中的变量初始偏移设为8*/
		VarDecPart(p); /*变量声明部分*/
		SymbTable * Ptr0 = scope[Level];

		while (Ptr0 != NULL)         /*只要不为空，就访问其兄弟节点*/
		{
			/*构造形参符号表，并使其连接至符号表的param项*/
			Ptr2 = NewParam();
			if (head == NULL)
				head = Ptr1 = Ptr2;
			//Ptr0->attrIR.More.VarAttr.isParam = true;
			Ptr2->entry = Ptr0;
			Ptr2->next = NULL;
			if (Ptr2 != Ptr1)
			{
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
			Ptr0 = Ptr0->next;
		}
	}
	return head;   /*返回形参符号表的头指针*/
}

void SemanticParser::Body(TreeNode * t)
{
	if (t->nodekind == StmLK)
	{
		TreeNode * p = t->child[0];
		while (p != NULL)
		{
			statement(p);  /*调用语句状态处理函数*/
			p = p->sibling;   /*依次读入语法树语句序列的兄弟节点*/
		}
	}
}

void SemanticParser::statement(TreeNode * t)
{
	switch (t->kind->stmt)
	{
	case IfK: ifstatment(t); break;
	case WhileK: whilestatement(t); break;
	case AssignK: assignstatement(t); break;
	case ReadK:     readstatement(t); break;
	case WriteK: writestatement(t); break;
	case CallK:     callstatement(t); break;
	case ReturnK: returnstatement(t); break;
	default:
		ErrorPrompt(t->lineno, "", "statement type error!\r\n");
		break;
	}
}

TypeIR * SemanticParser::Expr(TreeNode * t, AccessKind * Ekind)
{
	int present = FALSE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	if (t != NULL)
		switch (t->kind->exp)
		{
		case ConstK:
			Eptr = TypeProcess(t, IntegerK);
			Eptr->kind = intTy;
			if (Ekind != NULL)
				(*Ekind) = dir;   /*直接变量*/
			break;
		case VariK:
			/*Var = id的情形*/
			if (t->child[0] == NULL)
			{
				/*在符号表中查找此标识符*/
				string id = CT2A(t->name[0].GetString());
				present = FindEntry(id, &entry);
				t->table[0] = entry;
				if (present != FALSE)
				{   /*id不是变量*/
					if (FindAttr(entry).kind != varKind)
					{
						id = CT2A(t->name[0]);
						ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
						Eptr = NULL;
					}
					else
					{
						Eptr = entry->attrIR.idtype;
						if (Ekind != NULL)
							(*Ekind) = indir;  /*间接变量*/
					}
				}
				else /*标识符无声明*/
				{
					id = CT2A(t->name[0]);
					ErrorPrompt(t->lineno, id, "is not declarations!\r\n");
				}
			}
			else/*Var = Var0[E]的情形*/
			{
				if (t->attr->expattr->varkind == ArrayMembV)
					Eptr = arrayVar(t);
				else /*Var = Var0.id的情形*/
					if (t->attr->expattr->varkind == FieldMembV)
						Eptr = recordVar(t);
			}
			break;
		case OpK:
			/*递归调用儿子节点*/
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL)
				return NULL;
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL)
				return NULL;
			/*类型判别*/
			present = Compat(Eptr0, Eptr1);
			if (present != FALSE)
			{
				switch (t->attr->expattr->op)
				{
				case LT:
				case EQU:
					Eptr = boolPtr;
					break;  /*条件表达式*/
				case PLUS:
				case MINUS:
				case LexType::MULTIPLY:
				case LexType::DIVIDE:
					Eptr = intPtr;
					break;  /*算数表达式*/
				}
				if (Ekind != NULL)
					(*Ekind) = dir; /*直接变量*/
			}
			else
				ErrorPrompt(t->lineno, "", "operator is not compat!\r\n");
			break;
		}
	return Eptr;
}

TypeIR * SemanticParser::arrayVar(TreeNode * t)
{
	int present = FALSE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	/*在符号表中查找此标识符*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*找到*/
	if (present != FALSE)
	{
		/*Var0不是变量*/
		if (FindAttr(entry).kind != varKind)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
			Eptr = NULL;
		}
		else/*Var0不是数组类型变量*/
			if (FindAttr(entry).idtype != NULL)
				if (FindAttr(entry).idtype->kind != arrayTy)
				{
					id = CT2A(t->name[0].GetString());
					ErrorPrompt(t->lineno, id, "is not array variable error !\r\n");
					Eptr = NULL;
				}
				else
				{
					/*检查E的类型是否与下标类型相符*/
					Eptr0 = entry->attrIR.idtype->More.ArrayAttr.indexTy;
					if (Eptr0 == NULL)
						return NULL;
					Eptr1 = Expr(t->child[0], NULL);//intPtr;
					if (Eptr1 == NULL)
						return NULL;
					present = Compat(Eptr0, Eptr1);
					if (present != TRUE)
					{
						ErrorPrompt(t->lineno, "", "type is not matched with the array member error !\r\n");
						Eptr = NULL;
					}
					else
						Eptr = entry->attrIR.idtype->More.ArrayAttr.elemTy;
				}
	}
	else {/*标识符无声明*/
		id = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, id, "is not declarations!\r\n");
	}
	return Eptr;
}

TypeIR * SemanticParser::recordVar(TreeNode * t)
{
	int present = FALSE;
	int result = TRUE;
	SymbTable * entry = NULL;
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	fieldchain * currentP = NULL;
	/*在符号表中查找此标识符*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*找到*/
	if (present != FALSE)
	{
		/*Var0不是变量*/
		if (FindAttr(entry).kind != varKind)
		{
			id = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno, id, "is not variable error!\r\n");
			Eptr = NULL;
		}
		else/*Var0不是记录类型变量*/
			if (FindAttr(entry).idtype->kind != recordTy)
			{
				id = CT2A(t->name[0].GetString());
				ErrorPrompt(t->lineno, id, "is not record variable error !\r\n");
				Eptr = NULL;
			}
			else/*检查id是否是合法域名*/
			{
				Eptr0 = entry->attrIR.idtype;
				currentP = Eptr0->More.body;
				while ((currentP != NULL) && (result != FALSE))
				{
					string temp = CT2A(t->name[0].GetString());
					result = strcmp(temp.c_str(), currentP->id.c_str());
					/*如果相等*/
					if (result == FALSE)
						Eptr = currentP->UnitType;
					else
						currentP = currentP->Next;
				}
				if (currentP == NULL)
					if (result != FALSE)
					{
						string temp = CT2A(t->child[0]->name[0].GetString());
						ErrorPrompt(t->child[0]->lineno, temp,"is not field type!\r\n");
						Eptr = NULL;
					}
					else/*如果id是数组变量*/
						if (t->child[0]->child[0] != NULL)
							Eptr = arrayVar(t->child[0]);
			}
	}
	else {/*标识符无声明*/
		string name = CT2A(t->name[0]);
		ErrorPrompt(t->lineno, name, "is not declarations!\r\n");
	}
	return Eptr;
}

void SemanticParser::assignstatement(TreeNode * t)
{
	SymbTable * entry = NULL;
	int present = FALSE;
	TypeIR * ptr = NULL;
	TypeIR * Eptr = NULL;
	TreeNode * child1 = NULL;
	TreeNode * child2 = NULL;
	child1 = t->child[0];
	child2 = t->child[1];
	if (child1->child[0] == NULL)
	{
		/*在符号表中查找此标识符*/
		string id = CT2A(child1->name[0].GetString());
		present = FindEntry(id, &entry);
		if (present != FALSE)
		{   /*id不是变量*/
			if (FindAttr(entry).kind != varKind)
			{
				string name = CT2A(child1->name[0].GetString());
				ErrorPrompt(child1->lineno, name, "is not variable error!\r\n");
				Eptr = NULL;
			}
			else
			{
				Eptr = entry->attrIR.idtype;
				child1->table[0] = entry;
			}
		}
		else {/*标识符无声明*/

			string name = CT2A(child1->name[0].GetString());
			ErrorPrompt(child1->lineno, name, "is not declarations!\r\n");
		}
	}
	else/*Var0[E]的情形*/
	{
		if (child1->attr->expattr->varkind == ArrayMembV)
			Eptr = arrayVar(child1);
		else /*Var0.id的情形*/
			if (child1->attr->expattr->varkind == FieldMembV)
				Eptr = recordVar(child1);
	}
	if (Eptr != NULL)
	{
		if ((t->nodekind == StmtK) && (t->kind->stmt == AssignK))
		{
			/*检查是不是赋值号两侧 类型等价*/
			ptr = Expr(child2, NULL);
			if (!Compat(ptr, Eptr))
				ErrorPrompt(t->lineno, "", "ass_expression error!\r\n");
		}
		/*赋值语句中不能出现函数调用*/
	}
}

void SemanticParser::callstatement(TreeNode * t)
{
	AccessKind  Ekind;
	int present = FALSE;
	SymbTable * entry = NULL;
	TreeNode * p = NULL;
	/*用id检查整个符号表*/
	string id = CT2A(t->child[0]->name[0].GetString());
	present = FindEntry(id, &entry);
	t->child[0]->table[0] = entry;
	/*未查到表示函数无声明*/
	if (present == FALSE)
	{
		string name = CT2A(t->child[0]->name[0].GetString());
		ErrorPrompt(t->lineno, name, "function is not declarationed!\r\n");
	}
	else
		/*id不是函数名*/
		if (FindAttr(entry).kind != procKind) {
			string name = CT2A(t->name[0].GetString());
			ErrorPrompt(t->lineno,name, "is not function name!\r\n");
		}
		else/*形实参匹配*/
		{
			p = t->child[1];
			/*paramP指向形参符号表的表头*/
			ParamTable * paramP = FindAttr(entry).More.ProcAttr.param;
			while ((p != NULL) && (paramP != NULL))
			{
				SymbTable  * paraEntry = paramP->entry;
				TypeIR  * Etp = Expr(p, &Ekind);/*实参*/
				/*参数类别不匹配*/
				if ((FindAttr(paraEntry).More.VarAttr.access == indir) && (Ekind == dir))
					ErrorPrompt(p->lineno, "", "param kind is not match!\r\n");
				else
					/*参数类型不匹配*/
					if ((FindAttr(paraEntry).idtype) != Etp)
						ErrorPrompt(p->lineno, "", "param type is not match!\r\n");
				p = p->sibling;
				paramP = paramP->next;
			}
			/*参数个数不匹配*/
			if ((p != NULL) || (paramP != NULL))
				ErrorPrompt(t->child[1]->lineno, "", "param num is not match!\r\n");
		}
}

void SemanticParser::ifstatment(TreeNode * t)
{
	AccessKind * Ekind = NULL;
	TypeIR *  Etp = Expr(t->child[0], Ekind);
	if (Etp != NULL)
		/*处理条件表达式*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, "", "condition expressrion error!\r\n");  /*逻辑表达式错误*/
		else
		{
			TreeNode * p = t->child[1];
			/*处理then语句序列部分*/
			while (p != NULL)
			{
				statement(p);
				p = p->sibling;
			}
			t = t->child[2]; /*必有三儿子*/
			/*处理else语句不分*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}

void SemanticParser::whilestatement(TreeNode * t)
{
	TypeIR *  Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*处理条件表达式部分*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, "", "condition expression error!\r\n");  /*逻辑表达式错误*/
		else
		{
			t = t->child[1];
			/*处理循环部分*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}

void SemanticParser::readstatement(TreeNode * t)
{
	SymbTable * entry = NULL;
	int present = FALSE;
	/*用id检查整个符号表*/
	string id = CT2A(t->name[0].GetString());
	present = FindEntry(id, &entry);
	t->table[0] = entry;
	/*未查到表示变量无声明*/
	if (present == FALSE) {
		string name = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, name, " is not declarationed!\r\n");
	}
	else if (entry->attrIR.kind != varKind) {/*不是变量标识符错误*/
		string name = CT2A(t->name[0].GetString());
		ErrorPrompt(t->lineno, name, " is not var name!\r\n ");
	}
}

void SemanticParser::writestatement(TreeNode * t)
{
	TypeIR * Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*如果表达式类型为bool类型，报错*/
		if (Etp->kind == boolTy)
			ErrorPrompt(t->lineno, "", "exprssion type error!\r\n");
}

void SemanticParser::returnstatement(TreeNode * t)
{
	if (Level == 0)/*如果返回语句出现在主程序中，报错*/
		ErrorPrompt(t->lineno, "", "return statement error!\r\n");
}



