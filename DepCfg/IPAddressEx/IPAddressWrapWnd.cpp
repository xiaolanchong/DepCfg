// IPAddressWrapWnd.cpp : implementation file
//

#include "stdafx.h"
#include "DepCfg.h"
#include "IPAddressWrapWnd.h"

// CIPAddressWrapWnd

IMPLEMENT_DYNAMIC(CIPAddressWrapWnd, CWnd)
CIPAddressWrapWnd::CIPAddressWrapWnd()
{
}

CIPAddressWrapWnd::~CIPAddressWrapWnd()
{
}


BEGIN_MESSAGE_MAP(CIPAddressWrapWnd, CWnd)
	ON_MESSAGE( WM_SETFONT, OnMessage<WM_SETFONT> )
	ON_MESSAGE( WM_SETTEXT, OnMessage<WM_SETTEXT> )
	ON_MESSAGE( WM_GETTEXT, OnMessage<WM_GETTEXT> )
	ON_MESSAGE( WM_GETTEXTLENGTH, OnMessage<WM_GETTEXTLENGTH> )
	ON_MESSAGE( WM_SETFOCUS, OnMessage<WM_SETFOCUS> )
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CIPAddressWrapWnd message handlers

BOOL CIPAddressWrapWnd::Create(   DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	CString sClass = AfxRegisterWndClass( 0, 0, (HBRUSH)COLOR_WINDOW );
	BOOL res = CWnd::Create( sClass, NULL, WS_CHILD | ( dwStyle&WS_VISIBLE ), rect, pParentWnd, nID  );
	ASSERT(res);
	if(!res) return FALSE;
	res = m_wndIPAddress.Create( dwStyle|WS_VISIBLE, rect, this, nID );
	if(res) 
	{
		m_wndIPAddress.ModifyStyleEx( WS_EX_CLIENTEDGE, 0 );
		ResizeChild();
	}
	return res;
}

void CIPAddressWrapWnd::OnSize( UINT nType, int cx, int cy )
{
	CWnd::OnSize( nType, cx, cy );
	ResizeChild();
}

void	CIPAddressWrapWnd::ResizeChild()
{
	if( IsWindow( m_wndIPAddress.GetSafeHwnd() ) )
	{
		CDC* pDC = m_wndIPAddress.GetDC();
		CSize sizeText = pDC->GetTextExtent(_T("000.000.000.000"));
		//! from comctl32.dll v5.81 sources - lead gap
		sizeText.cx += 3;
		m_wndIPAddress.ReleaseDC(pDC);

		CRect rc;
		GetClientRect(rc);
		CSize sizeClient = rc.Size();
		sizeClient.cx = max ( sizeClient.cx , sizeText.cx );
		sizeClient.cy = max ( sizeClient.cy , sizeText.cy );
		m_wndIPAddress.SetWindowPos(0, 0, 0, sizeClient.cx, sizeClient.cy, SWP_NOZORDER|SWP_NOMOVE);
		m_wndIPAddress.SendMessage( WM_SETFONT, 0, TRUE );
	}
}

BOOL CIPAddressWrapWnd::OnCommand( WPARAM w, LPARAM l )
{
	CWnd* pWnd = GetParent();
	if(pWnd)
	{
		w = MAKEWPARAM(  GetDlgCtrlID(), HIWORD(w) );
		return pWnd->SendMessage( WM_COMMAND, w, l );
	}
	else return 0;
}