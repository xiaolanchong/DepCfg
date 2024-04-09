// MainFrm.cpp : implementation of the CMainFrame class
//

#pragma  warning(disable : 4786 )

#include "stdafx.h"
#include "DepCfg.h"

#include "MainFrm.h"
#include "docview/LogicView.h"
#include "../Common/docview/PhysicView.h"


#include "DragWnd.h"
//#include "BridgeWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, MainFrameParent)

BEGIN_MESSAGE_MAP(CMainFrame, MainFrameParent)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_ITEM_SLAVE, OnUpdateItemSlave)
	ON_UPDATE_COMMAND_UI(ID_ITEM_COMMUTATOR, OnUpdateItemCommutator)
	ON_UPDATE_COMMAND_UI(ID_ITEM_COMPUTER, OnUpdateItemComputer)
	ON_UPDATE_COMMAND_UI(ID_ITEM_MASTER, OnUpdateItemMaster)
	ON_WM_CANCELMODE()
	ON_COMMAND(ID_VIEW_PROPERTY, OnViewProperty)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTY, OnUpdateViewProperty)
	ON_COMMAND(ID_VIEW_ITEM, OnViewItem)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ITEM, OnUpdateViewItem)
	ON_WM_ACTIVATE()
	ON_COMMAND(ID_DELETE_ITEM, OnDeleteItem)
	ON_UPDATE_COMMAND_UI(ID_DELETE_ITEM, OnUpdateDeleteItem)
	//}}AFX_MSG_MAP


	ON_MESSAGE(WM_SCALE, OnScale)
	ON_MESSAGE(WM_FIT, OnFit)
	ON_MESSAGE_VOID(WM_UPDATE_TABS, OnUpdateTabs)
	ON_WM_ENTERMENULOOP()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()/*:
	COXMenuBarFrame<MainFrameParent,COXSizeDockBar>
		(CBRS_ALIGN_ANY, RUNTIME_CLASS(COXSizableMiniDockFrameWnd))*/
{
	// TODO: add member initialization code here
/*
	//—оеденение
	BridgeWrapper db;
	CString csConection;
	csConection.Format(L"");
	long dbRes = db.CreateDB(NULL);

	//—читать все ClusterIP
	std::vector<std::wstring> arrIP;
	db.LoadValue(_T("Cluster"), _T("ClusterIP"), arrIP );

	//—читать все ClusterName
	std::vector<std::wstring> arrName;
	db.LoadValue(_T("Cluster"), _T("ClusterName"), arrName );

	//—читать  все CameraID
	std::multimap<int, unsigned int> arrID;
	db.LoadValue(_T("Camera"), _T("CameraID"), arrID );

	//—читать  все CameraType
	std::multimap<int,unsigned int> arrType;
	db.LoadValue(_T("Camera"), _T("CameraType"), arrType );
*/
	/*long dbRes = db.CreateDBInter();
	LPDWORD ListCamera = db.GetIDList(DBBS_MASTER);*/

	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (MainFrameParent::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
#if 0
		| CBRS_GRIPPER 
#endif
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndItemBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
#if 0
		| CBRS_GRIPPER 
#endif
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndItemBar.LoadToolBar(IDR_ITEM))
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
#if 1
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	m_wndItemBar.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndItemBar);

#endif
	// Let the menu organizer fix our menus
	m_Organizer.AttachFrameWnd(this);
	// use button from toolbars with corresponding items in menu
	m_Organizer.AutoSetMenuImage();
#if 0
	m_wndItemBar.DockControlBarLeftOf( &m_wndToolBar );
#else
	DockControlBarLeftOf( &m_wndToolBar, &m_wndItemBar );
#endif
	m_wndDockBar.Create( this);
//	m_wndPropBar.Create(this);
#if 1
	m_wndDockBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndDockBar, AFX_IDW_DOCKBAR_LEFT );
/*	m_wndPropBar.EnableDocking(CBRS_ALIGN_ANY);
	CRect rc(0, 200, 200, 200);
	DockControlBar(&m_wndPropBar, AFX_IDW_DOCKBAR_LEFT, &rc);
*/
#endif
	int nIndex = m_wndToolBar.CommandToIndex(ID_SCALE);
	m_wndToolBar.SetButtonInfo( nIndex, ID_SCALE, TBBS_SEPARATOR , 60 );
	CRect rcCombo;
	m_wndToolBar.GetItemRect(nIndex, &rcCombo);
	rcCombo.bottom +=100;
	m_wndScaleBox.Create( WS_VISIBLE | CBS_DROPDOWNLIST, rcCombo, &m_wndToolBar, ID_SCALE_COMBOBOX );
	//NOTENOTE: resize combobox
	DWORD dwDesireWidth = m_wndScaleBox.GetDesireWidth();
	
	//CRect rcCombo;
	m_wndScaleBox.GetWindowRect(&rcCombo);
	m_wndToolBar.SetButtonInfo( nIndex, ID_SCALE, TBBS_SEPARATOR , dwDesireWidth );
	m_wndScaleBox.SetWindowPos( 0, 0, 0, dwDesireWidth, rcCombo.Height(), SWP_NOZORDER|SWP_NOMOVE );
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !MainFrameParent::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE; 
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	MainFrameParent::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	MainFrameParent::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	DWORD dwStyle = TCS_MULTILINE|TCS_HOTTRACK| 
	TCS_SCROLLOPPOSITE|TCS_RIGHTJUSTIFY|TCS_FOCUSNEVER|WS_VISIBLE|WS_CHILD; 

	if(!m_wndTabContainer.Create(this,CRect(0,0,0,0),dwStyle))
		return FALSE;

	m_wndTabContainer.SetOffsetExternal(0);
	m_wndTabContainer.SetOffsetInternal(0);

	VERIFY(m_ilContainer.Create(IDB_IL_TABVIEWS,16,0,RGB(255,0,255)));
	m_wndTabContainer.SetImageList(&m_ilContainer);

	CString strName;
	strName.LoadString( IDS_PAGE_PHYSIC);
	if(!m_wndTabContainer.AddPage(pContext->m_pNewViewClass,
		pContext,strName,0))
		return FALSE;
	strName.LoadString( IDS_PAGE_LOGIC);
	if(!m_wndTabContainer.AddPage(RUNTIME_CLASS(CLogicView),
		pContext,strName,1))
		return FALSE;

	m_wndTabContainer.SetActivePageIndex(0);
	
	return TRUE;

//	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMainFrame::OnUpdateItemSlave(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nIndex = m_wndTabContainer.GetActivePageIndex();
	if( nIndex  ) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateItemCommutator(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nIndex = m_wndTabContainer.GetActivePageIndex();
	if( !nIndex  ) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnUpdateItemComputer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nIndex = m_wndTabContainer.GetActivePageIndex();
	if( nIndex  ) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnUpdateItemMaster(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	int nIndex = m_wndTabContainer.GetActivePageIndex();
	if( nIndex  ) pCmdUI->Enable(FALSE);
	else pCmdUI->Enable(TRUE);	
}

void CMainFrame::OnCancelMode() 
{
	MainFrameParent::OnCancelMode();
	
	// TODO: Add your message handler code here
	CView * pView = GetActiveView();
	if( pView )
	{
		pView->SendMessage(WM_CANCELMODE);
		m_wndTabContainer.GetActivePage()->SendMessage(WM_CANCELMODE);
	}
}

LRESULT CMainFrame:: OnScale(WPARAM wParam, LPARAM lParam )
{
	m_wndTabContainer.GetPage(0)->SendMessage(WM_SCALE, wParam, lParam);
	m_wndTabContainer.GetPage(1)->SendMessage(WM_SCALE, wParam, lParam);
	return 0;
}

LRESULT CMainFrame:: OnFit(WPARAM wParam, LPARAM lParam )
{
	m_wndTabContainer.GetPage(0)->SendMessage(WM_FIT, wParam, lParam);
	m_wndTabContainer.GetPage(1)->SendMessage(WM_FIT, wParam, lParam);
	return 0;
}
void CMainFrame::OnViewProperty() 
{
	// TODO: Add your command handler code here
	ShowControlBar( &m_wndDockBar, !m_wndDockBar.IsVisible(), TRUE );
}

void CMainFrame::OnUpdateViewProperty(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_wndDockBar.IsVisible())
	{
		pCmdUI->SetCheck();
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CMainFrame::OnViewItem() 
{
	// TODO: Add your command handler code here
	ShowControlBar( &m_wndItemBar, !m_wndItemBar.IsVisible(), TRUE );
}

void CMainFrame::OnUpdateViewItem(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(m_wndItemBar.IsVisible())
	{
		pCmdUI->SetCheck();
	}
	else
	{
		pCmdUI->SetCheck(0);
	}	
}

void CMainFrame::OnEnterMenuLoop( BOOL bIsTrackPopupMenu )
{
	MainFrameParent::OnEnterMenuLoop( bIsTrackPopupMenu );

	m_wndTabContainer.GetActivePage()->SendMessage(WM_CANCELMODE);	
	m_wndDockBar.m_wndListBox.SendMessage(WM_ENTERMENULOOP);
}

void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	MainFrameParent::OnActivate(nState, pWndOther, bMinimized);
	
	// TODO: Add your message handler code here
	m_wndTabContainer.GetActivePage()->SendMessage(WM_CANCELMODE);
	m_wndDockBar.m_wndListBox.SendMessage(WM_ENTERMENULOOP);
}

void CMainFrame::OnDeleteItem() 
{
	// TODO: Add your command handler code here
	static_cast<CEditorView*>(m_wndTabContainer.GetActivePage())->DeleteCurrentItem(  );
}

void CMainFrame::OnUpdateDeleteItem(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	bool res = static_cast<CEditorView*>(m_wndTabContainer.GetActivePage())->CheckCurrentItem(  );
	if( !res) pCmdUI->Enable(FALSE);
}

void CMainFrame::ResetProperty()
{
	if( ::IsWindow( m_wndDockBar.m_wndListBox.GetSafeHwnd() ) )
		m_wndDockBar.m_wndListBox.Reset();
}

void CMainFrame::OnUpdateTabs(WPARAM,LPARAM)
{
	m_wndTabContainer.GetPage(0)->UpdateWindow();
	m_wndTabContainer.GetPage(1)->UpdateWindow();
}

void CMainFrame::DockControlBarLeftOf(CToolBar *leftOf, CToolBar* right)
{
//	ASSERT_VALID(this);
	// make sure CControlBar::EnableDocking has been called
//	ASSERT(m_pDockContext!=NULL);

	if(leftOf!=NULL)
	{
		ASSERT_VALID(leftOf);
	}
	if(right!=NULL)
	{
		ASSERT_VALID(right);
	}

	CFrameWnd* pFrameWnd= this;//GetDockingFrame();
	ASSERT_VALID(pFrameWnd);

	// get MFC to adjust the dimensions of all docked ToolBars
	// so that GetWindowRect will be accurate
	pFrameWnd->RecalcLayout();

	UINT nDockBarID=0;
	CRect rect(0,0,0,0);
	if(leftOf!=NULL)
	{
		leftOf->GetWindowRect(&rect);
		rect.OffsetRect(1,0);

		DWORD dwStyle=leftOf->GetBarStyle();
		nDockBarID=(dwStyle & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP :
		(dwStyle & CBRS_ALIGN_BOTTOM) ? AFX_IDW_DOCKBAR_BOTTOM :
		(dwStyle & CBRS_ALIGN_LEFT) ? AFX_IDW_DOCKBAR_LEFT :
		(dwStyle & CBRS_ALIGN_RIGHT) ? AFX_IDW_DOCKBAR_RIGHT : 0;
	}


	// When we take the default parameters on rect, DockControlBar will dock
	// each Toolbar on a seperate line. Calculating a rectangle, we in effect
	// are simulating a Toolbar being dragged to that location and docked.
	DockControlBar(right, nDockBarID, &rect);
}