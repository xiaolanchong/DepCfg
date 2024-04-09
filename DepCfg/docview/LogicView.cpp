// LogicView.cpp : implementation of the CDepCfgView class
//

#include "stdafx.h"
#include "DepCfg.h"

#include "../Common/docview/DepCfgDoc.h"
#include "LogicView.h"

#include "../Common/item/GuiComputer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView

IMPLEMENT_DYNCREATE(CLogicView, CEditorView)

BEGIN_MESSAGE_MAP(CLogicView, CEditorView)
	//{{AFX_MSG_MAP(CLogicView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView construction/destruction

CLogicView::CLogicView()
{
	// TODO: add construction code here
	
}

CLogicView::~CLogicView()
{
}

BOOL CLogicView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs



	return CEditorView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView drawing

void CLogicView::OnDraw(CDC* pDC)
{
	CEditorView::OnDraw(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView printing

BOOL CLogicView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLogicView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLogicView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView diagnostics

#ifdef _DEBUG
void CLogicView::AssertValid() const
{
	CEditorView::AssertValid();
}

void CLogicView::Dump(CDumpContext& dc) const
{
	CEditorView::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView message handlers

BOOL CLogicView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

int CLogicView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditorView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	m_pDB = &GetDocument()->m_LogicDB;
	m_pDB->SetCB( this );
	return 0;
}
