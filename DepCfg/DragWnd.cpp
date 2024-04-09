// DragWnd.cpp : implementation file
//

#include "stdafx.h"
#include "depcfg.h"
#include "DragWnd.h"

#include <GdiPlus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDragWnd

CDragWnd::CDragWnd()
{
}

CDragWnd::~CDragWnd()
{
}


BEGIN_MESSAGE_MAP(CDragWnd, CWnd)
	//{{AFX_MSG_MAP(CDragWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDragWnd message handlers

BOOL CDragWnd::Create(const CRect& rc, CWnd* pParent)
{
	CString str = AfxRegisterWndClass( CS_HREDRAW|CS_VREDRAW );
	if(!CWnd::Create(str, _T(""), WS_CHILD|WS_VISIBLE, rc, pParent, 1 )) return FALSE;
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
	SetCapture();
	return TRUE;
}

void CDragWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages

	CRect rc;
	GetClientRect(&rc);
	Graphics gr(dc.GetSafeHdc());

	Bitmap bmp( rc.Width(), rc.Height() );
	Graphics buf(&bmp);
	buf.Clear(Color(128, 192, 192, 192));
	gr.DrawImage( &bmp , 0, 0);
}

void CDragWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CDragWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	ReleaseCapture();
	::PostMessage(GetSafeHwnd(), WM_CLOSE, 0, 0);
	CWnd::OnLButtonUp(nFlags, point);
}

void CDragWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnMouseMove(nFlags, point);
}
