// SemanticDlg.cpp: 实现文件
//

#include "pch.h"
#include "SNLCompiler.h"
#include "SemanticDlg.h"
#include "afxdialogex.h"


// SemanticDlg 对话框

IMPLEMENT_DYNAMIC(SemanticDlg, CDialogEx)

SemanticDlg::SemanticDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

SemanticDlg::~SemanticDlg()
{
}

void SemanticDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_SEMANTIC_EDIT, SemanticEdit);
}


BEGIN_MESSAGE_MAP(SemanticDlg, CDialogEx)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// SemanticDlg 消息处理程序


void SemanticDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	SemanticEdit.SetWindowTextW(mSemantic);
	// TODO: 在此处添加消息处理程序代码
}
