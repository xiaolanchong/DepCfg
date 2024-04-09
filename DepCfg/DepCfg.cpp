// DepCfg.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DepCfg.h"
#include <atlbase.h>
#include "../Common/elvees_loc.h"

#include "MainFrm.h"
#include "docview/DepCfgFullDoc.h"
#include "../Common/docview/PhysicView.h"
#include "docview/LogicView.h"
#include "ModuleVersion.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgApp

BEGIN_MESSAGE_MAP(CDepCfgApp, CWinApp)
	//{{AFX_MSG_MAP(CDepCfgApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgApp construction

CDepCfgApp::CDepCfgApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDepCfgApp object

CDepCfgApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDepCfgApp initialization

BOOL CDepCfgApp::InitInstance()
{
/*	const DWORD c_DefLCID = MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT );
	DWORD dwLangID = c_DefLCID;
	CRegKey	reg;
	LONG res;
	res = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ELVEES\\Common"), KEY_READ ) ; 
	if( res == ERROR_SUCCESS )
	{
		res = reg.QueryDWORDValue( _T("LangLCID"), dwLangID );
		if( res != ERROR_SUCCESS )
		{
			dwLangID = c_DefLCID;
		}
	}
	SetThreadLocale( MAKELCID(dwLangID, SORT_DEFAULT));

	AfxEnableControlContainer();*/

	ElVEES_NLS::SetNLSLocale();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
#if _MSC_VER <= 1200
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(IDS_REGISTRYKEY);

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	//CRuntimeClass* pc = RUNTIME_CLASS( CDepCfgFullDoc );

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CDepCfgFullDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPhysicView));
	AddDocTemplate(pDocTemplate);

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDepCfgApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgApp message handlers


BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CModuleVersion Ver;
	TCHAR buf[MAX_PATH];
	GetModuleFileName( AfxGetInstanceHandle(), buf, MAX_PATH ) ;

	BOOL z = Ver.GetFileVersionInfo(buf);
	CString sz = Ver.GetValue(  _T("FileVersion"));
	
	CString str;
	sz.Replace(',', '.');
	sz.Remove(' ');
	GetDlgItemText(IDC_VERSION, str);
	str += _T(" ");
	str += sz;
	SetDlgItemText(IDC_VERSION, str);	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
