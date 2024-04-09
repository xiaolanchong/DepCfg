// PhysicLiteView.cpp : implementation of the CDepCfgView class
//

#include "stdafx.h"
#include "PhysicLiteView.h"
#include "DepCfgLiteDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPhysicLiteView

IMPLEMENT_DYNCREATE(CPhysicLiteView, CPhysicView)

BEGIN_MESSAGE_MAP(CPhysicLiteView, CPhysicView)
	//{{AFX_MSG_MAP(CPhysicView)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView construction/destruction

float g_ScaleTable[] = 
{
	1.5f, 1.0f, 0.75f, 0.5f, 0.25f, 0.1f
};

const int IDT_items = 3;

CPhysicLiteView::CPhysicLiteView(): m_CriticalSection()
{
	// TODO: add construction code here
	
}

CPhysicLiteView::~CPhysicLiteView()
{
}

#ifdef _DEBUG
void CPhysicLiteView::AssertValid() const
{
	CPhysicView::AssertValid();
}

void CPhysicLiteView::Dump(CDumpContext& dc) const
{
	CPhysicView::Dump(dc);
}


#endif //_DEBUG

CDepCfgLiteDoc* CPhysicLiteView::GetDocument()
{
	return STATIC_DOWNCAST( CDepCfgLiteDoc, m_pDocument );
}

/////////////////////////////////////////////////////////////////////////////
// PhysicView message handlers


int CPhysicLiteView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	if (CPhysicView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CDepCfgLiteDoc* pDoc = (CDepCfgLiteDoc*)RUNTIME_CLASS( CDepCfgLiteDoc ) ->CreateObject();
	pDoc->AddView( this );
	// TODO: Add your specialized creation code here
	m_pDB = &pDoc->m_PhysicDB;	

	SetTimer(IDT_items, 250, 0);

	return 0;
}

float*	GetScale( float Scale, bool ZoomIn)
{
	if(ZoomIn)
		for( int i = sizeof(g_ScaleTable)/sizeof(float)-1; i>=0; --i )
		{
			if(g_ScaleTable[i] > Scale) return &g_ScaleTable[i];
		}
	else
		for( int i = 0; i < sizeof(g_ScaleTable)/sizeof(float); ++i )
		{
			if(g_ScaleTable[i] < Scale) return &g_ScaleTable[i];
		}
	return 0;
}

void	CPhysicLiteView::ZoomIn()
{
	if(CheckZoomIn())
	{
		OnScale( *(DWORD*)GetScale(m_Zoom.ScaleX, true), 0 );
	}
}

BOOL	CPhysicLiteView::CheckZoomIn() const
{
	return GetScale(m_Zoom.ScaleX, true) != 0;
}
	
void	CPhysicLiteView::ZoomOut()
{
	if(CheckZoomOut())
	{
		OnScale( *(DWORD*)GetScale(m_Zoom.ScaleX, false), 0 );
	}
}

BOOL	CPhysicLiteView::CheckZoomOut() const
{
	return GetScale(m_Zoom.ScaleX, false) != 0;
}

void	CPhysicLiteView::FitToSize()
{
	OnFit(0,0);
}

void	CPhysicLiteView::State(void* p)
{
	m_CriticalSection.Lock();
	if( GetDocument() )
	{
		BOOL res = GetDocument()->ParseState(p);
		if(res) 
		{
			Invalidate();
			m_EditorState.bToolTip = FALSE;
		}
	}
	m_CriticalSection.Unlock();
}

BOOL	CPhysicLiteView::SetProjectFile(LPCWSTR path, DWORD TimeOutSec)
{
	if ( !GetDocument()) return FALSE;
	BOOL res =  GetDocument()->Load(  path, TimeOutSec);
	if( res) 
	{
		AdjustScroll(false);
	}
	return res;
}

bool	CPhysicLiteView::OpenFromDatabase(CStringW sConnectionString)
{
	if ( !GetDocument()) return FALSE;
	BOOL res =  GetDocument()->LoadFromDB( sConnectionString, 5 );
	if( res) 
	{
		AdjustScroll(false);
	}
	return res == TRUE;
}



bool	CPhysicLiteView::ConnectTeleCallback(ITeleCallback* pCallback, BOOL bInternal)
{
	if ( !GetDocument()) return false;
	return GetDocument()->Connect(pCallback,bInternal  );
//	return false;
}

bool	CPhysicLiteView::DisconnectTeleCallback(ITeleCallback* pCallback, BOOL bInternal)
{
//	return false;
	if ( !GetDocument()) return false;
	return GetDocument()->Disconnect(pCallback,bInternal  );
}

void CPhysicLiteView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch(nIDEvent)
	{
	case IDT_items: if(GetDocument()->Tick()) Invalidate();
					break;
	}

	CPhysicView::OnTimer(nIDEvent);
}