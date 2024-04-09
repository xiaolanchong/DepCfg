// ToolTipWnd.cpp : implementation file
//

#include "stdafx.h"
//#include "depcfg.h"

#include <gdiplus.h>
using namespace Gdiplus;

#include "ToolTipWnd.h"
#include "Helper.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C"
{
WINUSERAPI
BOOL
WINAPI
SetLayeredWindowAttributes(
  HWND hwnd,           // handle to the layered window
  COLORREF crKey,      // specifies the color key
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // action
);	
}

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x00080000
#endif

#ifndef LWA_ALPHA
#define LWA_ALPHA	0x00000002
#endif

/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd

CToolTipWnd::CToolTipWnd(CWnd* pWnd):m_NameCol(0),m_ValueCol(0), m_Cell(0), m_pWnd(pWnd)
{
}

CToolTipWnd::~CToolTipWnd()
{
}


BEGIN_MESSAGE_MAP(CToolTipWnd, CStatic)
	//{{AFX_MSG_MAP(CToolTipWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd message handlers

BOOL CToolTipWnd::Create(const CRect& rc, CWnd* pParent)
{
	//LPCTSTR szClass = AfxRegisterWndClass(0);
	BOOL res = CWnd::CreateEx( 0/*WS_EX_LAYERED| WS_EX_TOOLWINDOW*/, _T("STATIC"), _T(""), /*WS_POPUP*/WS_CHILD , rc,/*0*/pParent, 0 );
//	BOOL res = CToolTipCtrl::Create(pParent, 0);
	ASSERT(res);

	return res;

	
}


void CToolTipWnd::SetReadOnlyProperty( CString Name, CString Value )
{
	m_Properties.push_back( std::make_pair(Name, Value) );
//	Bitmap b(10,10);
	Graphics gr(GetSafeHwnd());

	RectF rc;
	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleBold, UnitPixel);
	
	gr.MeasureString( Helper::ConvertW(Name).c_str(), -1, &font, PointF(0,0), &rc );
	if(m_NameCol < rc.Width+10)  m_NameCol = rc.Width+10;
	if(m_Cell < rc.Height)  m_Cell = rc.Height;
	gr.MeasureString( Helper::ConvertW(Value).c_str(), -1, &font, PointF(0,0), &rc );
	if(m_ValueCol< rc.Width+10)  m_ValueCol = rc.Width+10	;
	if(m_Cell < rc.Height)  m_Cell = rc.Height;

	int h = m_Cell * m_Properties.size();
	gr.~Graphics();
	int w = m_ValueCol+ m_NameCol;
	SetWindowPos(0, 0, 0, w, h, SWP_NOZORDER|SWP_NOMOVE);
}

void CToolTipWnd::EraseProperty( CString Name)
{
	
}

void CToolTipWnd::Clear()
{
	m_Properties.clear();
	m_ValueCol = 0;
	m_NameCol = 0;
	m_Cell = 0;
}


void CToolTipWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	

	CRect rc;
	GetClientRect(&rc);

	Graphics	Native(dc.GetSafeHdc()/*MemDC.GetSafeHdc()*/);

	Bitmap		BackBitmap(  rc.Width(), rc.Height());
	Graphics	BackBuffer(&BackBitmap);

	SolidBrush br(Color::Ivory);
	Pen pen(Color::Gray);

	BackBuffer.FillRectangle( &br, 0, 0, rc.Width(), rc.Height() );
	BackBuffer.DrawRectangle( &pen, 0, 0, rc.Width()-1, rc.Height()-1 );

	Draw(BackBuffer);

	Native.DrawImage(&BackBitmap, 0, 0);
}

BOOL CToolTipWnd::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;//CWnd::OnEraseBkgnd(pDC);
}

int CToolTipWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;	
	// TODO: Add your specialized creation code here
	BOOL res = 	SetLayeredWindowAttributes( 
		#if _MSC_VER <= 1200
		GetSafeHwnd(), 
		#endif
		RGB(255,255,255), 128, LWA_ALPHA );
	return 0;
}

void	CToolTipWnd::Draw(Graphics& gr) const
{
	CRect rc;
	GetClientRect(&rc);

	if(m_Properties.empty()) return;

	CString str = m_Properties[0].first;

	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleBold, UnitPixel);

	Font        font2(&fontFamily, 12, FontStyleRegular, UnitPixel);
	   
	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentFar);
	stringFormat.SetAlignment(StringAlignmentNear);
//	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter   );
	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap   );

	for(size_t i = 0; i < m_Properties.size(); i++)
	{
		RectF f( 5, i*m_Cell, m_NameCol, m_Cell );
		RectF s( m_NameCol, i*m_Cell, m_ValueCol, m_Cell );
	gr.DrawString( Helper::ConvertW(m_Properties[i].first).c_str(), -1, &font2, 
					f, 
					&stringFormat, 
					&SolidBrush( Color::Indigo));

	gr.DrawString( Helper::ConvertW(m_Properties[i].second).c_str(), -1, &font, 
					s, 
					&stringFormat, 
					&SolidBrush( Color::Teal));
	}
}

LRESULT CToolTipWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	switch( message )
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_SYSCHAR:
		return m_pWnd->SendMessage( message, wParam, lParam );
	}

	return CStatic::WindowProc(message, wParam, lParam);
}
