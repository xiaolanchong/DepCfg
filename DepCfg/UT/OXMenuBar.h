// ==========================================================================
//						   Class Specification : 
//				COXMenuBar & COXMenuDockBar & COXMenuBarFrame
// ==========================================================================

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                          
// //////////////////////////////////////////////////////////////////////////

/*

Many products (Misrosoft Office, Visual Studio IDE to name just a few) 
use special replacement for standard menu bar in their frame window. 
It looks like common dockable flat toolbar while implements all the
functionality found in standard menu bar (accelerators, mouse and 
keyboard navigation). 

Due to similarity to standard toolbar control we thought it would be natural 
to try to use this control in order to implement the menubar.

But we have to note that only using toolbar control that is available
in comctl32.dll v4.71 and higher would allow us to implement the 
functionality of menubar to full extent (primarily, because new button
style TBSTYLE_AUTOSIZE, which allows to use buttons of different width 
in the toolbar, was introduced only in v4.71). As long as comctl32.dll
was freely redistributable we felt like it wasn't a big deal. 

Then, we already had the COXCoolToolBar class in our library that could be
good base class for menubar implementation. 

So we developed new class that is called COXMenuBar and is based on
COXCoolToolBar. If you try to use it with old version of comctl32.dll
we will throw unsupported exception.


The problem with replacing standard window menu with dockable menubar
had to be resolved in tight cooperation between frame window and 
menubar control. The solution should have worked with any CFrameWnd
derived window (CFrameWnd had to be used because it is support
docking functionality). The problem turned out to be more complicated
due to the fact that we had to provide our own CDockBar derived class
(COXMenuDockBar) in order to implement menu bar (it has to do with docking 
functionality). CDockBar is undocumented MFC class that is used 
internaly to provide docking functionality for any CControlBar derived
class within CFrameWnd derived framework window. 

Moreover, implementation for SDI and MDI main frame window was a little
bit different in both cases. And anyway it was highly possible to assume 
that programmers could use their own CFrameWnd derivates in their 
applications (e.g. they could have used our COXMDIFrameWndSizeDock class
in order to provide docking windows functionality).

So, we had major problem: implementation that will work with any 
CFrameWnd derived class. 

And we came up with the following solution.
We designed template class COXMenuBarFrame that defined as follows:


	template<class PARENTFRAME, class PARENTDOCKBAR>
	class COXMenuBarFrame : public PARENTFRAME
	{
		. . . . . . . . . . . . . . 
	};


where	PARENTFRAME is the name of the parent frame class (e.g. CFrameWnd)
		PARENTDOCKBAR is the name of parent dock bar class (e.g. CDockBar)

Using this approach you can define your mainframe window as:

	class CMainFrame : public COXMenuBarFrame<CMDIFrameWnd,CDockBar>

in order to implement standard MDI application with automatic menu bar. 
That's all what you have to do in order to include in your application 
support for menubar. We would like to note that in standard macros
like IMPLEMENT_DYNAMIC or BEGIN_MESSAGE_MAP you have to eplicitly use 
the name of the parent frame class:

	BEGIN_MESSAGE_MAP(CMainFrame,CMDIFrameWnd)


Although, COXMenuBarFrame, COXMenuDockBar and COXMenuBar classes define
a number of public functions you wouldn't normally use them because
the functionality that we provide is self sufficient and doesn't provide
any additional customization functionality.


*/

#ifndef _MENUBAR_H
#define _MENUBAR_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#ifndef __AFXOLE_H__
#include <afxole.h>
#define __AFXOLE_H__
#endif

#ifndef _OX_OLEIMPL2__
#include "..\src\oleimpl2.h"
#define _OX_OLEIMPL2__
#endif

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H__
#endif


#if _MFC_VER>=0x0420

#include "OXCoolToolBar.h"

// default ID for menubar
#define AFX_IDW_MENUBAR			0xE810

// message used to notify menubar that popup menu for the item 
// which index is specified as WPARAM must be displayed
#define WM_DISPLAYPOPUPMENU		WM_APP+267

// base id for menubar buttons that correspond to the index
#define ID_CMDBASE				1
// the gap between close, restore and minimize buttons and menu items
#define ID_BUTTONSGAP			6

// hit test values
typedef enum _tagMENUBARHITTEST
{
	ICON=-5,
	CLOSEBTN=-4,
	RESTOREBTN=-3,
	MINIMIZEBTN=-2,
	NONE=-1
} MENUBARHITTEST;
//////////////////////

// test point origin
typedef enum _tagHITTESTORIGIN
{
	CLIENT,
	SCREEN,
	ZEROBASED

} HITTESTORIGIN;
/////////////////////////
	
/////////////////////////////////////////////////////////////////////////////
// COXMenuBar window

class OX_CLASS_DECL COXMenuBar : public COXCoolToolBar
{
	DECLARE_DYNAMIC(COXMenuBar)
// Construction
public:
	COXMenuBar();
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object

// Attributes
public:
	// handle to the menu that is currently displayed in the menu bar
	HMENU m_hMenu;

	// zero-based index of the menu item that is currently displayed
	// in the dropdown state (the corresponding popup menu is active)
	int m_nActiveMenuItem;
	// zero-based index of the menu item that will be activated (the 
	// corresponding popup menu will be active) after the currently 
	// active popup menu is deactivated
	int m_nActivateNextItem;
	// zero-based index of the menu item that cannot be set as 
	// m_nActivateNextItem
	int m_nForbiddenItem;

	BOOL m_bForceEntireRow;

protected:
	// flag that specifies that menu bar will take the entire row or column 
	// while docked. At the moment this flag is always TRUE
	BOOL m_bTakeEntireRow;

	// pointer to the parent frame window
	CFrameWnd* m_pFrameWnd;
	// accelerator table for menu items 
	CMap<TCHAR,TCHAR,int,int> m_accelTable;

	// rectangles for close, restore and minimize buttons on the bar
	CRect m_rectCloseBtn;
	CRect m_rectRestoreBtn;
	CRect m_rectMinimizeBtn;
	////////////////////////////////////////////////////////////////

	// currently pressed button (close, restore or minimize)
	MENUBARHITTEST m_pressedBtn;

	// flag that specifies that menu bar is in surfing mode when user can 
	// use keyboard to navigate through menu items.
	BOOL m_bIsInSurfingMode;

	CFont m_fontMenu;
	BOOL m_bIsMenuMetricInitialized;

// Operations

public:
	
	// --- In  :	pParentWnd	-	Pointer to the window that is the 
	//								menubar's parent. It has to be 
	//								CFrameWnd derived class
	//				dwStyle		-	The menubar style. Refer to decription
	//								of CToolBar::Create() function for details
	//				nID			-	The toolbar's child-window ID.
	// --- Out : 
	// --- Returns:	TRUE if menubar was successfully created, 
	//				or FALSE otherwise
	// --- Effect : Creates the menubar
	BOOL Create(CFrameWnd* pParentWnd, 
		DWORD dwStyle=WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_SIZE_DYNAMIC,
		UINT nID=AFX_IDW_MENUBAR);


	// --- In  :	
	// --- Out : 
	// --- Returns:	Handle to the menu that is used in the menu bar
	// --- Effect : Retrieves handle to the menubar menu
	HMENU GetMenu() const { 
		ASSERT(::IsWindow(GetSafeHwnd()));
		return m_hMenu; 
	}


	// --- In  :	hMenu	-	handle to the valid menu that will be 
	//							displayed in the menubar
	// --- Out : 
	// --- Returns:	TRUE if the menu was successfully set,
	//				or FALSE otherwise
	// --- Effect : Sets menu to be displayed in the menubar
	BOOL SetMenu(const HMENU hMenu);


	// --- In  :	bTakeEntireRow	-	flag that specifies whether the 
	//									menubar will take the entire 
	//									row/column
	//				bRedraw			-	flag that specifies whethern the
	//									menubar will be redrawn or not
	// --- Out : 
	// --- Returns:	
	// --- Effect : Sets the flag that specifies whether the menubar 
	//				will take the entire row/column
	inline void SetTakeEntireRow(const BOOL bTakeEntireRow, 
		const BOOL bRedraw=TRUE) {
		m_bTakeEntireRow=bTakeEntireRow;
		if(bRedraw)
			RedrawToolBar();
	}

	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect : Retrieves the flag that specifies whether the menubar 
	//				will take the entire row/column
	inline BOOL GetTakeEntireRow() const { return m_bTakeEntireRow; }


	// --- In  :	bHorz	-	TRUE if the size of row is retrieved, 
	//							FALSE if the size of column is retrieved
	// --- Out : 
	// --- Returns:	Size of row/column to display the menu bar in
	// --- Effect : Retrieves the size of row/column to display the menu bar in
	CSize GetEntireRow(const BOOL bHorz) const;


	// --- In  :	bIsInSurfingMode	-	flag that specifies whether the 
	//										surfing mode is set/removed.
	//				nItem				-	if bIsInSurfingMode is TRUE then
	//										this variable specifies the index
	//										of the item that has to be 
	//										initially displayed as hot in the
	//										surfing mode.
	// --- Out :	
	// --- Returns:	TRUE if surfing mode was successfully set/removed
	// --- Effect : Sets/removes the surfing mode. If menu bar is in surfing 
	//				mode then user can use keyboard to navigate through menu 
	//				items
	BOOL SetSurfingMode(const BOOL bIsInSurfingMode=TRUE, const int nItem=0);

	// --- In  :	
	// --- Out :	
	// --- Returns:	TRUE if menubar is in surfing mode,	or FALSE otherwise
	// --- Effect : Retrives the flag that specifies whether the menubar is
	//				in the surfing mode or not. If menu bar is in surfing 
	//				mode then user can use keyboard to navigate through 
	//				menu items
	inline BOOL IsInSurfingMode() const { 
		ASSERT(::IsWindow(m_hWnd));
		return m_bIsInSurfingMode; 
	}


	// --- In  :	
	// --- Out :	
	// --- Returns:	TRUE if menubar is in displaying mode, or FALSE otherwise.
	// --- Effect : Retrives the flag that specifies whether the menubar is
	//				in the displaying mode or not. If menu bar is in 
	//				displaying mode then one of the popup menus is displayed
	inline BOOL IsInDisplayingMode() const { 
		ASSERT(::IsWindow(m_hWnd));
		return (m_nActiveMenuItem>=0 && 
			m_nActiveMenuItem<=GetToolBarCtrl().GetButtonCount()); 
	}


	// --- In  :	nChar	-	Specifies the virtual-key code of the given key.
	// --- Out :	
	// --- Returns:	TRUE if key was handled, or FALSE otherwise
	// --- Effect : Handles key pressed by user (accelerator key or 
	//				navigate keys)
	BOOL HandleMenuKey(UINT nChar);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

protected:
	// repiopulate the menu bar
	BOOL RecreateMenuBar(BOOL bRedraw=TRUE);
	// refresh the menubar icon if any is displayed
	void UpdateIcon(BOOL bRedraw=TRUE);
	// sets the specified menu item to pressed state and
	// displays the corresponding popup menu
	void DisplayPopupMenu(const int nMenuItem);
	// restores the state of the previously ative menu item
	void RestoreMenuButton();
	// calculates the size of rectangles for close, restore 
	// and minimize buttons
	int CalcButtonRects();
	// draws close, restore and minimize buttons
	virtual void DrawButtons(CDC& dc);

	// function that is called when the size of the menu bar 
	// is requested by dock bar or dock context
	virtual CSize CalcLayout(DWORD nMode, int nLength = -1);

	// returns the index of the previous (only visible if specified)
	// item in the menu bar
	int GetPrevMenuItem(int nItem, const BOOL bEnsureVisile=TRUE) const;
	// returns the index of the next (only visible if specified)
	// item in the menu bar
	int GetNextMenuItem(int nItem, const BOOL bEnsureVisible=TRUE) const;

// Implementation
public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	virtual ~COXMenuBar();

	
	// --- In  :	ptTest	-	point to test
	//				Origin	-	origin of the point. Can be one of the 
	//							following:
	//		
	//							CLIENT		-	ptTest is in client coordinates
	//							SCREEN		-	ptTest is in screen coordinates
	//							ZEROBASED	-	ptTest is in coordinates where 
	//											TopLeft point of menubar window
	//											is the origin point.
	// --- Out :	
	// --- Returns:	the index of the corresponding menu item or one of the 
	//				following values:
	//
	//				ICON		-	ptTest is over the icon rectangle
	//				CLOSEBTN	-	ptTest is over the close button rectangle
	//				RESTOREBTN	-	ptTest is over the restore button rectangle
	//				MINIMIZEBTN	-	ptTest is over the minimize button rectangle
	//				NONE		-	ptTest is in void space
	//
	// --- Effect : Retrieves the item that is under the specified point
	int HitTest(const CPoint& ptTest, 
		const HITTESTORIGIN Origin=ZEROBASED) const;

	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Redraws the menu bar. If a menu bar is changed after 
	//				Windows has created the window, call this function to 
	//				draw the changed menu bar.
	inline void DrawMenuBar() { 
		if(::IsWindow(GetSafeHwnd()))
			RecreateMenuBar(TRUE); 
	}

protected:
	// virtual function for designating some space. We book space for
	// close, restore and minimize buttons.
	virtual void BookSpace(CRect& rectBookedSpace, DWORD dwMode);
	// Draw in Booked space 
	virtual void DrawInBookedSpace(CDC& dc, CRect& rectToDrawIn);

	// function called all the time when menubar changes its state from
	// docking to floating and vise versa
	virtual void OnFloatingDocking(BOOL bFloating);

	void UpdateMenuMetrics(BOOL bRedraw=TRUE);

	// function called everytime when toolbar changes its docking side
	virtual void OnChangeDockSide(DWORD dwDockSide);

	// Generated message map functions
protected:
	// overwrite standard handlers to overcome some problems with MFC
	// standard painting routine
	//{{AFX_MSG(COXMenuBar)
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg
#if _MSC_VER < 1400
		UINT 
#else
		LRESULT
#endif
		OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	//}}AFX_MSG
	afx_msg LONG OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateMenuItem(CCmdUI* pCmd);
	afx_msg BOOL OnMenuDropDown(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg LONG OnDisplayPopupMenu(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};




template<class PARENTDOCKBAR>
class OX_CLASS_DECL COXMenuDockBar : public PARENTDOCKBAR
{
public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXMenuDockBar();


// Operations

	// --- In  :	bStretch	-	Indicates whether the bar should be 
	//								stretched to the size of the frame. 
	//				bHorz		-	Indicates that the bar is horizontally 
	//								or vertically oriented. The bHorz parameter 
	//								is nonzero if the bar is horizontally 
	//								oriented and is 0 if it is vertically 
	//								oriented.
	// --- Out : 
	// --- Returns:	The dock bar size, in pixels, of a CSize object
	// --- Effect : Calculates size of the dock bar
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);

public:
	// --- In  :	
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	~COXMenuDockBar();
};


// constructor
template<class PARENTDOCKBAR>
COXMenuDockBar<PARENTDOCKBAR>::COXMenuDockBar()
{
	// has to be derived from CDockBar or its derivates
	if(!IsKindOf(RUNTIME_CLASS(CDockBar)))
	{
		TRACE(_T("COXMenuDockBar has to be derived from CDockBar or its derivations\n"));
		AfxThrowNotSupportedException();
	}
}

// destructor
template<class PARENTDOCKBAR>
COXMenuDockBar<PARENTDOCKBAR>::~COXMenuDockBar()
{
}

// mostly MFC code
template<class PARENTDOCKBAR>
CSize COXMenuDockBar<PARENTDOCKBAR>::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	// scan all the control bars
	for (int nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		if(pBar!=NULL && ::IsWindow(pBar->m_hWnd) && pBar->IsVisible() && 
			pBar->IsKindOf(RUNTIME_CLASS(COXMenuBar)) &&
			((COXMenuBar*)pBar)->GetTakeEntireRow())
		{
			((COXMenuBar*)pBar)->m_bForceEntireRow=TRUE;
		}
	}

	// call the parent implementation
	CSize sizeFixed=PARENTDOCKBAR::CalcFixedLayout(bStretch,bHorz);

	CPoint pt(-afxData.cxBorder2, -afxData.cyBorder2);

	// get max size
	CSize sizeMax;
	if (!m_rectLayout.IsRectEmpty())
		sizeMax = m_rectLayout.Size();
	else
	{
		CRect rectFrame;
		CFrameWnd* pFrame=GetParentFrame();
		ASSERT(pFrame!=NULL);
		pFrame->GetClientRect(rectFrame);
		pFrame->ClientToScreen(rectFrame);

		for(int nID=AFX_IDW_CONTROLBAR_FIRST; nID<=AFX_IDW_CONTROLBAR_LAST; nID++)
		{
			CControlBar* pBar=m_pDockSite->GetControlBar(nID);

			if(pBar!=NULL && pBar->IsDockBar() && 
				pBar->IsVisible() && !pBar->IsFloating())
			{
				CRect rectBar;
				pBar->GetWindowRect(rectBar);
				if(pBar->GetStyle() & CBRS_TOP)
				{
					rectFrame.top=__max(rectFrame.top,rectBar.bottom);
				}
				else if(pBar->GetStyle() & CBRS_BOTTOM)
				{
					rectFrame.bottom=__min(rectFrame.bottom,rectBar.top);
				}
			}
		}

		sizeMax = rectFrame.Size();
	}

	// prepare for layout
    AFX_SIZEPARENTPARAMS layout;
    layout.hDWP = m_bLayoutQuery ? NULL : 
		::BeginDeferWindowPos(m_arrBars.GetSize());
	
	BOOL bAdjustBars=FALSE;
	CSize sizeOffset(0,0);
	// scan all the control bars
	for (nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		if(pBar!=NULL && ::IsWindow(pBar->m_hWnd) && pBar->IsVisible() && 
			pBar->IsKindOf(RUNTIME_CLASS(COXMenuBar)) &&
			((COXMenuBar*)pBar)->GetTakeEntireRow())
		{
			// get ideal rect for bar
			DWORD dwMode = 0;
			if ((pBar->m_dwStyle & CBRS_SIZE_DYNAMIC) &&
				(pBar->m_dwStyle & CBRS_FLOATING))
				dwMode |= LM_HORZ | LM_MRUWIDTH;
			else if (pBar->m_dwStyle & CBRS_ORIENT_HORZ)
				dwMode |= LM_HORZ | LM_HORZDOCK;
			else
				dwMode |=  LM_VERTDOCK;

			if((dwMode&LM_HORZDOCK) || (dwMode&LM_VERTDOCK))
			{
				// get current rect for bar
				CRect rectBar;
				pBar->GetWindowRect(&rectBar);
				ScreenToClient(&rectBar);

				CSize sizeBar(rectBar.Width(),rectBar.Height());
				CRect rect(pt,sizeBar);

				if(bHorz)
				{
					rect.right=sizeMax.cx+afxData.cxBorder2;
					rect.OffsetRect(0,rectBar.top-rect.top);

					// check if menubar was droped on the same row with other
					// control bar(s)
					if(rectBar.left>rect.left)
					{
						ASSERT(nPos>0);
						int nPrevPos=nPos-1;
						CControlBar* pPrevBar=NULL;
						while(TRUE)
						{
							ASSERT(nPrevPos>=0);
							pPrevBar=GetDockedControlBar(nPrevPos);
							if(pPrevBar==NULL || 
								!::IsWindow(pPrevBar->GetSafeHwnd()) || 
								pPrevBar->IsVisible())
								break;
							nPrevPos--;
						}

						if(pPrevBar!=NULL)
						{
							CRect rectPrevBar;
							pPrevBar->GetWindowRect(&rectPrevBar);
							rect.OffsetRect(0,rectPrevBar.Height()-
								afxData.cyBorder2);
							sizeOffset.cy+=__min(rectBar.Height(),
								rectPrevBar.Height())-afxData.cyBorder2;
							m_arrBars.InsertAt(nPos, (CObject*)NULL);
							nPos++;
						}
					}

					// mark the end of the row
					CControlBar* pNextBar=NULL;
					int nNextPos=nPos+1;
					while(nNextPos<m_arrBars.GetSize())
					{
						pNextBar=GetDockedControlBar(nNextPos);
						if(pNextBar==NULL || !::IsWindow(pNextBar->GetSafeHwnd()) || 
							pNextBar->IsVisible())
							break;
						nNextPos++;
					}

					if(pNextBar!=NULL)
					{
						m_arrBars.InsertAt(nPos+1, (CObject*)NULL);
						CRect rectNextBar;
						pNextBar->GetWindowRect(&rectNextBar);
						sizeOffset.cy+=rectNextBar.Height()-afxData.cyBorder2;
					}
				}
				else
				{
					rect.bottom=sizeMax.cy;
					rect.OffsetRect(rectBar.left-rect.left,0);

					// check if menubar was droped on the same row with other
					// control bar(s)
					if(rectBar.top>rect.top)
					{
						ASSERT(nPos>0);
						int nPrevPos=nPos-1;
						CControlBar* pPrevBar=NULL;
						while(TRUE)
						{
							ASSERT(nPrevPos>=0);
							pPrevBar=GetDockedControlBar(nPrevPos);
							if(pPrevBar==NULL || 
								!::IsWindow(pPrevBar->GetSafeHwnd()) || 
								pPrevBar->IsVisible())
								break;
							nPrevPos--;
						}

						if(pPrevBar!=NULL)
						{
							CRect rectPrevBar;
							pPrevBar->GetWindowRect(&rectPrevBar);
							rect.OffsetRect(rectPrevBar.Width()-afxData.cxBorder2,0);
							sizeOffset.cx+=__min(rectBar.Width(),
								rectPrevBar.Width())-afxData.cxBorder2;
							m_arrBars.InsertAt(nPrevPos+1, (CObject*)NULL);
							nPos++;
						}
					}

					// mark the end of row
					CControlBar* pNextBar=NULL;
					int nNextPos=nPos+1;
					while(nNextPos<m_arrBars.GetSize())
					{
						CControlBar* pNextBar=GetDockedControlBar(nNextPos);
						if(pNextBar==NULL || !::IsWindow(pNextBar->GetSafeHwnd()) || 
							pNextBar->IsVisible())
							break;
						nNextPos++;
					}

					if(pNextBar!=NULL)
					{
						m_arrBars.InsertAt(nPos+1, (CObject*)NULL);
					}
				}

				if(sizeOffset.cx!=0 || sizeOffset.cy!=0)
					bAdjustBars=TRUE;

				if((bHorz && rectBar.Width()!=rect.Width() && 
					!(rectBar.Width()>rect.Width())) || 
					(!bHorz && rectBar.Height()!=rect.Height() && 
					!(rectBar.Height()>rect.Height())))
				{
					AfxRepositionWindow(&layout, pBar->m_hWnd, &rect);
				}
			}
		}
		else if(pBar!=NULL && bAdjustBars)
		{
			// get current rect for bar
			CRect rectBar;
			pBar->GetWindowRect(&rectBar);
			ScreenToClient(&rectBar);
			rectBar+=sizeOffset;
			AfxRepositionWindow(&layout, pBar->m_hWnd, &rectBar);
		}
	}

	if (layout.hDWP == NULL || !::EndDeferWindowPos(layout.hDWP))
		TRACE(_T("Warning: DeferWindowPos failed - low system resources\n"));

	if(bAdjustBars)
	{
		sizeFixed+=sizeOffset;
	}
	sizeFixed=PARENTDOCKBAR::CalcFixedLayout(bStretch,bHorz);


	// scan all the control bars
	for (nPos = 0; nPos < m_arrBars.GetSize(); nPos++)
	{
		CControlBar* pBar = GetDockedControlBar(nPos);
		if(pBar!=NULL && ::IsWindow(pBar->m_hWnd) && pBar->IsVisible() && 
			pBar->IsKindOf(RUNTIME_CLASS(COXMenuBar)) &&
			((COXMenuBar*)pBar)->GetTakeEntireRow())
		{
			((COXMenuBar*)pBar)->m_bForceEntireRow=FALSE;
		}
	}

	return sizeFixed;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
class OX_CLASS_DECL COXMenuBarFrame : public PARENTFRAME
{
public:
	// dock bar control used internally
	typedef COXMenuDockBar<PARENTDOCKBAR> CInternalDockBar;

	// --- In  :	dwDockStyle		-	The desired styles for the control bar. 
	//									menu bar style must be CBRS_ALIGN_XXX 
	//									or 0 only. Refer to the description of 
	//									CControlBar::SetBarStyle function 
	//									for details.
	// --- Out : 
	// --- Returns:
	// --- Effect : Constructs the object
	COXMenuBarFrame(DWORD dwDockStyle=CBRS_ALIGN_ANY, 
		CRuntimeClass* pFloatingFrameClass=RUNTIME_CLASS(CMiniDockFrameWnd));


// Operations

	// --- In  :	dwDockStyle		-	Specifies which sides of the frame 
	//									window can serve as docking sites for 
	//									control bars. Refer to the description
	//									of CFrameWnd::EnableDocking function 
	//									for details.
	// --- Out :	
	// --- Returns:
	// --- Effect : Call this function to enable dockable control bars in a 
	//				frame window. By default, control bars will be docked to a 
	//				side of the frame window in the following order: 
	//				top, bottom, left, right.
	virtual void EnableDocking(DWORD dwDockStyle);


	virtual void RecalcLayout(BOOL bNotify=TRUE);
	

	virtual BOOL LoadFrame(UINT nIDResource, 
		DWORD dwDefaultStyle=WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE,
		CWnd* pParentWnd = NULL, CCreateContext* pContext=NULL);

protected:
	// --- In  :	lpcs		-	A pointer to a Windows CREATESTRUCT 
	//								structure.
	//				pContext	-	A pointer to a CCreateContext structure
	// --- Out :	
	// --- Returns:	Nonzero if successful; otherwise 0
	// --- Effect : 
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	// --- In  :	hMenu		-	valid handle of the menu that will be set
	//								to menu bar
	//				pParentWnd	-	Pointer to the window that is the 
	//								menubar's parent.
	//				dwStyle		-	The menubar style. Refer to decription
	//								of CToolBar::Create() function for details
	//				nID			-	The toolbar's child-window ID.
	// --- Out : 
	// --- Returns:	TRUE if menubar was successfully created, 
	//				or FALSE otherwise
	// --- Effect : Creates the menubar
	virtual BOOL OnCreateMenuBar(HMENU hMenu, CFrameWnd* pFrameWnd, 
		DWORD dwStyle, UINT nID);

// Attributes
public:

protected:
	// Specifies which sides of the frame window can serve as docking 
	// sites for menu bar.
	DWORD m_dwDockStyle;

	// flag that specifies that the frame window is MDIFrame
	BOOL m_bIsMDI;

	// menubar object
	COXMenuBar m_menuBar;

	// address of the window procedure, or a handle representing the address 
	// of the window procedure of MDIClient window
	WNDPROC m_pfnOldMDIClientProc;
	// handle of the old mouse hook procedure
	HHOOK m_pfnOldMouseHookProc;
	// handle of the old keyboard hook procedure
	HHOOK m_pfnOldKeyboardHookProc;

	// default menu of MDIFrame window
	HMENU m_hDefaultMenu;

	// flag that specifies that MDIClient window is subclassed
	BOOL m_bIsMDISubclassed;

	// flag that specifies that currently selected item in popup menu is popup item
	BOOL m_bIsPopupMenuItem;
	// array of currently displayed popup menus
	CArray<HMENU,HMENU> m_arrPopupMenu;

public:
	// --- In  :
	// --- Out : 
	// --- Returns:
	// --- Effect : Destructs the object
	~COXMenuBarFrame();

	
	// --- In  :	hWnd	-	Handle to the window to receive the message. 
	//				msg		-	Specifies the message. 
	//				wp		-	Specifies additional message-specific 
	//							information. The contents of this parameter 
	//							depend on the value of the msg parameter. 
	//				lp		-	Specifies additional message-specific 
	//							information. The contents of this parameter 
	//							depend on the value of the Msg parameter. 
	// --- Out : 
	// --- Returns:	the result of handling of message that was sent to 
	//				MDIClient window
	// --- Effect : Handles all messages that go to MDIClient window
	virtual LONG HandleMDIClientMsg(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);


	// --- In  :	nCode	-	Specifies a code the hook procedure uses 
	//							to determine how to process the mouse message. 
	//							If nCode is less than zero, the rocedure must 
	//							pass the message to the CallNextHookEx function 
	//							without further processing and should return 
	//							the value returned by CallNextHookEx. 
	//				wParam	-	Specifies the identifier of the mouse message. 
	//				lParam	-	Pointer to a MOUSEHOOKSTRUCT structure. 
	// --- Out : 
	// --- Returns:	the result of handling of mouse message
	// --- Effect : Handles all mouse messages in the current thread
	virtual LRESULT HandleMouseMsg(int nCode, WPARAM wParam, LPARAM lParam);

	
	// --- In  :	nCode	-	Specifies a code the hook procedure uses 
	//							to determine how to process the mouse message. 
	//							If nCode is less than zero, the rocedure must 
	//							pass the message to the CallNextHookEx function 
	//							without further processing and should return 
	//							the value returned by CallNextHookEx. 
	//				wParam	-	Specifies the virtual-key code of the key that 
	//							generated the keystroke message
	//				lParam	-	Specifies the repeat count, scan code, 
	//							extended-key flag, context code, previous 
	//							key-state flag, and transition-state flag
	// --- Out : 
	// --- Returns:	the result of handling of keyboard message
	// --- Effect : Handles all keyboard messages in the current thread
	virtual LRESULT HandleKeyboardMsg(int nCode, WPARAM wParam, LPARAM lParam);

	// --- In  :	
	// --- Out : 
	// --- Returns:	reference to COXMenuBar object used to represent 
	//				the menu bar
	// --- Effect :	Retrieves menu bar object
	COXMenuBar& GetMenuBar();

	// --- In  :	
	// --- Out : 
	// --- Returns:	
	// --- Effect :	Redraws menu bar
	virtual void DrawMenuBar();

	// --- In  :	
	// --- Out : 
	// --- Returns:	pointer to the window menu
	// --- Effect :	Retrieves window's menu
	virtual CMenu* GetMenu();

	// --- In  :	pMenu	-	pointer to the new window menu
	// --- Out : 
	// --- Returns:	TRUE id succeeded or FALSE otherwise
	// --- Effect :	Sets window's menu
	virtual BOOL SetMenu(CMenu* pMenu);

protected:
	// --- In  :	msg	-	message ID
	//				wp	-	WPARAM
	//				lp	-	LPARAM
	// --- Out : 
	// --- Returns:	result of message handling. Different for different messages.
	// --- Effect :	Handle all messages that go to the window
	virtual LRESULT WindowProc(UINT msg, WPARAM wp, LPARAM lp);	

	// helper for resizing the menubar
	void CheckMenuBarSize(BOOL bNotify=TRUE);

	// handles context help
	afx_msg void OnContextHelp();
	// handles all messages in context help mode
	BOOL ProcessHelpMsg(MSG& msg, DWORD* pContext);

	// helper function to subclass MDIClient window
	void SubclassMDIClient();

	// subclass window procedure for MDIClient window
	static LONG CALLBACK MDISubclassProc(HWND hWnd, UINT msg, 
		WPARAM wp, LPARAM lp);
	// hook procedure for mouse messages
	static LRESULT CALLBACK MenuMouseHookProc(int nCode, WPARAM wParam, 
		LPARAM lParam);
	// hook procedure for keyboard messages
	static LRESULT CALLBACK MenuKeyboardHookProc(int nCode, WPARAM wParam, 
		LPARAM lParam);
};



// global pointer to the created frame window
static void* g_pMenuBarFrame=NULL;

// analogue of CFrameWnd::dwDockBarMap
static const DWORD g_dwDockBarMap[4][2] =
{
	{ AFX_IDW_DOCKBAR_TOP,      CBRS_TOP    },
	{ AFX_IDW_DOCKBAR_BOTTOM,   CBRS_BOTTOM },
	{ AFX_IDW_DOCKBAR_LEFT,     CBRS_LEFT   },
	{ AFX_IDW_DOCKBAR_RIGHT,    CBRS_RIGHT  },
};



template<class PARENTFRAME, class PARENTDOCKBAR>
COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
COXMenuBarFrame(DWORD dwDockStyle/*=CBRS_ALIGN_ANY*/, 
				CRuntimeClass* pFloatingFrameClass/*=RUNTIME_CLASS(CMiniDockFrameWnd)*/)
{
	// has to be derived from CframeWnd or its derivates
	if(!IsKindOf(RUNTIME_CLASS(CFrameWnd)))
	{
		TRACE(_T("COXMenuBarFrame has to be derived from CFrameWnd or its derivates\n"));
		AfxThrowNotSupportedException();
	}

	// menu bar style must be CBRS_ALIGN_XXX or 0 only
	ASSERT(dwDockStyle==0 || (dwDockStyle & ~CBRS_ALIGN_ANY)==0);
	m_dwDockStyle=dwDockStyle;

	// is it MDI
	m_bIsMDI=IsKindOf(RUNTIME_CLASS(CMDIFrameWnd));

	m_pfnOldMDIClientProc=NULL;
	m_pfnOldMouseHookProc=NULL;
	m_pfnOldKeyboardHookProc=NULL;

	m_hDefaultMenu=NULL;

	m_bIsMDISubclassed=FALSE;

	m_bIsPopupMenuItem=FALSE;

	m_pFloatingFrameClass=pFloatingFrameClass;

	// set global pointer to the frame window
	g_pMenuBarFrame=this;
}

template<class PARENTFRAME, class PARENTDOCKBAR>
COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::~COXMenuBarFrame()
{
	// destroy menu bar
	if(::IsWindow(m_menuBar.GetSafeHwnd()))
		m_menuBar.DestroyWindow();

	// unhook mouse messages
	if(m_pfnOldMouseHookProc!=NULL)
	{
		VERIFY(::UnhookWindowsHookEx(m_pfnOldMouseHookProc));
		m_pfnOldMouseHookProc=NULL;
	}

	// unhook keyboard messages
	if(m_pfnOldKeyboardHookProc!=NULL)
	{
		VERIFY(::UnhookWindowsHookEx(m_pfnOldKeyboardHookProc));
		m_pfnOldKeyboardHookProc=NULL;
	}
}


template<class PARENTFRAME, class PARENTDOCKBAR>
BOOL COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	BOOL bResult=PARENTFRAME::OnCreateClient(lpcs,pContext);

	if(bResult && m_bIsMDI)
		SubclassMDIClient();

	return bResult;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::RecalcLayout(BOOL bNotify/*=TRUE*/)
{
	PARENTFRAME::RecalcLayout(bNotify);
	CheckMenuBarSize(bNotify);
}


template<class PARENTFRAME, class PARENTDOCKBAR>
BOOL COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::LoadFrame(UINT nIDResource, 
														   DWORD dwDefaultStyle/*=WS_OVERLAPPEDWINDOW|FWS_ADDTOTITLE*/,
														   CWnd* pParentWnd/*=NULL*/, 
														   CCreateContext* pContext/*=NULL*/)
{
	if(PARENTFRAME::LoadFrame(nIDResource,dwDefaultStyle,pParentWnd,pContext))
	{
		ASSERT(::IsWindow(GetMenuBar().GetSafeHwnd()));
		m_hMenuDefault=GetMenuBar().GetMenu();
		return TRUE;
	}

	return FALSE;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
BOOL COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
OnCreateMenuBar(HMENU hMenu, CFrameWnd* pFrameWnd, DWORD dwStyle, UINT nID)
{
	ASSERT(hMenu!=NULL);
	ASSERT(pFrameWnd!=NULL);
	ASSERT(::IsWindow(pFrameWnd->m_hWnd));

	if(!m_menuBar.Create(pFrameWnd,dwStyle,nID) || !m_menuBar.SetMenu(hMenu))
		return FALSE;
	else
	{
		m_hMenuDefault=hMenu;
		return TRUE;
	}
}

template<class PARENTFRAME, class PARENTDOCKBAR>
LRESULT COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::WindowProc(UINT msg, 
															   WPARAM wp, 
															   LPARAM lp)
{
	ASSERT(::IsWindow(((CFrameWnd*)this)->m_hWnd));

	switch(msg) 
	{
	// handle the creation of the frame
	case WM_CREATE:
		{
			// call parent implementation
			LRESULT lResult=PARENTFRAME::WindowProc(msg,wp,lp);
			if(lResult!=-1)
			{
				// ther shouldn't be any menu bar created at the moment
				ASSERT(!::IsWindow(m_menuBar.GetSafeHwnd()));

				if(::IsWindow(m_menuBar.GetSafeHwnd()))
					m_menuBar.DestroyWindow();

				// retrieve default menu
				CFrameWnd* pFrameWnd=(CFrameWnd*)this;
				HMENU hMenu=::GetMenu(pFrameWnd->m_hWnd);
				if(hMenu==NULL)
				{
					TRACE(_T("COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::WindowProc: There is no menu associated with frame window\n"));
					return lResult;
				}

				// save default menu
				m_hDefaultMenu=hMenu;

				// remove menu from window
				if(SetMenu(NULL)==0)
					return -1;

				// for SDI frame window it's important to set application
				// main window right now
				if(AfxGetApp()->m_pMainWnd==NULL)
					AfxGetApp()->m_pMainWnd=this;

				// create menu bar and set the menu
				if(!OnCreateMenuBar(hMenu,pFrameWnd,
					WS_CHILD|WS_VISIBLE|CBRS_TOP|CBRS_SIZE_DYNAMIC,
					AFX_IDW_MENUBAR))
				{
					TRACE(_T("COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::WindowProc: failed to initialize COXMenuBar object\n"));
					return -1;
				}
	
				// in order to subclass MDIClient if it's not done yet
				SendMessage(WM_IDLEUPDATECMDUI);

				// setup docking environment
				m_menuBar.EnableDocking(m_dwDockStyle);
				if(m_dwDockStyle!=0)
				{
					EnableDocking(m_dwDockStyle);
					CRect rect(0,0,0,0);
					pFrameWnd->DockControlBar(&m_menuBar,(UINT)0,&rect);
				}
				//////////////////////////////

				// setup hooks for mouse and keyboard messages
				m_pfnOldMouseHookProc=::SetWindowsHookEx(WH_MOUSE,
					MenuMouseHookProc,NULL,::GetCurrentThreadId());
				m_pfnOldKeyboardHookProc=::SetWindowsHookEx(WH_KEYBOARD,
					MenuKeyboardHookProc,NULL,::GetCurrentThreadId());
				//////////////////////////////////////////////
			}

			return lResult;
		}
	case WM_IDLEUPDATECMDUI:
		if(m_bIsMDI)
		{
			m_bIsMDISubclassed=FALSE;
			::SendMessage(((CMDIFrameWnd*)this)->m_hWndMDIClient,WM_NULL,0,0);
			if(!m_bIsMDISubclassed)
			{
				TRACE(_T("COXMenuBarFrame: menubar has been removed out of the list of objects that subclass MDIClient\n"));
				SubclassMDIClient();
				if(::GetMenu(m_hWnd)!=NULL)
					SetMenu(NULL);
			}
		}
		break;
	case WM_INITMENU:
		{
#if _MFC_VER>0x0421
#ifndef _AFX_NO_OLE_SUPPORT
			if(m_pNotifyHook != NULL)
			{
				COleDocObjectItem* pActiveDocObjectItem=
					DYNAMIC_DOWNCAST(COleDocObjectItem,m_pNotifyHook->m_pActiveItem);
				if(pActiveDocObjectItem!=NULL)
				{
					CWnd* pWnd=pActiveDocObjectItem->GetInPlaceWindow();
					ASSERT(pWnd!=NULL);
					pWnd->SendMessage(msg,wp,lp);
				}
			}
#endif	//	_AFX_NO_OLE_SUPPORT
#endif	//	_MFC_VER>0x0421
			m_arrPopupMenu.RemoveAll();
			TRACE(_T("Remove all popup menus\n"));
			break;
		}
	case WM_INITMENUPOPUP:
		{
			m_bIsPopupMenuItem=FALSE;
			m_arrPopupMenu.Add((HMENU)wp);
			TRACE(_T("Add popup menu\n"));

			WORD hiWord=HIWORD(lp);
			WORD loWord=LOWORD(lp);
			if(hiWord==0)
			{
				if(::IsWindow(m_menuBar.GetSafeHwnd()))
				{
					loWord=(WORD)m_menuBar.m_nActiveMenuItem;
				}
			}
			lp=MAKELPARAM(loWord,hiWord);
			
#if _MFC_VER>0x0421
#ifndef _AFX_NO_OLE_SUPPORT
			if(m_pNotifyHook != NULL)
			{
				COleDocObjectItem* pActiveDocObjectItem=
					DYNAMIC_DOWNCAST(COleDocObjectItem,m_pNotifyHook->m_pActiveItem);
				if(pActiveDocObjectItem!=NULL)
				{
					CWnd* pWnd=pActiveDocObjectItem->GetInPlaceWindow();
					ASSERT(pWnd!=NULL);
					pWnd->SendMessage(msg,wp,lp);
					return TRUE;
				}
			}
#endif	//	_AFX_NO_OLE_SUPPORT
#endif	//	_MFC_VER>0x0421
			break;
		}
	case WM_COMMAND:
		{
			WORD loWord=LOWORD(wp);
			if(loWord==ID_CONTEXT_HELP)
			{
				OnContextHelp();
				return 0;
			}

#if _MFC_VER>0x0421
#ifndef _AFX_NO_OLE_SUPPORT
			if(m_pNotifyHook != NULL)
			{
				COleDocObjectItem* pActiveDocObjectItem=
					DYNAMIC_DOWNCAST(COleDocObjectItem,m_pNotifyHook->m_pActiveItem);
				if(pActiveDocObjectItem!=NULL)
				{
					CWnd* pWnd=pActiveDocObjectItem->GetInPlaceWindow();
					ASSERT(pWnd!=NULL);
					if(pWnd->SendMessage(msg,wp,lp))
						return TRUE;
				}
			}
#endif	//	_AFX_NO_OLE_SUPPORT
#endif	//	_MFC_VER>0x0421

			break;
		}
	case WM_MENUSELECT:
		{
			UINT nFlags=HIWORD(wp);
			m_bIsPopupMenuItem=((nFlags&MF_POPUP)!=0 && 
				!(nFlags==0xFFFF && lp==NULL) ? TRUE : FALSE);

#if _MFC_VER>0x0421
#ifndef _AFX_NO_OLE_SUPPORT
			if(m_pNotifyHook!=NULL)
			{
				COleDocObjectItem* pActiveDocObjectItem=
					DYNAMIC_DOWNCAST(COleDocObjectItem,
					m_pNotifyHook->m_pActiveItem);
				if(pActiveDocObjectItem!=NULL)
				{
					CWnd* pWnd=pActiveDocObjectItem->GetInPlaceWindow();
					ASSERT(pWnd!=NULL);
					pWnd->SendMessage(msg,wp,lp);
				}
			}
#endif	//	_AFX_NO_OLE_SUPPORT
#endif	//	_MFC_VER>0x0421

			if((nFlags&MF_HILITE)!=0)
			{
				while(m_arrPopupMenu.GetSize()>0 && 
					m_arrPopupMenu[m_arrPopupMenu.GetSize()-1]!=(HMENU)lp)
				{
					m_arrPopupMenu.RemoveAt(m_arrPopupMenu.GetSize()-1);
					TRACE(_T("Remove popup menu\n"));
				}
			}

			break;
		}
	case WM_SIZE:
		{
			// call parent implementation
			LRESULT lResult=PARENTFRAME::WindowProc(msg,wp,lp);
			if(wp==SIZE_RESTORED)
				RecalcLayout(FALSE);
			return lResult;
		}
	}

	// I don't handle it: pass along
	return PARENTFRAME::WindowProc(msg,wp,lp);
}


template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::CheckMenuBarSize(BOOL bNotify/*=TRUE*/)
{
	COXMenuBar& menuBar=GetMenuBar();
	if(::IsWindow(menuBar.GetSafeHwnd()) && !menuBar.IsFloating())
	{
		CRect rectMenuBar;
		menuBar.GetWindowRect(rectMenuBar);
		BOOL bHorz=(menuBar.m_dwStyle & CBRS_ORIENT_HORZ);
		CControlBar* pDockBar=NULL;
		if(bHorz)
		{
			pDockBar=GetControlBar(AFX_IDW_DOCKBAR_TOP);
			if(pDockBar!=NULL)
				pDockBar=GetControlBar(AFX_IDW_DOCKBAR_BOTTOM);
		}
		else
		{
			pDockBar=GetControlBar(AFX_IDW_DOCKBAR_LEFT);
			if(pDockBar!=NULL)
				pDockBar=GetControlBar(AFX_IDW_DOCKBAR_RIGHT);
		}
		ASSERT(pDockBar!=NULL);
		CRect rectDockBar;
		pDockBar->GetWindowRect(rectDockBar);
		if((bHorz && rectDockBar.Width()!=rectMenuBar.Width()) || 
			(!bHorz && rectDockBar.Height()!=rectMenuBar.Height()))
		{
			PARENTFRAME::RecalcLayout(bNotify);
		}
	}
}


template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::OnContextHelp()
{
	// don't enter twice, and don't enter if initialization fails
	if (m_bHelpMode == HELP_ACTIVE || !CanEnterHelpMode())
		return;

	// don't enter help mode with pending WM_EXITHELPMODE message
	MSG msg;
	if (PeekMessage(&msg, m_hWnd, WM_EXITHELPMODE, WM_EXITHELPMODE,
		PM_REMOVE|PM_NOYIELD))
	{
		return;
	}

	BOOL bHelpMode = m_bHelpMode;
	ASSERT(m_bHelpMode == HELP_INACTIVE || m_bHelpMode == HELP_ENTERING);
	m_bHelpMode = HELP_ACTIVE;

#ifndef _AFX_NO_OLE_SUPPORT
	// allow any in-place active servers to go into help mode
	if (bHelpMode != HELP_ENTERING && m_pNotifyHook != NULL &&
		!m_pNotifyHook->OnContextHelp(TRUE))
	{
		TRACE0("Error: an in-place server failed to enter context help mode.\n");
		m_pNotifyHook->OnContextHelp(FALSE);    // undo partial help mode
		m_bHelpMode = HELP_INACTIVE;
		return;
	}
#endif

	if (bHelpMode == HELP_INACTIVE)
	{
		// need to delay help startup until later
		PostMessage(WM_COMMAND, ID_CONTEXT_HELP);
		m_bHelpMode = HELP_ENTERING;
		return;
	}

	ASSERT(m_bHelpMode == HELP_ACTIVE);

	// display special help mode message on status bar
	UINT nMsgSave = (UINT)SendMessage(WM_SETMESSAGESTRING,
		(WPARAM)AFX_IDS_HELPMODEMESSAGE);
	if (nMsgSave == 0)
		nMsgSave = AFX_IDS_IDLEMESSAGE;

	DWORD   dwContext = 0;
	POINT   point;

	GetCursorPos(&point);
	SetHelpCapture(point, NULL);
	LONG lIdleCount = 0;
	CWinApp* pApp = AfxGetApp();

	while (m_bHelpMode)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!ProcessHelpMsg(msg, &dwContext))
				break;
			ASSERT(dwContext == 0);
		}
		else if (!pApp->OnIdle(lIdleCount++))
		{
			lIdleCount = 0;
			WaitMessage();
		}
	}

	m_bHelpMode = HELP_INACTIVE;
	ReleaseCapture();

	// make sure the cursor is set appropriately
	SetCapture();
	ReleaseCapture();

	// restore original status bar text
	SendMessage(WM_SETMESSAGESTRING, (WPARAM)nMsgSave);

#ifndef _AFX_NO_OLE_SUPPORT
	// tell in-place servers to exit Shift+F1 help mode
	if (m_pNotifyHook != NULL)
		m_pNotifyHook->OnContextHelp(FALSE);
#endif

	if (dwContext != 0)
	{
		if (dwContext == -1)
			SendMessage(WM_COMMAND, ID_DEFAULT_HELP);
		else
			pApp->WinHelp(dwContext);
	}
	PostMessage(WM_KICKIDLE);    // trigger idle update
}


template<class PARENTFRAME, class PARENTDOCKBAR>
BOOL COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::ProcessHelpMsg(MSG& msg, 
																DWORD* pContext)
{
	ASSERT(pContext != NULL);

	if (msg.message == WM_EXITHELPMODE ||
		(msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE))
	{
		PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
		return FALSE;
	}

	CPoint point;
	if ((msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST) ||
		(msg.message >= WM_NCMOUSEFIRST && msg.message <= WM_NCMOUSELAST))
	{
		BOOL bDescendant;
		HWND hWndHit = SetHelpCapture(msg.pt, &bDescendant);
		if (hWndHit == NULL)
			return TRUE;

		BOOL bHitMenuBar=(hWndHit==m_menuBar.GetSafeHwnd());

		if (bDescendant)
		{
			if(bHitMenuBar)
			{
				if (msg.message == WM_LBUTTONDOWN || m_menuBar.IsInDisplayingMode())
				{
					ASSERT(::GetCapture() == m_hWnd);
					ReleaseCapture();
					GetMessage(&msg, NULL, msg.message, msg.message);
					DispatchMessage(&msg);
					GetCursorPos(&point);
					SetHelpCapture(point, NULL);
				}
				else
				{
					// Hit one of our owned windows -- eat the message.
					PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);
					return TRUE;
				}
			}
			else
			{
				return PARENTFRAME::ProcessHelpMsg(msg,pContext);
			}
		}
		else
		{
			// Hit one of our apps windows (or desktop) -- dispatch the message.
			PeekMessage(&msg, NULL, msg.message, msg.message, PM_REMOVE);

			// Dispatch mouse messages that hit the desktop!
			DispatchMessage(&msg);
		}
	}
	else
	{
		return PARENTFRAME::ProcessHelpMsg(msg,pContext);
	}

	return TRUE;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::SubclassMDIClient()
{
	ASSERT(::IsWindow(((CFrameWnd*)this)->m_hWnd));

	if(m_bIsMDI)
	{
		// if MDI application then we have to handle MDIClient messages
		WNDPROC pfnOldMDIClientProc=
			(WNDPROC)::GetWindowLong(((CMDIFrameWnd*)this)->
			m_hWndMDIClient,GWL_WNDPROC);
		if(m_pfnOldMDIClientProc!=pfnOldMDIClientProc)
		{
			m_pfnOldMDIClientProc=pfnOldMDIClientProc;
			::SetWindowLong(((CMDIFrameWnd*)this)->m_hWndMDIClient,GWL_WNDPROC,
				(LONG)MDISubclassProc);
			m_bIsMDISubclassed=TRUE;
		}
	}
}


// dock bars will be created in the order specified by g_dwDockBarMap
// this also controls which gets priority during layout
// this order can be changed by calling EnableDocking repetitively
// with the exact order of priority
template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::EnableDocking(DWORD dwDockStyle)
{
	// must be CBRS_ALIGN_XXX or CBRS_FLOAT_MULTI only
	ASSERT((dwDockStyle & ~(CBRS_ALIGN_ANY|CBRS_FLOAT_MULTI)) == 0);

	ASSERT(m_pFloatingFrameClass!=NULL);
	for (int i = 0; i < 4; i++)
	{
		if (g_dwDockBarMap[i][1] & dwDockStyle & CBRS_ALIGN_ANY)
		{
			CInternalDockBar* pDock = 
				(CInternalDockBar*)GetControlBar(g_dwDockBarMap[i][0]);
			if (pDock == NULL)
			{
				// use our internal implementation of the dock bar
				pDock = new CInternalDockBar;
				if (!pDock->Create(this,
					WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_CHILD|WS_VISIBLE|
					g_dwDockBarMap[i][1], g_dwDockBarMap[i][0]))
				{
					AfxThrowResourceException();
				}
			}
		}
	}
}


template<class PARENTFRAME, class PARENTDOCKBAR>
LONG COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::HandleMDIClientMsg(HWND hWnd, 
																	UINT msg, 
																	WPARAM wp, 
																	LPARAM lp)
{
	// only in MDIFrame
	ASSERT(m_bIsMDI);
	ASSERT(::IsWindow(((CFrameWnd*)this)->m_hWnd));

    switch(msg)
    {
	case WM_MDISETMENU:
		ASSERT(::IsWindow(m_menuBar.GetSafeHwnd()));
		if(NULL==wp)
			wp=(WPARAM)m_hDefaultMenu;
		// set new menu to the menubar
		if(::IsWindow(m_menuBar.GetSafeHwnd()) && 
			::IsWindow(m_menuBar.GetToolBarCtrl().GetSafeHwnd()))
			m_menuBar.SetMenu((HMENU)wp);
	    wp=NULL;
		break;
	case WM_MDIACTIVATE:
	case WM_MDICREATE:
	case WM_MDIDESTROY:
		// the icon of the MDIChild window could have changed
		if(::IsWindow(m_menuBar.GetSafeHwnd()))
			m_menuBar.PostMessage(WM_IDLEUPDATECMDUI);
		break;
	case WM_NULL:
		m_bIsMDISubclassed=TRUE;
		break;
    }

	// handle other messages
    if(m_pfnOldMDIClientProc!=NULL)
        return ::CallWindowProc(m_pfnOldMDIClientProc,hWnd,msg,wp,lp);

    return 0;
}


// last mouse cursor position when hew menubar item was activated 
static CPoint g_ptLast(-1,-1);

template<class PARENTFRAME, class PARENTDOCKBAR>
LRESULT COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::HandleMouseMsg(int nCode, 
																   WPARAM wParam, 
																   LPARAM lParam)
{
	if(nCode<0)
		// have to call the default implementation
	    return ::CallNextHookEx(m_pfnOldMouseHookProc,nCode,wParam,lParam);

	if(!::IsWindow(((CFrameWnd*)this)->m_hWnd))
		return ::CallNextHookEx(m_pfnOldMouseHookProc,nCode,wParam,lParam);

	if(nCode==HC_ACTION && ::IsWindow(m_menuBar.GetSafeHwnd()))
	{
		MOUSEHOOKSTRUCT* pMHS=(MOUSEHOOKSTRUCT*)lParam;
		// there should be an active menu bar at the moment
		if(m_menuBar.IsInDisplayingMode() && 
			pMHS->hwnd!=m_menuBar.GetSafeHwnd())
		{
			CRect rect;
			m_menuBar.GetWindowRect(rect);
			// handle WM_MOUSEMOVE and WM_LBUTTONDOWN messages
			if((wParam==WM_MOUSEMOVE ||	wParam==WM_LBUTTONDOWN) &&
				rect.PtInRect(pMHS->pt) &&  
				::WindowFromPoint(pMHS->pt)==m_menuBar.GetSafeHwnd())
			{
				CPoint point=pMHS->pt;
				m_menuBar.ScreenToClient(&point);
				// test the mouse cursor point
				int hitTest=m_menuBar.HitTest(point,CLIENT);

				if(wParam==WM_MOUSEMOVE && g_ptLast!=point &&
					hitTest!=m_menuBar.m_nActiveMenuItem && 
					(hitTest>=0 || hitTest==ICON) &&
					!(m_menuBar.m_nActiveMenuItem==
					::GetMenuItemCount(m_menuBar.m_hMenu) && 
					hitTest==ICON))
				{
					// set the menu item that will be activated next
					m_menuBar.m_nActivateNextItem=(hitTest!=ICON) ? hitTest : 
						::GetMenuItemCount(m_menuBar.m_hMenu);

					// emulate mouse pressing
					::PostMessage(pMHS->hwnd,WM_LBUTTONDOWN,MK_LBUTTON,
						MAKELPARAM(point.x,point.y));
					::PostMessage(pMHS->hwnd,WM_LBUTTONUP,MK_LBUTTON,
						MAKELPARAM(point.x,point.y));
	
					return 0;
				}
				else if(wParam==WM_LBUTTONDOWN && (hitTest>=0 || hitTest==ICON))
				{
					// set the forbidden item
					m_menuBar.m_nForbiddenItem=(hitTest!=ICON) ? hitTest : 
						::GetMenuItemCount(m_menuBar.m_hMenu);
				}

				g_ptLast=point;
			}
		}
	}

    return ::CallNextHookEx(m_pfnOldMouseHookProc,nCode,wParam,lParam);
}


// flag that ALT was pressed down
static BOOL g_altDownNoOtherKey=FALSE;

template<class PARENTFRAME, class PARENTDOCKBAR>
LRESULT COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
HandleKeyboardMsg(int nCode, WPARAM wParam, LPARAM lParam)
{
	if(nCode<0)
	    return ::CallNextHookEx(m_pfnOldKeyboardHookProc,nCode,wParam,lParam);

	ASSERT(::IsWindow(((CFrameWnd*)this)->m_hWnd));

	if(nCode==HC_ACTION && ::IsWindow(m_menuBar.GetSafeHwnd()) && IsWindowEnabled())
	{
		// handle differently keyboard messages when menubar is in 
		// displaying mode
		if(!m_menuBar.IsInDisplayingMode())
		{
			if(wParam==VK_MENU)
			{
				if(lParam>=0)
				{
					if(!(lParam&(1<<30)))
					{
						g_altDownNoOtherKey=TRUE;
						return 1;
					}
				}
				else if(g_altDownNoOtherKey)
				{
					// ALT key was pressed
					m_menuBar.HandleMenuKey(0);
					g_altDownNoOtherKey=FALSE;
					return 1;
				}
			}

			if(wParam!=VK_MENU && (::GetAsyncKeyState(VK_MENU)<0 || 
				m_menuBar.IsInSurfingMode()))
			{
				g_altDownNoOtherKey=FALSE;
				if((lParam&0x80000000)==0 && int(lParam)>=0 && 
					::GetAsyncKeyState(VK_F4)>=0)
				{
					// handle the key 
					if(m_menuBar.HandleMenuKey(wParam))
						return 1;
				}
			}
		}
		else
		{
			// handle only VK_LEFT and VK_RIGHT in the specific way 
			// while in displaying mode
			if(int(lParam)>=0 && ((wParam==VK_LEFT && m_arrPopupMenu.GetSize()<=1) || 
				(wParam==VK_RIGHT && !m_bIsPopupMenuItem)))
			{
				// handle the key 
				if(m_menuBar.HandleMenuKey(wParam))
					return 1;
			}
		}
	}

    return ::CallNextHookEx(m_pfnOldKeyboardHookProc,nCode,wParam,lParam);
}


template<class PARENTFRAME, class PARENTDOCKBAR>
COXMenuBar& COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::GetMenuBar()
{
	return m_menuBar;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
void COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::DrawMenuBar()
{
	if(::IsWindow(m_menuBar.GetSafeHwnd()))
		m_menuBar.DrawMenuBar();
	else
		PARENTFRAME::DrawMenuBar();
}


template<class PARENTFRAME, class PARENTDOCKBAR>
CMenu* COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::GetMenu()
{
	if(::IsWindow(m_menuBar.GetSafeHwnd()))
		return CMenu::FromHandle(GetMenuBar().GetMenu());
	else
		return PARENTFRAME::GetMenu();
}


template<class PARENTFRAME, class PARENTDOCKBAR>
BOOL COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::SetMenu(CMenu* pMenu)
{
	if(::IsWindow(m_menuBar.GetSafeHwnd()))
		return m_menuBar.SetMenu(pMenu==NULL ? NULL : 
			pMenu->GetSafeHmenu());
	else
		return PARENTFRAME::SetMenu(pMenu);
}


template<class PARENTFRAME, class PARENTDOCKBAR>
LONG CALLBACK COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
MDISubclassProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	ASSERT(g_pMenuBarFrame!=NULL);
	// call COXMenuBarFrame implementation
	if(::IsWindow(((COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>*)
		g_pMenuBarFrame)->m_hWnd))
	{
		return ((COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>*)g_pMenuBarFrame)->
			HandleMDIClientMsg(hWnd,msg,wp,lp);
	}

	return 0;
}


template<class PARENTFRAME, class PARENTDOCKBAR>
LRESULT CALLBACK COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
MenuMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	ASSERT(g_pMenuBarFrame!=NULL);
	// call COXMenuBarFrame implementation
	return ((COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>*)g_pMenuBarFrame)->
		HandleMouseMsg(nCode,wParam,lParam);
}


template<class PARENTFRAME, class PARENTDOCKBAR>
LRESULT CALLBACK COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>::
MenuKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	ASSERT(g_pMenuBarFrame!=NULL);
	// call COXMenuBarFrame implementation
	return ((COXMenuBarFrame<PARENTFRAME,PARENTDOCKBAR>*)g_pMenuBarFrame)->
		HandleKeyboardMsg(nCode,wParam,lParam);
}



#endif	//	_MFC_VER>=0x0420


#endif // _MENUBAR_H

