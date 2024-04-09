// ==========================================================================
// 							   Class Implementation : 
//									COXMenuBar 
// ==========================================================================

// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                          
// //////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "OXMenuBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _MFC_VER>=0x0420


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// COXMenuBar
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(COXMenuBar,COXCoolToolBar)

COXMenuBar::COXMenuBar()
{
	// only works when comctl32.dll v4.71>= is installed
	if(m_dwComCtlVersion<_IE40_COMCTL_VERSION)
	{
		TRACE(_T("In order to use Menu Bar in your application you have to have comctl32.dll v 4.71 or higher installed\n"));
		AfxThrowNotSupportedException();
	}

	// use entire row/column
	m_bTakeEntireRow=TRUE;

	m_hMenu=NULL;

	m_nActiveMenuItem=-1;
	m_nActivateNextItem=-1;
	m_nForbiddenItem=-1;

	m_rectCloseBtn.SetRectEmpty();
	m_rectRestoreBtn.SetRectEmpty();
	m_rectMinimizeBtn.SetRectEmpty();
	m_pressedBtn=NONE;

	m_cxLeftBorder=0;
	m_cyTopBorder=0;
	m_cxRightBorder=0;
	m_cyBottomBorder=0;

	m_bIsInSurfingMode=FALSE;

	m_bForceEntireRow=FALSE;

	m_bIsMenuMetricInitialized=FALSE;
}

COXMenuBar::~COXMenuBar()
{
}

BEGIN_MESSAGE_MAP(COXMenuBar, COXCoolToolBar)
	//{{AFX_MSG_MAP(COXMenuBar)
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCHITTEST()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_NCLBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_SETTINGCHANGE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_UPDATE_COMMAND_UI_RANGE(0,0xffff,OnUpdateMenuItem)
	ON_NOTIFY_REFLECT_EX(TBN_DROPDOWN,OnMenuDropDown)
	ON_MESSAGE(WM_DISPLAYPOPUPMENU, OnDisplayPopupMenu)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL COXMenuBar::Create(CFrameWnd* pParentWnd, 
						DWORD dwStyle/*=WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_SIZE_DYNAMIC*/, 
						UINT nID/*=AFX_IDW_MENUBAR*/)
{
	ASSERT(pParentWnd!=NULL);
	ASSERT(::IsWindow(pParentWnd->m_hWnd));
	ASSERT(pParentWnd->IsKindOf(RUNTIME_CLASS(CFrameWnd)));

	// try to create cool toolbar
	if(!COXCoolToolBar::Create(pParentWnd,dwStyle,nID))
		return FALSE;

	// set size of images to zero (cannot call SetSizes function
	// because it will assert if zero value is specified)
	SendMessage(TB_SETBITMAPSIZE,0,MAKELPARAM(0,0));

	// set window text
	SetWindowText(_T("Menu"));

	// save pointer to parent window
	m_pFrameWnd=pParentWnd;

	return TRUE;
}

void COXMenuBar::OnFloatingDocking(BOOL bFloating) 
{
	// TODO: Add your message handler code here
	COXCoolToolBar::OnFloatingDocking(bFloating);
	
	if(bFloating)
	{
		// remove all items form mini dock frame window system menu
		CFrameWnd* pMiniDockFrame=
			(CFrameWnd*)GetDockingFrame();
		ASSERT(pMiniDockFrame!=NULL &&
			pMiniDockFrame->IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)));
		if(pMiniDockFrame!=NULL && pMiniDockFrame->
			IsKindOf(RUNTIME_CLASS(CMiniDockFrameWnd)))
		{
			CMenu* pPopup=pMiniDockFrame->GetSystemMenu(FALSE);
			while(pPopup->DeleteMenu(0,MF_BYPOSITION));
		}
	}
}

void COXMenuBar::OnChangeDockSide(DWORD dwDockSide)
{
	UNREFERENCED_PARAMETER(dwDockSide);
	if(!IsFloating() && GetParentFrame()!=NULL)
		GetParentFrame()->RecalcLayout();
}

LONG COXMenuBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	// refresh icon if any is displayed
	UpdateIcon(TRUE);

	COXCoolToolBar::OnIdleUpdateCmdUI(wParam,lParam);

	return 0L;
}

LONG COXMenuBar::OnDisplayPopupMenu(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	MSG msg;
	// handle only the last one
	while(::PeekMessage(&msg,GetSafeHwnd(),
		WM_DISPLAYPOPUPMENU,WM_DISPLAYPOPUPMENU,PM_REMOVE))
	{
		wParam=msg.wParam;
	}

	ASSERT(wParam>=0 && wParam<=(WPARAM)::GetMenuItemCount(m_hMenu));

	DisplayPopupMenu(wParam);

	return 0;
}

void COXMenuBar::OnUpdateMenuItem(CCmdUI* pCmd)
{
	// if there is nothing to update
	if(m_hMenu==NULL) 
		return;

	CMenu* pMenu=CMenu::FromHandle(m_hMenu);
	ASSERT(pMenu!=NULL);
	if(pMenu==NULL) 
		return;

	// enable menu items
	if(pCmd->m_nID-ID_CMDBASE<pMenu->GetMenuItemCount())
		pCmd->Enable((pMenu->GetMenuState(pCmd->m_nID-ID_CMDBASE,
			MF_BYPOSITION)&(MF_DISABLED|MF_GRAYED))==0);
}

BOOL COXMenuBar::OnMenuDropDown(NMHDR* pNotifyStruct, LRESULT* result)
{
	NMTOOLBAR* pNMTB=(NMTOOLBAR*)pNotifyStruct;

	// don't call it twice for forbidden item
	if(m_nForbiddenItem!=pNMTB->iItem-ID_CMDBASE)
		PostMessage(WM_DISPLAYPOPUPMENU,pNMTB->iItem-ID_CMDBASE);
	else
		m_nForbiddenItem=-1;

	*result=TBDDRET_DEFAULT;

	return FALSE;
}

#if _MSC_VER < 1400
UINT 
#else
LRESULT
#endif
 COXMenuBar::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRect rect;
	GetWindowRect(rect);
	CPoint ptTest=point;
	ptTest-=rect.TopLeft();

	// handle areas where icon and close, restore, minimize buttons
	// are located in a specific way
	if(GetIcon()!=NULL && (m_iconRect.PtInRect(ptTest) || 
		m_rectCloseBtn.PtInRect(ptTest) || m_rectRestoreBtn.PtInRect(ptTest) || 
		m_rectMinimizeBtn.PtInRect(ptTest)))
		return HTCAPTION;

	return CToolBar::OnNcHitTest(point);
}

void COXMenuBar::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	UNREFERENCED_PARAMETER(nHitTest);

	CRect rect;
	GetWindowRect(rect);
	CPoint ptTest=point;
	ptTest-=rect.TopLeft();

	ASSERT(GetIcon()!=NULL && (m_iconRect.PtInRect(ptTest) || 
		m_rectCloseBtn.PtInRect(ptTest) || m_rectRestoreBtn.PtInRect(ptTest) || 
		m_rectMinimizeBtn.PtInRect(ptTest)));

	if(GetIcon()==NULL)
		return;

	// handle mouse click over icon and close, restore, minimize buttons
	//

	m_pressedBtn=NONE;

	MENUBARHITTEST hitTest=(MENUBARHITTEST)HitTest(ptTest);

	if(hitTest==ICON)
		PostMessage(WM_DISPLAYPOPUPMENU,::GetMenuItemCount(m_hMenu));
	else if(hitTest==CLOSEBTN || hitTest==RESTOREBTN || hitTest==MINIMIZEBTN)
	{
		SetCapture();
		m_pressedBtn=hitTest;
		RedrawToolBar(FALSE,TRUE);
	}
}

void COXMenuBar::OnNcLButtonDblClk(UINT nHitTest, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	// handle mouse double click over icon
	//
	if(GetIcon() != NULL)
	{
	    BOOL bMaximize=FALSE;
		CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->
			MDIGetActive(&bMaximize);
		ASSERT(pChild!=NULL && bMaximize);
		if(pChild==NULL || !bMaximize)
		{
			COXCoolToolBar::OnNcLButtonDblClk(nHitTest, point);
			return;
		}

		int hitTest=HitTest(point,SCREEN);
		if(hitTest==ICON)
			pChild->PostMessage(WM_SYSCOMMAND,SC_CLOSE);
	}

	COXCoolToolBar::OnNcLButtonDblClk(nHitTest, point);
}


void COXMenuBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if(IsFloating())
	{
		CToolBarCtrl& tool=GetToolBarCtrl();
		tool.PressButton(ID_CMDBASE,FALSE);
	}
	COXCoolToolBar::OnLButtonDown(nFlags,point);
	return;
}


void COXMenuBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	// handle mouse click close, restore, minimize buttons
	//

	if(GetIcon()==NULL || ::GetCapture()!=GetSafeHwnd())
	{
		COXCoolToolBar::OnLButtonUp(nFlags,point);
		return;
	}

	::ReleaseCapture();

    BOOL bMaximize=FALSE;
	CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->MDIGetActive(&bMaximize);
	ASSERT(pChild!=NULL && bMaximize);
	if(pChild==NULL || !bMaximize)
	{
		COXCoolToolBar::OnLButtonUp(nFlags,point);
		return;
	}

	CRect rect;
	GetWindowRect(rect);
	CPoint ptTest=point;
	ClientToScreen(&ptTest);
	ptTest-=rect.TopLeft();

	MENUBARHITTEST hitTest=(MENUBARHITTEST)HitTest(ptTest);

	// send corresponding messages

	if(m_pressedBtn==hitTest)
	{
		if(m_pressedBtn==CLOSEBTN)
			pChild->PostMessage(WM_SYSCOMMAND,SC_CLOSE);
		else if(m_pressedBtn==RESTOREBTN)
			pChild->PostMessage(WM_SYSCOMMAND,SC_RESTORE);
		else if(m_pressedBtn==MINIMIZEBTN)
			pChild->PostMessage(WM_SYSCOMMAND,SC_MINIMIZE);
	}

	m_pressedBtn=NONE;

	RedrawToolBar(FALSE,TRUE);
}


void COXMenuBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if(GetIcon()==NULL || ::GetCapture()!=GetSafeHwnd())
	{
		COXCoolToolBar::OnMouseMove(nFlags, point);
		return;
	}

	CPoint ptTest=point;

	// handle mouse cursor movement over iclose, restore, minimize buttons
	//

	int hitTest=HitTest(ptTest,CLIENT);
	hitTest=hitTest>=0 ? NONE : hitTest;

	if(m_pressedBtn!=hitTest)
	{
		m_pressedBtn=(MENUBARHITTEST)hitTest;
		RedrawWindow(NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_FRAME);
	}

	COXCoolToolBar::OnMouseMove(nFlags, point);
}


void COXMenuBar::OnKillFocus(CWnd* pNewWnd) 
{
	COXCoolToolBar::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here

	// reset the surfing mode
	m_bIsInSurfingMode=FALSE;

}

void COXMenuBar::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	UNREFERENCED_PARAMETER(uFlags);
	UNREFERENCED_PARAMETER(lpszSection);

	UpdateMenuMetrics();

	COXCoolToolBar::OnSettingChange(uFlags, lpszSection);
}

////////////////////////////////////////////////////////////////////

CSize COXMenuBar::CalcLayout(DWORD dwMode, int nLength)
{
	CSize sizeResult=COXCoolToolBar::CalcLayout(dwMode,nLength);

	// adjust the size of menu bar if it should take the entire 
	// row/column
	if(m_bTakeEntireRow)
	{
		CSize size=GetEntireRow((dwMode & LM_HORZ));
		if((dwMode&LM_HORZDOCK) || (dwMode&LM_VERTDOCK) || m_bForceEntireRow)
		{
			if(dwMode & LM_HORZ)
				sizeResult.cx=size.cy-size.cx;
			else
				sizeResult.cy=size.cy-size.cx;
		}

	}

	return sizeResult;
}

CSize COXMenuBar::GetEntireRow(const BOOL bHorz) const
{
	CWnd* pWndFrame=AfxGetMainWnd();

	CSize sizeResult(0,0);
	CRect rect;
	pWndFrame->GetClientRect(rect);
	if(bHorz)
	{
		sizeResult.cx=-afxData.cxBorder2;
		sizeResult.cy=rect.Width()+afxData.cxBorder2;
	}
	else
	{
		sizeResult.cx=-afxData.cyBorder2;
		sizeResult.cy=rect.Height()+afxData.cyBorder2;
	}

	if(pWndFrame->IsKindOf(RUNTIME_CLASS(CFrameWnd)))
	{
		// find corresponding dock bar 

		CRect rectDockBar;
		CWnd* pDockBar;

		if(bHorz)
		{
			pDockBar=((CFrameWnd*)pWndFrame)->
				GetControlBar(AFX_IDW_DOCKBAR_TOP);
			if(pDockBar!=NULL)
				pDockBar=((CFrameWnd*)pWndFrame)->
					GetControlBar(AFX_IDW_DOCKBAR_BOTTOM);
		}
		else
		{
			pDockBar=((CFrameWnd*)pWndFrame)->
				GetControlBar(AFX_IDW_DOCKBAR_LEFT);
			if(pDockBar!=NULL)
				pDockBar=((CFrameWnd*)pWndFrame)->
					GetControlBar(AFX_IDW_DOCKBAR_RIGHT);
		}

		if(pDockBar!=NULL)
		{
			pDockBar->GetWindowRect(rectDockBar);
			pDockBar->ScreenToClient(rectDockBar);
			if(bHorz)
			{
				sizeResult.cx=-afxData.cxBorder2;
				sizeResult.cy=rectDockBar.right+afxData.cyBorder2;
			}
			else
			{
				sizeResult.cx=-afxData.cxBorder2;
/*				if(m_bForceEntireRow)
					sizeResult.cy=16383;
				else*/
					sizeResult.cy=rectDockBar.bottom+afxData.cyBorder2;
			}
		}
	}

	return sizeResult;
}


// global handle of the window that had focus before the menu bar
// was set in the surfing mode
HWND g_hWndHadFocus=NULL;

BOOL COXMenuBar::SetSurfingMode(const BOOL bIsInSurfingMode/*=TRUE*/, 
								const int nItem/*=0*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));
	ASSERT(nItem==-1 || (nItem>=0 && nItem<GetToolBarCtrl().GetButtonCount()));

	if(bIsInSurfingMode==m_bIsInSurfingMode)
	{
		ASSERT(!m_bIsInSurfingMode || !IsInDisplayingMode());
		ASSERT((m_bIsInSurfingMode && ::GetFocus()==m_hWnd) || 
			(!m_bIsInSurfingMode && ::GetFocus()!=m_hWnd));
		// just reset the hot item
		if(m_bIsInSurfingMode)
			SendMessage(TB_SETHOTITEM,nItem);
		return TRUE;
	}

	// double check
	if(bIsInSurfingMode && IsInDisplayingMode())
	{
		TRACE(_T("COXMenuBar::SetSurfingMode: Cannot be set in the surfing mode while displaying popup menus\n"));
		return FALSE;
	}

	if(bIsInSurfingMode)
	{
		// save thae handle of the window that had focus
		g_hWndHadFocus=::GetFocus();
		SetFocus();
		// reset hot item
		SendMessage(TB_SETHOTITEM,nItem);
	}
	else		
	{
		g_hWndHadFocus=::IsWindow(g_hWndHadFocus) ? g_hWndHadFocus : NULL;
		// reset focus window
		::SetFocus(g_hWndHadFocus);
		g_hWndHadFocus=NULL;
		// reset hot item
		SendMessage(TB_SETHOTITEM,(WPARAM)-1);
	}

	m_bIsInSurfingMode=bIsInSurfingMode;

	return TRUE;
}

void COXMenuBar::BookSpace(CRect& rectBookedSpace, DWORD dwMode) 
{
	// book some space for close, restore, minimize buttons
	if(dwMode&LM_HORZ)
		rectBookedSpace.right=CalcButtonRects();
	else
		rectBookedSpace.bottom=CalcButtonRects();
}

void COXMenuBar::DrawInBookedSpace(CDC& dc, CRect& rectToDrawIn)
{
	if(GetIcon()!=NULL && m_pFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
		// recalculate close, restore, minimize buttons rectangles
		CalcButtonRects();

		// and draw them
		DrawButtons(dc);

		// update the rectangle that can be used by parent window to draw 
		// additional elements
		if(m_dwStyle&CBRS_ORIENT_HORZ)
			rectToDrawIn.right=(!m_rectMinimizeBtn.IsRectEmpty() ? 
				m_rectMinimizeBtn.left : (!m_rectRestoreBtn.IsRectEmpty() ? 
				m_rectRestoreBtn.left : m_rectCloseBtn.left))-6;
		else
			rectToDrawIn.bottom=(!m_rectMinimizeBtn.IsRectEmpty() ? 
				m_rectMinimizeBtn.top : (!m_rectRestoreBtn.IsRectEmpty() ? 
				m_rectRestoreBtn.top : m_rectCloseBtn.top))-6;
	}
}

BOOL COXMenuBar::SetMenu(const HMENU hMenu)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	if(m_hMenu==hMenu)
		return TRUE;

	m_hMenu=hMenu;

	// repopulate menubar
	return RecreateMenuBar();
}

BOOL COXMenuBar::RecreateMenuBar(BOOL bRedraw/*=TRUE*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// delete existing buttons
	CToolBarCtrl& tool=GetToolBarCtrl();
	while(tool.DeleteButton(0));
	// clean up accelerator table
	m_accelTable.RemoveAll();


	// work out currently active MDIChild window icon (only for MDI applications)
	UpdateIcon(FALSE);

	BOOL bResult=TRUE;

	if(m_hMenu!=NULL)
	{

		// create one button for every menu item
		//

		CMenu menu;
		if(!menu.Attach(m_hMenu))
			return FALSE;

		// number of menu items 
		int nItems=menu.GetMenuItemCount();
		CString sMenuItem;
		TBBUTTON* pButtons=new TBBUTTON[nItems]; 

		// add buttons and set text to them
		for(int nIndex=0; nIndex<nItems; nIndex++)
		{
			// item name
			menu.GetMenuString(nIndex,sMenuItem,MF_BYPOSITION);

			// work out accelerator
			int nPos=sMenuItem.Find(_T("&"));
			if(nPos!=-1)
			{
				CString sAccelSymbol=sMenuItem.Mid(nPos+1,1);
				sAccelSymbol.MakeUpper();
				m_accelTable.SetAt((TCHAR)*((LPCTSTR)sAccelSymbol),nIndex);
			}

			// setup button structure
			pButtons[nIndex].iBitmap=-1;
			pButtons[nIndex].idCommand=nIndex+ID_CMDBASE;
			pButtons[nIndex].fsState=TBSTATE_ENABLED;
			pButtons[nIndex].fsStyle=TBSTYLE_BUTTON|TBSTYLE_DROPDOWN|
				TBSTYLE_AUTOSIZE;
			pButtons[nIndex].dwData=nIndex;
			pButtons[nIndex].iString=tool.AddStrings(sMenuItem);
		}

		bResult=tool.AddButtons(nItems,pButtons);
		delete[] pButtons;
		//////////////////////////////////////////////////////////////////////

		VERIFY(menu.Detach()!=NULL);
	}

	if(!m_bIsMenuMetricInitialized)
	{
		UpdateMenuMetrics(FALSE);
		m_bIsMenuMetricInitialized=TRUE;
	}
	if(bRedraw)
	{
		// redraw the toolbar if specified
		if(!IsFloating() && (m_dwStyle & CBRS_ORIENT_HORZ) && GetTakeEntireRow())
			RedrawToolBar(FALSE,FALSE);
		else
			RedrawToolBar(TRUE,TRUE);
		m_bDelayedButtonLayout=TRUE;
	}

	return bResult;
}


void COXMenuBar::UpdateIcon(BOOL bRedraw/*=TRUE*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	// refresh the icon
	HICON hIcon=NULL;
	if(m_pFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)))
	{
	    BOOL bMaximize=FALSE;
		CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->
			MDIGetActive(&bMaximize);
		if(pChild!=NULL && bMaximize)
		{
			hIcon=(HICON)::GetClassLong(pChild->m_hWnd,GCL_HICONSM);
			if(hIcon==NULL)
				hIcon=(HICON)::GetClassLong(pChild->m_hWnd,GCL_HICON);
			if(hIcon==NULL)
			{
				if(!::SendMessageTimeout(pChild->m_hWnd,
					WM_GETICON,ICON_SMALL,0,SMTO_ABORTIFHUNG,1000,
					(LPDWORD)&hIcon) || hIcon==NULL)
				{
					::SendMessageTimeout(pChild->m_hWnd,
						WM_GETICON,ICON_BIG,0,SMTO_ABORTIFHUNG,1000,
						(LPDWORD)&hIcon);
				}
			}
		}
	}

	if(hIcon!=GetIcon())
		SetIcon(hIcon,bRedraw);
}

void COXMenuBar::DisplayPopupMenu(const int nMenuItem)
{
	ASSERT(m_hMenu!=NULL);
	ASSERT((nMenuItem>=0 && nMenuItem<::GetMenuItemCount(m_hMenu)) ||
		(GetIcon()!=NULL && nMenuItem==::GetMenuItemCount(m_hMenu)));

	if(m_hMenu==NULL || nMenuItem<0 || nMenuItem>::GetMenuItemCount(m_hMenu) ||
		(GetIcon()==NULL && nMenuItem==::GetMenuItemCount(m_hMenu)))
		return;

	BOOL bWasInDisplayingMode=IsInDisplayingMode();

	// reset the surfing mode
	SetSurfingMode(FALSE);

	// if sytem menu should be displayed
	BOOL bSysMenu=(nMenuItem==::GetMenuItemCount(m_hMenu));

	CMenu* pPopup=NULL;
	if(bSysMenu)
	{
		// work out system menu
		ASSERT(m_pFrameWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)));
	    BOOL bMaximize=FALSE;
		CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->
			MDIGetActive(&bMaximize);
		ASSERT(pChild!=NULL && bMaximize);
		if(pChild==NULL || !bMaximize)
			return;

		pPopup=pChild->GetSystemMenu(FALSE);
	}
	else
	{
		// work out popup menu
		CMenu* pMenu=CMenu::FromHandle(m_hMenu);
		ASSERT(pMenu!=NULL);
		if(pMenu!=NULL && (pMenu->GetMenuState(nMenuItem,MF_BYPOSITION)&
			(MF_DISABLED|MF_GRAYED))==0)
			pPopup=pMenu->GetSubMenu(nMenuItem);
	}

	if(pPopup==NULL) 
	{
		// make sure the state of previously active button is restored
		RestoreMenuButton();
		return;
	}

	// send WM_INITMENU message. 
	if(!bWasInDisplayingMode)
	{
		m_pFrameWnd->SendMessage(WM_INITMENU,(WPARAM)m_hMenu);
	}
	// send WM_MENUSELECT message
	m_pFrameWnd->SendMessage(WM_MENUSELECT,MAKEWPARAM((bSysMenu ? 0 : nMenuItem),
		((bSysMenu ? MF_SYSMENU : MF_POPUP))),(LPARAM)m_hMenu);

	UINT nFlags=TPM_LEFTBUTTON;

	CRect rect;
	// translate the current toolbar item rectangle into screen coordinates
	// so that we'll know where to pop up the menu
	if(bSysMenu)
	{
		CRect rectWindow;
		GetWindowRect(rectWindow);
		ScreenToClient(&rectWindow);

		CRect rectClient;
		GetClientRect(rectClient);
		
		rect=m_iconRect;
		rect.OffsetRect(rectWindow.left-rectClient.left,
			rectWindow.top-rectClient.top);
	}
	else
		GetItemRect(nMenuItem,&rect);

	ClientToScreen(&rect);

	// adjust the coordinate in order to display the popup window 
	// in the best position
	if((m_dwStyle & CBRS_ORIENT_HORZ))
	{
		if(::GetSystemMetrics(SM_CYSCREEN)-rect.bottom<200)
		{
			nFlags|=TPM_LEFTALIGN;
			rect.left=rect.right+1;
		}
		else
		{
			nFlags|=TPM_LEFTALIGN;
			rect.top=rect.bottom+1;
		}
	}
	else
	{
		if(::GetSystemMetrics(SM_CXSCREEN)-rect.right<200)
		{
			nFlags|=TPM_RIGHTALIGN;
			rect.left--;
		}
		else
		{
			nFlags|=TPM_LEFTALIGN;
			rect.left=rect.right+1;
		}
	}

	// make sure the state of previously active button is restored
	RestoreMenuButton();

	// set current active item
	m_nActiveMenuItem=nMenuItem;
	m_nActivateNextItem=-1;

	// set button to pressed state
	CToolBarCtrl& tool=GetToolBarCtrl();
	tool.PressButton(m_nActiveMenuItem+ID_CMDBASE);

	// display popup menu
	CWnd* pWndOwner=AfxGetMainWnd();
	pPopup->TrackPopupMenu(nFlags,rect.left,rect.top,pWndOwner);

	// while deactivating popup menu the user could have set to
	// activate another menu item. In this case just post the 
	// corresponding message
	if(m_nActivateNextItem!=-1)
		PostMessage(WM_DISPLAYPOPUPMENU,m_nActivateNextItem);
	else
	{
		// Check whether we are still on the original button
		CPoint cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(&cursorPos);
		int hitPos=tool.HitTest(&cursorPos);
		if(hitPos==m_nActiveMenuItem)
			m_nForbiddenItem=m_nActiveMenuItem;  // Don't allow it to drop down again
		RestoreMenuButton();
	}

}

void COXMenuBar::RestoreMenuButton()
{
	// set button to unpressed state
	if(m_nActiveMenuItem!=-1)
	{
		CToolBarCtrl& tool=GetToolBarCtrl();
		tool.PressButton(m_nActiveMenuItem+ID_CMDBASE,FALSE);
		m_nActiveMenuItem=-1;
	}
}


int COXMenuBar::CalcButtonRects()
{
	m_rectCloseBtn.SetRectEmpty();
	m_rectRestoreBtn.SetRectEmpty();
	m_rectMinimizeBtn.SetRectEmpty();

	// total width of buttons
	int nButtonsWidth=0;

	if(GetIcon()!=NULL)
	{
		ASSERT(::IsWindow(m_pFrameWnd->m_hWnd));

	    BOOL bMaximize=FALSE;
		CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->
			MDIGetActive(&bMaximize);
		if(pChild==NULL || !bMaximize)
			return nButtonsWidth;

		nButtonsWidth=2;

		DWORD dwStyle=pChild->GetStyle();

		// buttons metrics
		int cxIcon=GetSystemMetrics(SM_CXSIZE);
		int cyIcon=GetSystemMetrics(SM_CYSIZE);

		BOOL bHorz=(m_dwStyle&CBRS_ORIENT_HORZ); 

		CRect rectToDrawIn;
		GetWindowRect(rectToDrawIn);
		rectToDrawIn-=rectToDrawIn.TopLeft();

		// center the button images depending on orientation
		int nOffset=0;
		if(bHorz)
		{
			if(rectToDrawIn.Height()<cyIcon)
			{
				cyIcon=rectToDrawIn.Height();
				cxIcon=cyIcon;
			}
			else
				nOffset=(rectToDrawIn.Height()-cyIcon)/2+
					(rectToDrawIn.Height()-cyIcon)%2;
		}
		else
		{
			if(rectToDrawIn.Width()<cxIcon)
			{
				cxIcon=rectToDrawIn.Width();
				cyIcon=cxIcon;
			}
			else
				nOffset=(rectToDrawIn.Width()-cxIcon)/2+
					(rectToDrawIn.Width()-cxIcon)%2;
		}

		// flags that specify if correponding buttons are displayed
		BOOL bCloseBox=TRUE;
		BOOL bMaxBox=bCloseBox & 
			((dwStyle&WS_MAXIMIZEBOX)!=0 || (dwStyle&WS_MINIMIZEBOX)!=0);

		// Calculate caption buttons. 
		CRect rect;
		if(bHorz)
			rect=CRect(rectToDrawIn.right-cxIcon-nButtonsWidth,nOffset, 
				rectToDrawIn.right-nButtonsWidth,nOffset+cyIcon);
		else
			rect=CRect(nOffset,rectToDrawIn.bottom-cxIcon-nButtonsWidth,
				nOffset+cxIcon,rectToDrawIn.bottom-nButtonsWidth);
	
		if(bCloseBox)
		{
			// Close box has a 2 pixel border on all sides but left/top, 
			// which is zero
			if(bHorz)
				rect.DeflateRect(0,2,2,2);
			else
				rect.DeflateRect(2,0,2,2);
			m_rectCloseBtn=rect;
			nButtonsWidth+=bHorz ? cxIcon : cyIcon;
		}

		// Max/restore button is like close box; just shift rectangle left
		if(bMaxBox) 
		{
			ASSERT(bCloseBox);

			rect-=bHorz ? CPoint(cxIcon,0) : CPoint(0,cyIcon);
			m_rectRestoreBtn=rect;
			nButtonsWidth+=bHorz ? cxIcon : cyIcon;
		}

		// Minimize button has 2 pixel border on all sides but right.
		if(bMaxBox) 
		{
			ASSERT(bCloseBox);

			rect-=bHorz ? CPoint(cxIcon-2,0) : CPoint(0,cyIcon-2);
			m_rectMinimizeBtn=rect;
			nButtonsWidth+=bHorz ? cxIcon : cyIcon;
		}

		nButtonsWidth+=ID_BUTTONSGAP;
	}

	return nButtonsWidth;
}


void COXMenuBar::DrawButtons(CDC& dc)
{
	ASSERT(::IsWindow(m_pFrameWnd->m_hWnd));

    BOOL bMaximize=FALSE;
	CMDIChildWnd* pChild=((CMDIFrameWnd*)m_pFrameWnd)->
		MDIGetActive(&bMaximize);
	if(pChild==NULL || !bMaximize)
		return;

	ASSERT(!m_rectCloseBtn.IsRectEmpty());
	if(m_rectCloseBtn.IsRectEmpty())
		return;

	DWORD dwStyle=pChild->GetStyle();

	UINT nPushState;

	// close button
	nPushState=(m_pressedBtn==CLOSEBTN) ? DFCS_PUSHED : 0;
	dc.DrawFrameControl(&m_rectCloseBtn,DFC_CAPTION,
		DFCS_CAPTIONCLOSE|nPushState);

	// Max/restore button
	if(!m_rectRestoreBtn.IsRectEmpty())
	{
		UINT nState=DFCS_CAPTIONRESTORE;
		if((dwStyle&WS_MAXIMIZEBOX)==0)
			nState|=DFCS_INACTIVE;
		nPushState=(m_pressedBtn==RESTOREBTN) ? DFCS_PUSHED : 0;
		dc.DrawFrameControl(&m_rectRestoreBtn,DFC_CAPTION,nState|nPushState);
	}

	// Minimize button
	if(!m_rectMinimizeBtn.IsRectEmpty())
	{
		UINT nState=DFCS_CAPTIONMIN;
		if((dwStyle&WS_MINIMIZEBOX)==0)
			nState|=DFCS_INACTIVE;
		nPushState=(m_pressedBtn==MINIMIZEBTN) ? DFCS_PUSHED : 0;
		dc.DrawFrameControl(&m_rectMinimizeBtn,DFC_CAPTION,nState|nPushState);
	}
}

int COXMenuBar::HitTest(const CPoint& ptTest, 
						const HITTESTORIGIN Origin/*=ZEROBASED*/) const
{
	CPoint point=ptTest;

	CRect rect;

	// update the test point depending on origin
	switch(Origin)
	{
	case ZEROBASED:
		break;
	case CLIENT:
		GetWindowRect(rect);
		ScreenToClient(rect);
		point-=rect.TopLeft();
		break;
	case SCREEN:
		GetWindowRect(rect);
		point-=rect.TopLeft();
		break;
	}

	int hitTest;
	if(m_iconRect.PtInRect(point))
		hitTest=ICON;
	else if(m_rectCloseBtn.PtInRect(point))
		hitTest=CLOSEBTN;
	else if(m_rectRestoreBtn.PtInRect(point))
		hitTest=RESTOREBTN;
	else if(m_rectMinimizeBtn.PtInRect(point))
		hitTest=MINIMIZEBTN;
	else
	{
		// find corresponding menu item
		//

		hitTest=NONE;

		CRect rectClient;
		GetClientRect(rectClient);
		rectClient-=rect.TopLeft();
		if(rectClient.PtInRect(point))
		{
			GetWindowRect(rect);
			ScreenToClient(rect);
			point+=rect.TopLeft();

			CToolBarCtrl& tool=GetToolBarCtrl();
			for(int nIndex=0; nIndex<tool.GetButtonCount(); nIndex++)
			{
				tool.GetItemRect(nIndex,rect);
				if(rect.PtInRect(point))
				{
					hitTest=nIndex;
					break;
				}
			}
		}
	}

	return hitTest;
}


BOOL COXMenuBar::HandleMenuKey(UINT nChar)
{
	// handle keyboard messages
	//

	if(AfxGetMainWnd()->SendMessage(WM_KEYDOWN,nChar,1))
		return TRUE;
	
	ASSERT(::IsWindow(m_hWnd));

	BOOL bHandled=FALSE;

	if(nChar==0)
	{
		// ALT key was pressed
		ASSERT(!IsInDisplayingMode());
		int nHotItem=SendMessage(TB_GETHOTITEM);
		nHotItem=nHotItem==-1 ? 0 : nHotItem;
		// set the menu  bar in surfing mode when user presses ALT key
		SetSurfingMode(!IsInSurfingMode(),nHotItem);
		bHandled=TRUE;
	}
	else
	{
		int nItemCount=GetToolBarCtrl().GetButtonCount();
		if(nItemCount==0)
			return FALSE;

		// handle keys  depending on their values, menu bar
		// orientation sand the mode it is currently in
		switch(nChar)
		{
		case VK_LEFT:
			if(m_dwStyle & CBRS_ORIENT_HORZ)
			{
				if(IsInSurfingMode())
				{
					// in surfing mode just navigate through menu items
					int nItem=SendMessage(TB_GETHOTITEM);
					// retrieve the item that preceedes the current hot one
					nItem=GetPrevMenuItem(nItem);
					// set it to hot state
					SendMessage(TB_SETHOTITEM,
						(nItem>=0 && nItem<nItemCount) ? nItem : -1);
					bHandled=TRUE;
				}
				else if(IsInDisplayingMode())
				{
					// in displaying mode we activate the preceeding menu item
					int nItem=GetPrevMenuItem(m_nActiveMenuItem);
					if(nItem!=m_nActiveMenuItem)
					{
						m_nActivateNextItem=nItem;

						// emulate mouse click 
						//
						CRect rect;
						if(GetIcon()!=NULL && nItem==nItemCount)
						{
							GetToolBarCtrl().GetItemRect(0,rect);
							rect.DeflateRect(1,1);
							rect.OffsetRect(-::GetSystemMetrics(SM_CXSMICON),0);
						}
						else
						{
							GetToolBarCtrl().GetItemRect(nItem,rect);
						}
						CPoint point(rect.left+rect.Width()/2,
							rect.top+rect.Height()/2);
						PostMessage(WM_LBUTTONDOWN,MK_LBUTTON,
							MAKELPARAM(point.x,point.y));
						PostMessage(WM_LBUTTONUP,MK_LBUTTON,
							MAKELPARAM(point.x,point.y));
						//////////////////////////////
						bHandled=TRUE;
					}
				}
			}
			else
			{
				if(IsInSurfingMode())
				{
					// in surfing mode activate the currently hot item
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=nItem==-1 ? nItemCount : nItem;
					DisplayPopupMenu(nItem);
					bHandled=TRUE;
				}
			}
			break;
		
		case VK_RIGHT:
			if(m_dwStyle & CBRS_ORIENT_HORZ)
			{
				if(IsInSurfingMode())
				{
					// in surfing mode just navigate through menu items
					int nItem=SendMessage(TB_GETHOTITEM);
					// retrieve the item that followss the current hot one
					nItem=GetNextMenuItem(nItem);
					// set it to hot state
					SendMessage(TB_SETHOTITEM,
						(nItem>=0 && nItem<nItemCount) ? nItem : -1);
					bHandled=TRUE;
				}
				else if(IsInDisplayingMode())
				{
					// in displaying mode we activate the following menu item
					int nItem=GetNextMenuItem(m_nActiveMenuItem);
					if(nItem!=m_nActiveMenuItem)
					{
						m_nActivateNextItem=nItem;

						// emulate mouse click 
						//
						CRect rect;
						if(GetIcon()!=NULL && nItem==nItemCount)
						{
							GetToolBarCtrl().GetItemRect(0,rect);
							rect.DeflateRect(1,1);
							rect.OffsetRect(-::GetSystemMetrics(SM_CXSMICON),0);
						}
						else
						{
							GetToolBarCtrl().GetItemRect(nItem,rect);
						}
						CPoint point(rect.left+rect.Width()/2,
							rect.top+rect.Height()/2);
						PostMessage(WM_LBUTTONDOWN,MK_LBUTTON,
							MAKELPARAM(point.x,point.y));
						PostMessage(WM_LBUTTONUP,MK_LBUTTON,
							MAKELPARAM(point.x,point.y));
						////////////////////////////
						bHandled=TRUE;
					}
				}
			}
			else
			{
				if(IsInSurfingMode())
				{
					// in surfing mode activate the currently hot item
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=nItem==-1 ? nItemCount : nItem;
					DisplayPopupMenu(nItem);
					bHandled=TRUE;
				}
			}
			break;

		case VK_UP:
			if(m_dwStyle & CBRS_ORIENT_VERT)
			{
				if(IsInSurfingMode())
				{
					// in surfing mode just navigate through menu items
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=GetPrevMenuItem(nItem);
					SendMessage(TB_SETHOTITEM,
						(nItem>=0 && nItem<nItemCount) ? nItem : -1);
					bHandled=TRUE;
				}
			}
			else
			{
				if(IsInSurfingMode())
				{
					// in surfing mode activate the currently hot item
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=nItem==-1 ? nItemCount : nItem;
					DisplayPopupMenu(nItem);
					bHandled=TRUE;
				}
			}
			break;
		case VK_DOWN:
			if(m_dwStyle & CBRS_ORIENT_VERT)
			{
				if(IsInSurfingMode())
				{
					// in surfing mode just navigate through menu items
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=GetNextMenuItem(nItem);
					SendMessage(TB_SETHOTITEM,
						(nItem>=0 && nItem<nItemCount) ? nItem : -1);
					bHandled=TRUE;
				}
			}
			else
			{
				if(IsInSurfingMode())
				{
					// in surfing mode activate the currently hot item
					int nItem=SendMessage(TB_GETHOTITEM);
					nItem=nItem==-1 ? nItemCount : nItem;
					DisplayPopupMenu(nItem);
					bHandled=TRUE;
				}
			}
			break;
		case VK_RETURN:
			if(IsInSurfingMode())
			{
				// in surfing mode activate the currently hot item
				int nItem=SendMessage(TB_GETHOTITEM);
				nItem=nItem==-1 ? nItemCount : nItem;
				DisplayPopupMenu(nItem);
				bHandled=TRUE;
			}
			break;
		default:
			if(!IsInDisplayingMode())
			{
				// try to find the matching key in accelerator table
				int nMenuItem=-1;
				TCHAR sHotKey=(TCHAR)nChar;
                _tcsupr(&sHotKey);
				if(m_accelTable.Lookup(sHotKey,nMenuItem))
				{
					ASSERT(nMenuItem>=0 && 
						nMenuItem<GetToolBarCtrl().GetButtonCount());
					// if found, activate it
					DisplayPopupMenu(nMenuItem);
					bHandled=TRUE;
				}
			}
		}
	}

	return bHandled;
}


int COXMenuBar::GetPrevMenuItem(int nItem, 
								const BOOL bEnsureVisible/*=TRUE*/) const
{
	int nItemCount=GetToolBarCtrl().GetButtonCount();

	nItem=nItem==-1 ? ((GetIcon()!=NULL) ? nItemCount : 0) : nItem;
	if(GetIcon()!=NULL)
		nItem=nItem==0 ? nItemCount : nItem-1;
	else
		nItem=nItem==0 ? nItemCount-1 : nItem-1;

	if(bEnsureVisible && nItem!=nItemCount)
	{
		// make sure the button rectangle is within menubar client area
		//
		CRect rectClient;
		GetClientRect(rectClient);
		CRect rectItem;
		GetItemRect(nItem,rectItem);
		if(rectItem.left>rectClient.right)
			nItem=GetIcon()!=NULL ? nItemCount : 0;
	}

	return nItem;
}

int COXMenuBar::GetNextMenuItem(int nItem, 
								const BOOL bEnsureVisible/*=TRUE*/) const
{
	int nItemCount=GetToolBarCtrl().GetButtonCount();

	nItem=nItem==-1 ? ((GetIcon()!=NULL) ? nItemCount : 0) : nItem;
	if(GetIcon()!=NULL)
		nItem=nItem==nItemCount ? 0 : nItem+1;
	else
		nItem=nItem==nItemCount-1 ? 0 : nItem+1;

	if(bEnsureVisible && nItem!=nItemCount)
	{
		// make sure the button rectangle is within menubar client area
		//
		CRect rectClient;
		GetClientRect(rectClient);
		CRect rectItem;
		GetItemRect(nItem,rectItem);
		if(rectItem.left>rectClient.right)
			nItem=GetIcon()!=NULL ? nItemCount : 0;
	}

	return nItem;
}


void COXMenuBar::UpdateMenuMetrics(BOOL bRedraw/*=TRUE*/)
{
	ASSERT(::IsWindow(GetSafeHwnd()));

	if((HFONT)m_fontMenu!=NULL)
		m_fontMenu.DeleteObject();
	// Menu font, height and color
	NONCLIENTMETRICS ncm={ sizeof(ncm) };
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(ncm),&ncm,0);
	VERIFY(m_fontMenu.CreateFontIndirect(&ncm.lfMenuFont));
	SetFont(&m_fontMenu,TRUE);

	if(GetToolBarCtrl().GetButtonCount()>0)
	{
		GetToolBarCtrl().AutoSize();
		CRect rect;
		GetToolBarCtrl().GetItemRect(0,&rect);
#if _MFC_VER<=0x0421
		SetHeight(rect.Height()+6);
#else
		SetHeight(rect.Height()+3);
#endif
		GetToolBarCtrl().SetButtonSize(rect.Size());
	}

	SetDefaultTextColor(::GetSysColor(COLOR_MENUTEXT));

	if(bRedraw)
		// redraw the toolbar if specified
		RedrawToolBar();
}
///////////////////////////////////////////////////////////////////////////


#endif	//	_MFC_VER>=0x0420





