// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Sample.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_ZOOMIN, OnZoomin)
	ON_UPDATE_COMMAND_UI(ID_ZOOMIN, OnUpdateZoomin)
	ON_COMMAND(ID_ZOOMOUT, OnZoomout)
	ON_UPDATE_COMMAND_UI(ID_ZOOMOUT, OnUpdateZoomout)
	ON_COMMAND(ID_FITTO, OnFitto)
	ON_COMMAND( ID_FILE_OPEN, OnFileOpen )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame* CMainFrame::pThis = 0;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_wndView = (CDepCfgView*) RUNTIME_CLASS( CDepCfgView )->CreateObject() ;

	pThis = this;
#ifdef ORWELL_CAM
	bool z = EventStreamConnect( &CallbackProc );
	ASSERT(z);
#endif
}

CMainFrame::~CMainFrame()
{
#ifdef ORWELL_CAM
	EventStreamDisconnect();
#endif
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
/*	if (!m_wndView->Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}*/

	if( !m_wndView->Create( 0, 0, WS_CHILD | WS_VISIBLE , CRect(), this, AFX_IDW_PANE_FIRST	) )
	{
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| /*CBRS_GRIPPER |*/ CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	BOOL res;
	res = m_wndView->SetProjectFile(L"");
	ASSERT(!res);

	m_wndView->OpenFromDatabase();

	m_wndView->ConnectTeleCallback( this );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
//	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
/*	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
*/
	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnZoomin() 
{
	// TODO: Add your command handler code here
	m_wndView->ZoomIn();
}

void CMainFrame::OnUpdateZoomin(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(! m_wndView->CheckZoomIn() ) pCmdUI->Enable(FALSE);
}

void CMainFrame::OnZoomout() 
{
	// TODO: Add your command handler code here
	m_wndView->ZoomOut();
}

void CMainFrame::OnUpdateZoomout(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(! m_wndView->CheckZoomOut() ) pCmdUI->Enable(FALSE);
}

void CMainFrame::OnFitto() 
{
	// TODO: Add your command handler code here
	m_wndView->FitToSize();
}

#ifdef ORWELL_CAM
void __cdecl CMainFrame::CallbackProc(EOrwEvent eventID,  void *pData, long datalen)
{
	if(eventID == EOrwEventInfo)
	{
		pThis->m_wndView->State( pData );
	}
}
#endif

void CMainFrame::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T("*.dep"), NULL, 0, 
					_T("DepCfg files(*.dep)|*.dep|All files(*.*)|*.*||"), 
					this, sizeof(OPENFILENAME));
	if( dlg.DoModal() == IDOK )
	{
		BOOL res = m_wndView->SetProjectFile( dlg.GetPathName() );
		if(!res) AfxMessageBox( _T("Failed to open the configuration file"), MB_OK|MB_ICONERROR );
	}
}