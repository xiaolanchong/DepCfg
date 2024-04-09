// mysizeba.cpp : implementation file
//
// Docktest demo of Dockable Views functionality

#include "stdafx.h"
//#include "docktest.h"
#include "mysizeba.h"

#define ID_PopupMessage			2000

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListBoxBar
//
// This is a sizeable contorl bar that has a list box and a button on it.
// It is designed to be created dynamically ie you can have more than one 
// of them.

IMPLEMENT_DYNAMIC(CMyListBoxBar, COXSizeControlBar);

CMyListBoxBar::CMyListBoxBar() 
		: COXSizeControlBar( SZBARF_STDMOUSECLICKS | SZBARF_NORESIZEBTN |SZBARF_DLGAUTOSIZE)
{
}

CMyListBoxBar::~CMyListBoxBar()
{
}



BEGIN_MESSAGE_MAP(CMyListBoxBar, COXSizeControlBar)
	//{{AFX_MSG_MAP(CMyListBoxBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyListBoxBar message handlers


//-------------------------------------------------------------------
void CMyListBoxBar::OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags)
// respond to this event as we need to override it
//-------------------------------------------------------------------
{
	UNREFERENCED_PARAMETER(bFloating);
	UNREFERENCED_PARAMETER(flags);
	UNREFERENCED_PARAMETER(cx);
	UNREFERENCED_PARAMETER(cy);

	CRect rect;
	GetClientRect(rect);

	// shrink rectangle if we're docked



	// position the button at the top right of the window
	CRect ButtonRect;
	m_wndListBox.MoveWindow(&rect);
	m_wndListBox.Invalidate(TRUE);
}


//-------------------------------------------------------------------
int CMyListBoxBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
//-------------------------------------------------------------------
{
	if (COXSizeControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	GetClientRect(&rect);
	
	
	// Register a class for the list box without CS_VREDRAW, CS_HREDRAW
	WNDCLASS wc;
	VERIFY(GetClassInfo(AfxGetInstanceHandle(), _T("LISTBOX"), &wc)); 	
	wc.style &= ~(CS_VREDRAW | CS_HREDRAW);
	wc.lpszClassName = _T("_MF_LISTBOX");
	VERIFY(AfxRegisterClass(&wc));
	
	// add some strings to the list box - makes it a bit more interesting

	m_wndListBox.Create( WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
						 WS_VSCROLL | LBS_OWNERDRAWFIXED , 
						 CRect(50 , 50, 250, 250), this, 1 );


/*
	if (!m_wndListBox.CreateEx(0,
                 			_T("_MF_LISTBOX"),
                 			NULL,
     						WS_CHILD | WS_VISIBLE | LBS_NOTIFY |
						 WS_VSCROLL | LBS_OWNERDRAWFIXED , 
	             			rect.left,
                 			rect.top,
      						rect.Width(),
							rect.Height(),
                 			GetSafeHwnd(),
                 			0,
                 			NULL))
	{
		TRACE0("Failed to create list box\n");
		return -1;
	}
*/
	return 0;					
}

BOOL CMyListBoxBar::PreCreateWindow(CREATESTRUCT& cs) 
{
	return COXSizeControlBar::PreCreateWindow(cs);
}

BOOL CMyListBoxBar::Create(CWnd * pParentWnd, const CString& sTitle/*=_T("size bar")*/,
						   const UINT nID/*=ID_LISTBOXBAR*/)
{
	// register a window class for the control bar
	static CString strWndClass;
	if (strWndClass.IsEmpty())
	{
		strWndClass = AfxRegisterWndClass(CS_DBLCLKS);
	}
	CRect rc;
	pParentWnd->GetClientRect(&rc);
	rc.right = rc.left + rc.Width()/4;
	return COXSizeControlBar::Create(strWndClass, sTitle,
		WS_VISIBLE | CBRS_LEFT | WS_CLIPCHILDREN | WS_CHILD,
		rc, pParentWnd, nID);
}



