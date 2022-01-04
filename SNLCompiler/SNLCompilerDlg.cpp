
// SNLCompilerDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "SNLCompiler.h"
#include "SNLCompilerDlg.h"
#include "afxdialogex.h"
#include "LexicalAnalyzer.h"
#include "RSyntaxParser.h"
#include "SemanticParser.h"
#include "LL1SyntaxParser.h"
#include "SyntaxTreeDlg.h"
#include "SemanticDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSNLCompilerDlg 对话框



CSNLCompilerDlg::CSNLCompilerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SNLCOMPILER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSNLCompilerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SRC_EDIT, mSrcEdit);
	DDX_Control(pDX, IDC_TOKEN_LIST, mListControl);
	DDX_Control(pDX, IDC_SYNTAXLOG_LIST, mSyntaxLogList);
	DDX_Control(pDX, IDC_COMBO1, mCombo);
	//DDX_Control(pDX, IDC_SEMANTICLOG_LIST, mSemanticLogList);
}

BEGIN_MESSAGE_MAP(CSNLCompilerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TOKEN_BUTTON, &CSNLCompilerDlg::OnBnClickedTokenButton)
	ON_BN_CLICKED(IDC_SYNTAX_PARSE_BUTTON, &CSNLCompilerDlg::OnBnClickedSyntaxParseButton)
	ON_BN_CLICKED(IDC_OPEN_FILE_BUTTON, &CSNLCompilerDlg::OnBnClickedOpenFileButton)
	ON_EN_CHANGE(IDC_SRC_EDIT, &CSNLCompilerDlg::OnEnChangeSrcEdit)
	ON_BN_CLICKED(IDC_SEMANTIC_PARSE_BUTTON, &CSNLCompilerDlg::OnBnClickedSemanticParseButton)
END_MESSAGE_MAP()


// CSNLCompilerDlg 消息处理程序

BOOL CSNLCompilerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	mListControl.InsertColumn(0, _T("LINE"));
	mListControl.InsertColumn(1, _T("LEX"));
	mListControl.InsertColumn(2, _T("SEM"));
	mListControl.SetColumnWidth(0, 40);
	mListControl.SetColumnWidth(1, 140);
	mListControl.SetColumnWidth(2, 140);

	// 设置整行选中
	mListControl.SetExtendedStyle(mListControl.GetExtendedStyle() | LVS_EX_FULLROWSELECT);	

	mSyntaxLogList.InsertColumn(0, _T("LINE"));
	mSyntaxLogList.InsertColumn(1, _T("TYPE"));
	mSyntaxLogList.InsertColumn(2, _T("LOG"));
	mSyntaxLogList.SetColumnWidth(0, 40);
	mSyntaxLogList.SetColumnWidth(1, 50);
	mSyntaxLogList.SetColumnWidth(2, 280);

	// 设置整行选中
	mSyntaxLogList.SetExtendedStyle(mSyntaxLogList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	mCombo.SetCurSel(0);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CSNLCompilerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSNLCompilerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CSNLCompilerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CSNLCompilerDlg::OnBnClickedTokenButton()
{
	LexicalAnalyzer la;
	mSrcEdit.GetWindowTextW(la.mOrignalSrcCode);
	//la.mOrignalSrcCode += "\0\0";
	la.getTokenList();
	//la.Lex2File();
	mListControl.DeleteAllItems();
	mLexErrorFlag = true;
	for (int i = la.mTokenList.size() - 1; i >= 0; i--)
	{
		CString str = _T("");
		Token t = la.mTokenList[i];
		int idx = mListControl.InsertItem(0, Utils::Int2Cstr(t.line));
		if (t.lex == LexType::LEXERR)
		{
			mListControl.SetItemText(idx, 1, _T("[ERROR] Unexpected symbol"));
			mListControl.SetItemText(idx, 2, t.sem);
			mLexErrorFlag = false;
			continue;
		}
		mListControl.SetItemText(idx, 1, la.mLex2String[t.lex]);
		if (!la.isReservedWord(t.sem) && !la.isSingleDelimiter(t.sem) && t.sem != ":=" && t.sem != "." && t.sem != "..")
			mListControl.SetItemText(idx, 2, t.sem);
	}
}


void CSNLCompilerDlg::OnBnClickedSyntaxParseButton()
{
	// TODO: 在此添加控件通知处理程序代码
	LexicalAnalyzer la;
	mSrcEdit.GetWindowTextW(la.mOrignalSrcCode);
	//la.mOrignalSrcCode += "\0\0";
	la.getTokenList();
	//la.Lex2File();
	
	mSyntaxLogList.DeleteAllItems();
	if (mLexErrorFlag) {

		CString selected;

		mCombo.GetLBText(mCombo.GetCurSel(), selected);

		if (selected == _T("LL1"))
		{
			LL1SyntaxParser parser(la.mTokenList);
			parser.Parse();
			for (int i = parser.mParseLog.size() - 1; i >= 0; i--)
			{
				CString str = _T("");
				ParseLog log = parser.mParseLog[i];
				int idx = mSyntaxLogList.InsertItem(0, Utils::Int2Cstr(log.line));
				switch (log.type)
				{
				case LogType::LERROR:
					mSyntaxLogList.SetItemText(idx, 1, _T("ERROR"));
					break;
				case LogType::LDEBUG:
					mSyntaxLogList.SetItemText(idx, 1, _T("DEBUG"));
					break;
				case LogType::LINFO:
					mSyntaxLogList.SetItemText(idx, 1, _T("INFO"));
					break;
				default:
					break;
				}

				mSyntaxLogList.SetItemText(idx, 2, log.log);
			}
			CString s = parser.GetSyntaxTreeStr(_T(" "), _T(""), parser.mSyntaxTree);
			//mSyntaxTreeEdit.SetWindowTextW(s);
			SyntaxTreeDlg* dlg = new SyntaxTreeDlg;
			dlg->mSyntax = s;
			dlg->Create(IDD_SYNTAXTREE_DIALOG, this->GetDesktopWindow());
			dlg->ShowWindow(SW_SHOW);
		}
		else
		{
			RSyntaxParser parser(la.mTokenList);
			parser.Parse();
			for (int i = parser.mParseLog.size() - 1; i >= 0; i--)
			{
				CString str = _T("");
				ParseLog log = parser.mParseLog[i];
				int idx = mSyntaxLogList.InsertItem(0, Utils::Int2Cstr(log.line));
				switch (log.type)
				{
				case LogType::LERROR:
					mSyntaxLogList.SetItemText(idx, 1, _T("ERROR"));
					break;
				case LogType::LDEBUG:
					mSyntaxLogList.SetItemText(idx, 1, _T("DEBUG"));
					break;
				case LogType::LINFO:
					mSyntaxLogList.SetItemText(idx, 1, _T("INFO"));
					break;
				default:
					break;
				}

				mSyntaxLogList.SetItemText(idx, 2, log.log);
			}
			if (parser.Errorflag == 1) {
				parser.printTree(parser.mSyntaxTree);
				CString s = parser.Treemessage;
				//CString s = parser.GetSyntaxTreeStr(_T(" "), _T(""), parser.mSyntaxTree);
				//mSyntaxTreeEdit.SetWindowTextW(s);
				SyntaxTreeDlg* dlg = new SyntaxTreeDlg;
				dlg->mSyntax = s;
				dlg->Create(IDD_SYNTAXTREE_DIALOG, this->GetDesktopWindow());
				dlg->ShowWindow(SW_SHOW);
			}
			else {
				MessageBox(_T("Error!!!  you can check the SyntaxList to find the line of Error position"));
			}
			
		}
	}
	else
		MessageBox(_T("Please run LexicalAnalyzer first or correct the error!"));
}

void CSNLCompilerDlg::OnBnClickedSemanticParseButton()
{
	// TODO: 在此添加控件通知处理程序代码
	SemanticParser SParser;
	LexicalAnalyzer la;
	mSrcEdit.GetWindowTextW(la.mOrignalSrcCode);
	//la.mOrignalSrcCode += "\0\0";
	la.getTokenList();
	//la.Lex2File();
	RSyntaxParser parser(la.mTokenList);
	parser.Parse();
	if (parser.Errorflag == 1)
		mSyntaxErrorFlag = true;
	/////////////////////////////
	SParser.Analyze(parser.mSyntaxTree);
	
	//////////////////////////



	//mSemanticLogList.DeleteAllItems();
	if (mLexErrorFlag == true && mSyntaxErrorFlag == true) {
		SParser.SMessage += _T("                  符号表打印如下:\r\n\r\n\r\n");



		SParser.PrintSymbTable();




		CString s = SParser.SMessage;
		//CString s = parser.GetSyntaxTreeStr(_T(" "), _T(""), parser.mSyntaxTree);
		//mSyntaxTreeEdit.SetWindowTextW(s);
		SemanticDlg* dlg = new SemanticDlg;
		dlg->mSemantic = s;
		dlg->Create(IDD_DIALOG1, this->GetDesktopWindow());
		dlg->ShowWindow(SW_SHOW);


	}
	else if (mLexErrorFlag == false) {
		MessageBox(_T("Please run LexicalAnalyzer first or correct the error!"));
	}
	else {
		MessageBox(_T("Please run SyntaxParser first or correct the error!"));
	}
}
void CSNLCompilerDlg::OnBnClickedOpenFileButton()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR filter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	// 构造打开文件对话框   
	CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, filter, this);
	CString path;

	if (IDOK == fileDlg.DoModal())
	{
		path = fileDlg.GetPathName();
		mSrcEdit.SetWindowTextW(Utils::ReadSrc(path));
		mLexErrorFlag = false;
		mSyntaxErrorFlag = false;
		mListControl.DeleteAllItems();
		mSyntaxLogList.DeleteAllItems();
	}
}


void CSNLCompilerDlg::OnEnChangeSrcEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	mLexErrorFlag = false;
	mSyntaxErrorFlag = false;
}



