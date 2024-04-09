// EditorView.cpp : implementation file
//

#include "stdafx.h"
#include "depcfg.h"
#include "EditorView.h"

#include "MainFrm.h"
#include "DepCfgDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define max_stub(a,b)    (((a) > (b)) ? (a) : (b))

/////////////////////////////////////////////////////////////////////////////
// CEditorView

IMPLEMENT_DYNCREATE(CEditorView, CScrollView)

const int IDT_scroll = 1;
const int IDT_tooltip = 2;

#pragma warning(  disable : 4355  )

CEditorView::CEditorView(): m_pDB(0), m_wndToolTip( static_cast<CWnd*>(this))
{
}

CEditorView::~CEditorView()
{
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

	ON_MESSAGE( WM_SCALE, OnScale )
	ON_MESSAGE( WM_FIT, OnFit )
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

void CEditorView::SetHeader(CString Name)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->SetPropertyBoxHeader( Name );
}


void CEditorView::SetEditProperty(CString Name, CString Value, PropertyObserver* pObserver)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetEditItem( Name, Value, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}


void CEditorView::SetNumEditProperty(CString Name, CString Value, PropertyObserver* pObserver)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetNumEditItem( Name, Value, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

void CEditorView::SetComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetComboBoxItem( Name, Values, nDefValue, false, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

void CEditorView::SetEditComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetComboBoxItem( Name, Values, nDefValue, false, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

void CEditorView::SetIpAddressProperty(CString Name, CString Value, PropertyObserver* pObserver)
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetIpAddress( Name, Value, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

void CEditorView::SetBoolProperty(CString Name, int nDefValue, PropertyObserver* pObserver)
{
	std::vector<CString> Values;
	Values.push_back( CString() );
	CString sDesc;
	sDesc.LoadString( IDS_TRUE );
	Values.push_back( sDesc );
	sDesc.LoadString( IDS_FALSE );
	Values.push_back( sDesc );

	if( nDefValue < 0) nDefValue = 0;
	else if(nDefValue == 0) nDefValue = 2;
	else if(nDefValue == 1) nDefValue = 1;

	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetComboBoxItem( Name, Values, nDefValue, false, pObserver );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

void CEditorView::SetStatic( CString Name, CString Value )
{
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->SetStatic( Name, Value );
	pFrame->GetPropertyBox()->Invalidate(TRUE);
}

////////////////////

void CEditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDepCfgDoc* pDoc = GetDocument();

	ASSERT(!m_EditorState.bAction);

	HideToolTip();

	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->GetPropertyBox()->Reset();

	CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
	PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
	m_EditorState.bAction = m_pDB->StartAction(  ptMouse);
	if( m_EditorState.bAction )
	{
		m_EditorState.ptOldMouse = point;
		CRect rc;
		GetClientRect( &rc );
		ClientToScreen(&rc);
		ClipCursor(&rc);
		SetCapture();
	}
	else 
	{
//		pDoc->m_PhysicDB.ResetSelect();
	}
	
	Invalidate(FALSE);
	
	CScrollView::OnLButtonDown(nFlags, point);
}

void CEditorView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDepCfgDoc* pDoc = GetDocument();
	
	CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
	PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
	if( m_EditorState.bAction)
	{
		m_EditorState.bAction = false;
		m_pDB->EndAction(ptMouse);
		EndEdit();
		
	}
	
	CScrollView::OnLButtonUp(nFlags, point);
}



void CEditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	if( nChar == VK_DELETE )
	{
/*		if(m_EditorState.bAction)
		{
			PointF pt(m_EditorState.ptOldMouse.x, m_EditorState.ptOldMouse.y);
			m_EditorState.bAction = false;
			m_pDB->EndAction(pt);
			EndEdit();
		}
		CDepCfgDoc* pDoc = GetDocument();
		CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
		pFrame->GetPropertyBox()->Reset();
		pDoc->DeleteItems(  m_pDB->GetSelect() );
		Invalidate();*/
		DeleteCurrentItem();
	}
	
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

int CEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->style |= WS_CLIPCHILDREN;
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_wndToolTip.Create( CRect(0, 0, 0, 0), this );
	SetTimer(IDT_tooltip, 150, 0);

	CSize sz(lpCreateStruct->cx, lpCreateStruct->cy);
	if(sz.cx != 0 && sz.cy != 0)
	m_pDB->Invalidate(sz);
	return 0;
}

void CEditorView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
	
	// TODO: Add your message handler code here
/*	ClipCursor(0);
	m_EditorState.bAction = false;*/
	EndEdit();
	CDepCfgDoc* pDoc = GetDocument();
	m_pDB->CancelAction();	
}

BOOL CEditorView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
 /*   CBrush br( GetSysColor( COLOR_WINDOW ) ); 
    FillOutsideRect( pDC, &br );*/
	
	return TRUE;
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

	Graphics	Native(pDC->GetSafeHdc());

//	Native.SetCompositingMode( CompositingModeSourceCopy );
	m_pDB->DrawCached(Native, m_Zoom);

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




	m_pDB->DrawUncached(BackBuffer, Zoom(1.0, 1.0f));
	Native.DrawImage(&BackBitmap, 0, 0);
}

void CEditorView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	
	m_Zoom.ScaleX = m_Zoom.ScaleY = 1.0;

	m_pDB->SetZoom(m_Zoom);
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	pFrame->ResetScale();

	AdjustScroll(true);
}

void CEditorView::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CScrollView::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
	CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	if( !bShow && ::IsWindow(pFrame->GetSafeHwnd()) )
	pFrame->GetPropertyBox()->Reset();
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
/*	
   if(pDC->IsPrinting()) // Is the DC a printer DC.
   {
      CRect rect;
      GetClientRect (&rect);

      int oldMapMode = pDC->SetMapMode(MM_TEXT);
      CSize ptOldWinExt = pDC->SetWindowExt(100, 100);
      ASSERT( ptOldWinExt.cx != 0 && ptOldWinExt.cy != 0 );
      CSize ptOldViewportExt = pDC->SetViewportExt(rect.Width(),
          -rect.Height());
      ASSERT( ptOldViewportExt.cx != 0 && ptOldViewportExt.cy != 0 );
      CPoint ptOldOrigin = pDC->SetViewportOrg(rect.Width()/2,
          rect.Height()/2);
   }
*/

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
	

	 DoPreparePrinting(pInfo);


	CDC dc;
	dc.Attach(pInfo->m_pPD->CreatePrinterDC());
	CSize p(500, 500);
	dc.DPtoLP(&p);
	pInfo->m_rectDraw = CRect(CPoint(0, 0), p);
	dc.Detach();
	
	 return TRUE;
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


//	CSize sizePic ( max_stub(z.cx, prevSize.cx),  max_stub(z.cy, prevSize.cy));
	CSize sizePic ( max_stub(sz.Width, z.cx),  max_stub(sz.Height, z.cy));
	m_pDB->Invalidate( sizePic);
	
	if(fit)
	{
		z = CSize ( sz.Width, sz.Height );
	}
	else
	z = CSize(max_stub( z.cx , sz.Width) , max_stub( z.cy , sz.Height) );
	
	SetScrollSizes(MM_TEXT, z);
	

}

static bool timer = false;

const CSize WndGap(30, 30);
const CSize ScrollGap(60, 60);

void CEditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CDepCfgDoc* pDoc = GetDocument();
	if(m_EditorState.bAction )
	{
		CRect rc;
		GetClientRect(&rc);

		if( rc.right - WndGap.cx <  point.x || rc.bottom - WndGap.cy < point.y) 
		{		
			if(!timer)
			{ 
				SetTimer(IDT_scroll, 500, 0);
			}
			timer = true;
		}
		else if(timer)
		{
			KillTimer(IDT_scroll); timer = false;
//			AdjustScroll();
		}
//		else AdjustScroll();

		CSize sizeOffset = point - m_EditorState.ptOldMouse;
		
		m_EditorState.ptOldMouse = point;
		CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		m_pDB->DoAction( /*m_Zoom.GetInvSize(SizeF(sizeOffset.cx, sizeOffset.cy)), */ptMouse);

		Invalidate(FALSE);
	}
	else	// not in edit tool tip code
	{
		CPoint z = point + CSize(GetScrollPos( SB_HORZ), GetScrollPos(SB_VERT)) ;
		PointF ptMouse  = m_Zoom.GetInvPoint( PointF(z.x, z.y));
		GuiItem* pItem = m_pDB->FromPoint(ptMouse);	
		if( pItem )
		{
			CPoint pt(point);
//			ClientToScreen(&pt);
//			m_wndToolTip.SetWindowPos( 0   , pt.x+5, pt .y+5, 0, 0, SWP_NOZORDER| SWP_NOSIZE );
			MoveToolTip(pt);

//			MoveWindow(pt.x, pt.y, 0, 0, TRUE);
	
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
	}
	
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

/*	if(sizeTotal.cy == sizePos.cy + rc.Height() )
		ret.cy = 30;
	**/
	SetScrollSizes( MM_TEXT, sizeTotal + ret);
/*
	sizePos   =CSize(	GetScrollPos(SB_HORZ), 
						GetScrollPos(SB_VERT) ) ;*/
	SetScrollPos( SB_HORZ, sizePos.cx + scr.cx );
	SetScrollPos( SB_VERT, sizePos.cy + scr.cy );
	//============================
	CDepCfgDoc* pDoc = GetDocument();
	if(m_EditorState.bAction )
	{
		CPoint point = m_EditorState.ptOldMouse ;
		m_EditorState.ptOldMouse = point;
		CSize sizeOffset = scr;
		m_pDB->DoAction( m_Zoom.GetInvSize(SizeF(sizeOffset.cx, sizeOffset.cy)));

//		m_pDB->Invalidate(sizeTotal + ret);

//		m_pDB->EndAction( m_Zoom.GetInvPoint( PointF(sizeWndPos.cx,  sizeWndPos.cx)));
	}	

	m_pDB->Invalidate(sizeTotal + ret);
//	Invalidate(FALSE);
//	m_pDB->Invalidate(  );
}

void CEditorView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	switch(nIDEvent)
	{
	case IDT_scroll:ScrollTimer(); break;
	case IDT_tooltip:ToolTipTimer() ; break;
	}

	CScrollView::OnTimer(nIDEvent);
}

void	CEditorView::ToolTipTimer()
{
	CRect rc;
	GetWindowRect(&rc);

	CPoint pt;
	GetCursorPos(&pt);
	if(!rc.PtInRect(pt))
	{
		HideToolTip();
	}
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

	GetDocument()->SetModifiedFlag();
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

	CSize sz = GetTotalSize();

	sz.cx = max_stub(sz.cx, cx);
	sz.cy = max_stub(sz.cy, cy);

	m_pDB->Invalidate(sz);
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

void	CEditorView::StartEdit()
{
}

void	CEditorView::EndEdit()
{
	m_EditorState.bAction = false;
	ClipCursor(0);
	ReleaseCapture();
	AdjustScroll();
	KillTimer(IDT_scroll);
	Invalidate(FALSE);	
}

void	CEditorView::DeleteCurrentItem()
{
		if(m_EditorState.bAction)
		{
			PointF pt(m_EditorState.ptOldMouse.x, m_EditorState.ptOldMouse.y);
			m_EditorState.bAction = false;
			m_pDB->EndAction(pt);
			EndEdit();
		}
		CDepCfgDoc* pDoc = GetDocument();
		CMainFrame * pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
		pFrame->GetPropertyBox()->Reset();
		pDoc->DeleteItems(  m_pDB->GetSelect() );
		Invalidate();
}

bool	CEditorView::CheckCurrentItem()
{
	std::set<DWORD> Select = m_pDB->GetSelect();
	return !Select.empty();
}
