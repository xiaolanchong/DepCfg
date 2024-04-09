// EditorView.cpp : implementation file
//

#include "stdafx.h"
#include "EditorView.h"

#include "../../common/docview/DepCfgDoc.h"
#include <afxsock.h>


#include "../../common/item/ProjectDB.h"
#include "../../common/utility/ToolTipWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditorView

IMPLEMENT_DYNCREATE(CEditorView, CScrollView)

const int IDT_scroll = 1;
const int IDT_tooltip = 2;

#pragma warning(  disable : 4355  )

CEditorView::CEditorView(): m_pDB(0), m_wndToolTip( * (new CToolTipWnd(static_cast<CWnd*>(this)))), m_Zoom( *new Zoom() )
{
	AfxSocketInit(0);
}

CEditorView::~CEditorView()
{
	delete &m_wndToolTip;
	delete &m_Zoom;
}


BEGIN_MESSAGE_MAP(CEditorView, CScrollView)
	//{{AFX_MSG_MAP(CEditorView)
	ON_WM_CANCELMODE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP

//ON_MESSAGE( WM_SCALE, OnScale )
//ON_MESSAGE( WM_FIT, OnFit )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorView drawing


/////////////////////////////////////////////////////////////////////////////
// CEditorView diagnostics



#ifdef _DEBUG
void CEditorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CEditorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CDepCfgDoc* CEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDepCfgDoc)));
	return (CDepCfgDoc*)m_pDocument;
}


#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEditorView message handlers

////////////////////

void CEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CScrollView::OnLButtonDown(nFlags, point);
}

void CEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDepCfgDoc* pDoc = GetDocument();
	
	
	CScrollView::OnLButtonUp(nFlags, point);
}



void CEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_DELETE )

	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= WS_CLIPCHILDREN;
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	// TODO: Add your specialized creation code here
	m_wndToolTip.Create( CRect(0, 0, 0, 0), this );
	SetTimer(IDT_tooltip, 200, 0);
#if 0
	SetTimer(IDT_items, 200, 0);
#endif
	return 0;
}

void CEditorView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
		
}

BOOL CEditorView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
 /*   CBrush br( GetSysColor( COLOR_WINDOW ) ); 
    FillOutsideRect( pDC, &br );*/
	
	return TRUE;
}

CSize	CEditorView::GetInvalidateSize()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	CSize szScroll = GetTotalSize();
	CSize szPos = GetScrollPosition( );  
	rcClient.InflateRect( 0, 0, szPos.cx, szPos.cy );
	CSize szInv;

	szInv.cx = max( szScroll.cx , rcClient.Size().cx);
	szInv.cy = max( szScroll.cy , rcClient.Size().cy);
	return szInv;
}

void CEditorView::OnDraw(CDC* pDC)
{
	CDepCfgDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here

	if(pDC->IsPrinting()) 
	{
		OnPrint(pDC);return;
	}

	if( m_pDB->IsCached() )
	{
		m_pDB->Draw( pDC, m_Zoom );
	}
	else
	{
		CSize szInv = GetInvalidateSize();
		Graphics gr( pDC->GetSafeHdc() );

		Bitmap		BackBmp( szInv.cx, szInv.cy );
		Graphics	BackBuffer( &BackBmp );

		BackBuffer.Clear( Color::White );

		m_pDB->Draw( BackBuffer, m_Zoom );

		gr.DrawImage( &BackBmp, 0, 0 );
	}
}

void CEditorView::OnPrint(CDC* pDC)
{

	int iWidth  = pDC->GetDeviceCaps( HORZRES);
    int iHeight = pDC->GetDeviceCaps( VERTRES);

	Graphics	Native(pDC->GetSafeHdc());


	CSize sz = GetTotalSize();
	pDC->LPtoDP(  &sz );


	Bitmap		BackBitmap(  iWidth, iHeight);
	Graphics	BackBuffer(&BackBitmap);


	CRectF rc = m_pDB->GetRect();
	if(rc.IsEmptyArea()) return ;

	SizeF Off = SizeF(-rc.X, -rc.Y);
	
	rc.Inflate(20, 20, 20, 20);

//	m_pDB->Offset( SizeF(-rc.X, -rc.Y) );
	
	rc.Offset( Off );
	float ScaleX = rc.Width / iWidth;
	float ScaleY = rc.Height / iHeight;
	float Scale = ScaleX > ScaleY ? ScaleX : ScaleY ;

	Zoom z(1.0f/Scale, 1.0f/Scale);

//	m_Zoom.ScaleX = m_Zoom.ScaleY = 1.0f/Scale;



	BackBuffer.SetTransform( &Matrix(z.ScaleX,0,0,z.ScaleX, Off.Width / Scale, Off.Height/ Scale));
	m_pDB->Draw(BackBuffer, Zoom(1.0, 1.0f));

	Native.DrawImage(&BackBitmap, 0, 0);
}

void CEditorView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
	m_Zoom.ScaleX = m_Zoom.ScaleY = 1.0;

	m_pDB->SetZoom(m_Zoom);
/*	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->ResetScale();*/

	AdjustScroll(true);
}

void CEditorView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CScrollView::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here

}

LRESULT CEditorView::OnScale(WPARAM wParam, LPARAM lParam )
{
	float size = *(float*)&wParam;

	m_Zoom.ScaleX = m_Zoom.ScaleY = size;

	m_pDB->SetZoom(m_Zoom);
	AdjustScroll();
	Invalidate();
	return 0;
}

void CEditorView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class

	CScrollView::OnPrepareDC(pDC, pInfo);
}

void CEditorView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	


	CScrollView::OnBeginPrinting(pDC, pInfo);
}

BOOL CEditorView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: call DoPreparePrinting to invoke the Print dialog box

	 return TRUE;
}

void CEditorView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint )
{
//	AdjustScroll(false);
}

void CEditorView::AdjustScroll(bool fit)
{
	CRectF rc = m_pDB->GetRect();
	SizeF sz = m_Zoom.GetSize( SizeF(rc.Right(), rc.Bottom()) );
	sz.Width += 20; 
	sz.Height += 20;

	CRect rcCl;
	GetClientRect(&rcCl);

	CSize prevSize = GetTotalSize();
	CSize prevPos (  GetScrollPos(SB_HORZ), GetScrollPos(SB_VERT) );

	CSize z  (GetScrollPos(SB_HORZ) + rcCl.Width(),
		       GetScrollPos(SB_VERT) + rcCl.Height());

	CSize sizePic ( max(z.cx, prevSize.cx),  max(z.cy, prevSize.cy));
	m_pDB->Resize( sizePic);
	
	if(fit)
	{
		//NOTENOTE: subtrate some gap 2 or 3 px
		z = CSize ( sz.Width - 3, sz.Height -3 );
	}
	else
	z = CSize(max( z.cx , sz.Width) , max( z.cy , sz.Height) );
	
	SetScrollSizes(MM_TEXT, z);
}

static bool timer = false;

const CSize WndGap(30, 30);
const CSize ScrollGap(60, 60);

void CEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDepCfgDoc* pDoc = GetDocument();
		CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		GuiItem* pItem = m_pDB->FromPoint(ptMouse);	
		if( pItem )
		{
			CPoint pt(point);

			MoveToolTip(pt);

			if(! m_EditorState.bToolTip)
			{
				m_wndToolTip.Clear();
				pItem->SetReadOnlyProperty( &m_wndToolTip );	
				m_wndToolTip.Invalidate();
				m_wndToolTip.ShowWindow( SW_SHOWNOACTIVATE );
				m_EditorState.bToolTip  = true;
			}
		}
		else if(m_EditorState.bToolTip )
			HideToolTip();
	
	CScrollView::OnMouseMove(nFlags, point);
}

void  CEditorView::ScrollTimer()
{
	CSize sizeTotal;
	int z;
	GetDeviceScrollSizes (z, sizeTotal, CSize(), CSize());
	
	CSize sizePos   (	GetScrollPos(SB_HORZ), 
						GetScrollPos(SB_VERT) ) ;

	CRect rc;
	GetClientRect(&rc);

	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient( &pt );

	CSize sizeWndPos = sizePos + rc.BottomRight();

	CSize sz = GetTotalSize();
	if(sz.cx < sizeWndPos.cx )
	{		
		sizeTotal.cx = sizeWndPos.cx ;
	}
	if(sz.cy < sizeWndPos.cy )
	{		
		sizeTotal.cy = sizeWndPos.cy ;
	}

	CSize ret (0, 0);
	CSize scr (0, 0);
	if( rc.right - WndGap.cx < pt.x)
	{
		if(sizeTotal.cx == sizePos.cx + rc.Width() )
		{
			ret.cx = ScrollGap.cx;
			scr.cx = ScrollGap.cx;
		}
		else 
		{
			scr.cx = sizeTotal.cx - sizeWndPos.cx;
			if(scr.cx > ScrollGap.cx)
				scr.cx = ScrollGap.cx;
		}
	}
	else if( WndGap.cx > pt.x  )
	{
		scr.cx = -ScrollGap.cx;
		ret.cx = ScrollGap.cx; 
//		m_pDB->OffsetUnselect( m_Zoom.GetInvSize(SizeF(-scr.cx, 0)));
	}

	if( rc.bottom - WndGap.cy < pt.y)
	{
		if(sizeTotal.cy == sizePos.cy + rc.Height() )
		{
			ret.cy = ScrollGap.cy;
			scr.cy = ScrollGap.cy;
		}
		else 
		{
			scr.cy = sizeTotal.cy - sizeWndPos.cy;
			if(scr.cy > ScrollGap.cy)
				scr.cy = ScrollGap.cy;
		}
	}
	else if( WndGap.cy > pt.y  )
	{
		scr.cy = -ScrollGap.cy;
		ret.cy = ScrollGap.cy; 
//		m_pGuiDB->OffsetUnselect( m_Zoom.GetInvSize(SizeF(0, -scr.cy)));
	}

	SetScrollSizes( MM_TEXT, sizeTotal + ret);
	SetScrollPos( SB_HORZ, sizePos.cx + scr.cx );
	SetScrollPos( SB_VERT, sizePos.cy + scr.cy );
	//============================
/*	if(m_EditorState.Action == EditorState::drag )
	{
		CPoint point = m_EditorState.ptOldMouse ;
		m_EditorState.ptOldMouse = point;
		CSize sizeOffset ( scr.cx, scr.cy < 0 ? 0 : scr.cy);
		m_pDB->DoAction( m_Zoom.GetInvSize(SizeF(sizeOffset.cx, sizeOffset.cy)));
	}	*/

	m_pDB->Resize(sizeTotal + ret);
}

void CEditorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch(nIDEvent)
	{
	case IDT_scroll:ScrollTimer(); break;
	case IDT_tooltip:ToolTipTimer() ; break;
#if 0
	case IDT_items: if(GetDocument()->Tick()) Invalidate();
					break;
#endif
	}

	CScrollView::OnTimer(nIDEvent);
}

void	CEditorView::ToolTipTimer()
{
	CRect rc;
	GetWindowRect(&rc);

	CPoint point;
	GetCursorPos(&point);
	if(!rc.PtInRect(point))
	{
		HideToolTip();
	}

	ScreenToClient( &point );

		CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		if( !m_pDB ) return;
		GuiItem* pItem = m_pDB->FromPoint(ptMouse);	
		if( pItem )
		{
			CPoint pt(point);

			MoveToolTip(pt);

//			if(! m_EditorState.bToolTip)
			{
				m_wndToolTip.Clear();
				pItem->SetReadOnlyProperty( &m_wndToolTip );	
				m_wndToolTip.Invalidate();
				m_wndToolTip.ShowWindow( SW_SHOWNOACTIVATE );
				m_EditorState.bToolTip  = true;
			}
		}
/*		else if(m_EditorState.bToolTip )
			HideToolTip();*/

}

void	CEditorView::HideToolTip()
{
	m_wndToolTip.ShowWindow(SW_HIDE);
	m_wndToolTip.Clear();
	m_EditorState.bToolTip = false;
}

LRESULT CEditorView::OnFit(WPARAM wParam, LPARAM lParam )
{
	CRect rcClient;
	GetClientRect(&rcClient);

	// tab ctrl  don't resize child until it's active so get client rect from physic page 
	if(rcClient.IsRectEmpty())
	{
		POSITION pos = GetDocument()->GetFirstViewPosition();
		GetDocument()->GetNextView(pos)->GetClientRect(&rcClient);
	}

	CRectF rc = m_pDB->GetRect();

	if(rc.IsEmptyArea()) return 0;

	rc.Inflate(20, 20, 20, 20);
//	rc.Inflate(20, 20, 20, 20);

	float ScaleX = rc.Width / (rcClient.Width());
	float ScaleY = rc.Height / (rcClient.Height());

	m_pDB->Offset( SizeF(-rc.X, -rc.Y) );

	float Scale = ScaleX > ScaleY ? ScaleX : ScaleY ;


	m_Zoom.ScaleX = m_Zoom.ScaleY = 1.0f/Scale;

	m_pDB->SetZoom(m_Zoom);

	

	AdjustScroll(true);
	Invalidate();

//	GetDocument()->SetModifiedFlag();
	return 0;
}

void	CEditorView::MoveToolTip(CPoint pt)
{
	CRect rcWindow, rcToolTip;
	GetClientRect(&rcWindow);
	m_wndToolTip.GetWindowRect(&rcToolTip);
	ScreenToClient(&rcToolTip);

	CPoint ptWindow = pt;
/*
	if( rcToolTip.bottom > rcWindow.bottom )
		ptWindow.y -= rcToolTip.Height();
		ptWindow = rcWindow.CenterPoint();
	if( rcToolTip.right > rcWindow.right )
		ptWindow.x -= rcToolTip.Width();*/
	
	m_wndToolTip.SetWindowPos( 0, ptWindow.x + 5, ptWindow.y + 5, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
//	m_wndToolTip.MoveWindow( ptWindow.x, ptWindow.y, rcToolTip.Width(), rcToolTip.Height());
}

void CEditorView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
}

BOOL CEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: Add your message handler code here and/or call default
/*	if(m_EditorState.bAction)
	{
		CSize sizePos   (	GetScrollPos(SB_HORZ), 
							GetScrollPos(SB_VERT) ) ;

		CPoint point = m_EditorState.ptOldMouse ;
		m_EditorState.ptOldMouse = pt;
		CSize sizeOffset = pt - point;
		CPoint z = point + sizePos ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		m_pDB->DoAction( m_Zoom.GetInvSize(SizeF(sizeOffset.cx, sizeOffset.cy)), ptMouse);

		Invalidate(FALSE);
		
	}	*/
//	m_pDB->Invalidate();
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
/*
	if(m_EditorState.bAction)
	{
		CSize sizePos   (	GetScrollPos(SB_HORZ), 
							GetScrollPos(SB_VERT) ) ;

		CPoint point = m_EditorState.ptOldMouse ;
		m_EditorState.ptOldMouse = pt;
		CSize sizeOffset = pt - point;
		CPoint z = point + sizePos ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		m_pDB->DoAction( m_Zoom.GetInvSize(SizeF(sizeOffset.cx, sizeOffset.cy)), ptMouse);

		Invalidate(FALSE);
		
	}	
	*/
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

CSize	CEditorView::GetInvRect()
{
	CRect rc;
	GetClientRect(&rc);
	CSize sizeScroll = GetTotalSize();

	return sizeScroll;
}