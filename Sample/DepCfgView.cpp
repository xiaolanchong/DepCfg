// DepCfgView.cpp : implementation file
//

#include "stdafx.h"
#include "Sample.h"
#include "DepCfgView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView

IMPLEMENT_DYNCREATE(CDepCfgView, CView)

CDepCfgView::CDepCfgView()
{
}

CDepCfgView::~CDepCfgView()
{
}


BEGIN_MESSAGE_MAP(CDepCfgView, CView)
	//{{AFX_MSG_MAP(CDepCfgView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView drawing

void CDepCfgView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView diagnostics

#ifdef _DEBUG
void CDepCfgView::AssertValid() const
{
	CView::AssertValid();
}

void CDepCfgView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView message handlers

int CDepCfgView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= WS_CLIPCHILDREN;
//	lpCreateStruct->s
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	m_Child.Create( WS_VISIBLE | WS_CHILD, this);

	return 0;
}


void	CDepCfgView::ZoomIn()
{
	m_Child.ZoomIn();
}

BOOL	CDepCfgView::CheckZoomIn() const
{
	return m_Child.CheckZoomIn();
}
	
void	CDepCfgView::ZoomOut()
{
	m_Child.ZoomOut();
}

BOOL	CDepCfgView::CheckZoomOut() const
{
	return m_Child.CheckZoomOut();
}

void	CDepCfgView::FitToSize()
{
	m_Child.FitToSize();
}

void	CDepCfgView::State(void* p)
{
	m_Child.State(p);
}


BOOL	CDepCfgView::SetProjectFile(	LPCWSTR path,	// Full file name of configuration
							DWORD TimeOutSec )	// Timeout, sec
{
	return m_Child.SetProjectFile( path, TimeOutSec );
//	return
}
	
bool	CDepCfgView::ConnectTeleCallback(ITeleCallback* pCallback)
{
	return m_Child.ConnectTeleCallback( pCallback );
}

bool	CDepCfgView::DisconnectTeleCallback(ITeleCallback* pCallback )
{
	return m_Child.DisconnectTeleCallback( pCallback);
}

bool	CDepCfgView::OpenFromDatabase(  )
{
	CStringW s( true ? L"" :
				L"Provider=SQLOLEDB.1;Persist Security Info=False;Initial Catalog=OrwellV2;Data Source=shuttlex-pc;Use Encryption for Data=False;Tag with column collation when possible=False;User ID=OrwellSAdmin;Password=t*e_AVafrEzUtaTha2rEmUJE@ew#xaze"
				);
	return m_Child.OpenFromDatabase( s );
}

void CDepCfgView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	
//	GetClientRect(  )
	m_Child.SetWindowPos( 0, 0, 0, cx, cy, SWP_NOMOVE|SWP_NOZORDER );
}

BOOL CDepCfgView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CView::OnEraseBkgnd(pDC);
}
