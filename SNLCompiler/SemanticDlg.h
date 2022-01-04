#pragma once


// SemanticDlg 对话框

class SemanticDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SemanticDlg)

public:
	SemanticDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SemanticDlg();
	CString mSemantic;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	CEdit mSemanticEdit;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	CEdit SemanticEdit;
};
