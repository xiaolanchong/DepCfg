// ItemToolBar.cpp : implementation file
//

#include "stdafx.h"
#include "depcfg.h"
#include "ItemToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemToolBar

CItemToolBar::CItemToolBar()
{
}

CItemToolBar::~CItemToolBar()
{
}


BEGIN_MESSAGE_MAP(CItemToolBar, CToolBar/*COXCoolToolBar*/)
	//{{AFX_MSG_MAP(CItemToolBar)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP

	ON_MESSAGE(TB_PRESSBUTTON, OnPressButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemToolBar message handlers

LPARAM CItemToolBar::OnPressButton(WPARAM wParam, LPARAM lParam)
{

    int idButton = wParam; 
//	AfxMessageBox("zz");
 //   lParam = (LPARAM) MAKELONG(fPress, 0); 
	
	return 0;
}

void CItemToolBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	int nID = GetToolBarCtrl().HitTest(&point);

	::SendMessage( GetParent()->GetSafeHwnd(), WM_PRESS, nID, 0 );
#if 0
	COXCoolToolBar::OnLButtonDown(nFlags, point);
#else
	CToolBar::OnLButtonDown(nFlags, point);
#endif
}
