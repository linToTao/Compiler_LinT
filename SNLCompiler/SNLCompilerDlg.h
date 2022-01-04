
// SNLCompilerDlg.h: 头文件
//

#pragma once


// CSNLCompilerDlg 对话框
class CSNLCompilerDlg : public CDialogEx
{
// 构造
public:
	CSNLCompilerDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SNLCOMPILER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CEdit mSrcEdit;
	afx_msg void OnBnClickedTokenButton();
	CListCtrl mListControl;
	CListCtrl mSyntaxLogList;
	afx_msg void OnBnClickedSyntaxParseButton();
	CComboBox mCombo;
	afx_msg void OnBnClickedOpenFileButton();
	bool mLexErrorFlag;
	bool mSyntaxErrorFlag;
	afx_msg void OnEnChangeSrcEdit();
	
	afx_msg void OnBnClickedSemanticParseButton();
};
