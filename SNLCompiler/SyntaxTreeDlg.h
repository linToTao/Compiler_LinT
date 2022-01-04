#pragma once


// SyntaxTreeDlg 对话框

class SyntaxTreeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SyntaxTreeDlg)

public:
	SyntaxTreeDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SyntaxTreeDlg();
	CString mSyntax;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYNTAXTREE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit mSyntaxTreeEdit;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void PostNcDestroy();
};
