// SyntaxTreeDlg.cpp: 实现文件
//

#include "pch.h"
#include "SNLCompiler.h"
#include "SyntaxTreeDlg.h"
#include "afxdialogex.h"


// SyntaxTreeDlg 对话框

IMPLEMENT_DYNAMIC(SyntaxTreeDlg, CDialogEx)

SyntaxTreeDlg::SyntaxTreeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYNTAXTREE_DIALOG, pParent)
{
	
}

SyntaxTreeDlg::~SyntaxTreeDlg()
{
}

void SyntaxTreeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SYNTAX_TREE_EDIT, mSyntaxTreeEdit);
}


BEGIN_MESSAGE_MAP(SyntaxTreeDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// SyntaxTreeDlg 消息处理程序

void SyntaxTreeDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	mSyntaxTreeEdit.SetWindowTextW(mSyntax);
	// TODO: 在此处添加消息处理程序代码
}


void SyntaxTreeDlg::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::PostNcDestroy();
	delete this;
}
