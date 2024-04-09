// PhysicView.cpp : implementation of the CDepCfgView class
//

#include "stdafx.h"
//#include "DepCfg.h"


#include "PhysicView.h"
#include "DepCfgDoc.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView

IMPLEMENT_DYNCREATE(CPhysicView, CEditorView)

BEGIN_MESSAGE_MAP(CPhysicView, CEditorView)
	//{{AFX_MSG_MAP(CPhysicView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView construction/destruction

CPhysicView::CPhysicView()
{
	// TODO: add construction code here
	
}

CPhysicView::~CPhysicView()
{
}

BOOL CPhysicView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CEditorView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView drawing



/////////////////////////////////////////////////////////////////////////////
// CDepCfgView printing

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView diagnostics

#ifdef _DEBUG
void CPhysicView::AssertValid() const
{
	CEditorView::AssertValid();
}

void CPhysicView::Dump(CDumpContext& dc) const
{
	CEditorView::Dump(dc);
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// PhysicView message handlers


int CPhysicView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	
	if (CEditorView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
#ifndef DEPCFGLITE
	CDepCfgDoc* pDoc = GetDocument();
	m_pDB = &pDoc->m_PhysicDB;	
	m_pDB->SetCB( this );
#endif
	return 0;
}

void CPhysicView::OnDraw(CDC* pDC)
{
	CEditorView::OnDraw(pDC);
}