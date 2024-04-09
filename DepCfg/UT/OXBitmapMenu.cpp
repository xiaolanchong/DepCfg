// ==========================================================================
//				Class Implementation : COXBitmapMenu
// ==========================================================================

// Source file : OXBitmapMenu.cpp

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXBitmapMenu.h"
#include "OXBitmapMenuOrganizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(COXBitmapMenu, CMenu)

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

int COXBitmapMenu::m_nGapLeftBitmap = 2;
int COXBitmapMenu::m_nGapBitmapText = 4;	
int COXBitmapMenu::m_nGapTextAcclrtr = 2;	
int COXBitmapMenu::m_nGapAcclrtrRight = 3;
int COXBitmapMenu::m_nGapVertText = 4;	
int COXBitmapMenu::m_nGapVertBitmap = 3;	

// Data members-------------------------------------------------------------
//	CMap<TCHAR,TCHAR,int,int> m_KeyAccessMap
//	--- holds the mapping from keyboard access char to command ID
//	int m_nBitmapExtent
//	--- holds the largest width of all the bitmaps in the menu
//	int m_nTextHeight;
//	--- holds the height of the text
//	int m_nAcceleratorExtent;
//	--- Holds the largest width of the all the accelerator texts of menu items
//	int m_nStringExtent;
//	--- Holds the largest width of the texts of menu items
//	CList<COXItemInfo*,COXItemInfo*> m_ItemInfoList;
//	--- Maintains a list of item infos just for cleaning purpose
//		while destructing the object

COXBitmapMenu::COXBitmapMenu()
	:
	m_nBitmapExtent(0),
	m_nTextHeight(0),
	m_nAcceleratorExtent(0),
	m_nStringExtent(0)
{	
}

COXBitmapMenu::~COXBitmapMenu()
{
	m_KeyAccessMap.RemoveAll();
	COXItemInfo* pItemInfo;
	while(!m_ItemInfoList.IsEmpty())
	{
		pItemInfo = m_ItemInfoList.RemoveHead();
		delete pItemInfo;
	}
}


void COXBitmapMenu::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// ... It must be a menu we have to draw
	ASSERT(lpDrawItemStruct != NULL);
	ASSERT(lpDrawItemStruct->CtlType == ODT_MENU);

	UINT nState = lpDrawItemStruct->itemState;

	CRect itemRect(lpDrawItemStruct->rcItem);
	CRect buttonRect(0, 0, 0, 0);
	CRect imageRect(0, 0, 0, 0);
	CRect text1Rect(0, 0, 0, 0);
	CRect text2Rect(0, 0, 0, 0);

	COXItemInfo* pItemInfo = (COXItemInfo*)lpDrawItemStruct->itemData;
	ASSERT(AfxIsValidAddress(pItemInfo, sizeof(COXItemInfo)));
	COXImageInfo* pImageInfo = pItemInfo->GetImageInfo();
	CString sText = pItemInfo->GetText();

	// Compute the space for each menu item part
	DistributeSpace(nState, pImageInfo, itemRect, buttonRect, imageRect, text1Rect, text2Rect);

#ifdef _DEBUG
	// Make sure every rect is within the itemRect
	CRect intersectRect;
	intersectRect.IntersectRect(itemRect, buttonRect);
	ASSERT(buttonRect.IsRectEmpty() || intersectRect.EqualRect(buttonRect));
	intersectRect.IntersectRect(itemRect, imageRect);
	ASSERT(imageRect.IsRectEmpty() || intersectRect.EqualRect(imageRect));
	intersectRect.IntersectRect(itemRect, text1Rect);
	ASSERT(text1Rect.IsRectEmpty() || intersectRect.EqualRect(text1Rect));
	intersectRect.IntersectRect(itemRect, text2Rect);
	ASSERT(text2Rect.IsRectEmpty() || intersectRect.EqualRect(text2Rect));
#endif // _DEBUG

	// Draw evry part of the menu item
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	DrawBackground(&dc, nState, pImageInfo, itemRect, buttonRect);
	DrawButton(&dc, nState, pImageInfo, buttonRect);
	DrawImage(&dc, nState, pImageInfo, imageRect);
	DrawText(&dc, nState, sText, text1Rect, text2Rect);

	dc.Detach(); 
}


void COXBitmapMenu::DistributeSpace(UINT /* nState */, 
									COXImageInfo* pImageInfo, 
									CRect itemRect, 
									CRect& buttonRect, 
									CRect& imageRect, 
									CRect& text1Rect, 
									CRect& text2Rect)
{
	// Compute the image and button rectangle
	if(pImageInfo != NULL)
	{
		// Get the actual bitmap rect
		IMAGEINFO ii;
		::ZeroMemory(&ii, sizeof(ii));
		pImageInfo->GetImageList()->GetImageInfo(pImageInfo->GetIndex(), &ii);
		
		// Center it vertically in the item rect
		CRect bitmapRect =  ii.rcImage;
		bitmapRect -= bitmapRect.TopLeft();
		int nVertGap = (itemRect.Height() - bitmapRect.Height()) / 2;

		ASSERT(bitmapRect.Width() <= itemRect.Width());
		ASSERT(bitmapRect.Height() <= itemRect.Height());

		imageRect = bitmapRect;
		imageRect += itemRect.TopLeft();
		imageRect += CPoint(m_nGapLeftBitmap, nVertGap);

		// Compute the button rectangle
		buttonRect = imageRect;
		buttonRect.InflateRect(2, 2);
	}

	// Compute the rectangle for the text in front of the tab 
	text1Rect = itemRect;
	text1Rect.left = m_nGapLeftBitmap + m_nBitmapExtent + m_nGapBitmapText;
	text1Rect.right = text1Rect.left + m_nStringExtent;

	// Compute the rectangle for the text after the tab (shortcut key)
	text2Rect = itemRect;
	text2Rect.right -= m_nGapAcclrtrRight;
	text2Rect.left = text2Rect.right - m_nAcceleratorExtent;
	int nGapTextHorz = (text2Rect.left - text1Rect.right) / 2;
	text2Rect -= CPoint(nGapTextHorz, 0);

	// Center the text vertically
	int nGapTextVert = (itemRect.Height() - m_nTextHeight) / 2;
	text1Rect.InflateRect(0, -nGapTextVert);
	text2Rect.top = text1Rect.top;
	text2Rect.bottom = text1Rect.bottom;
}

void COXBitmapMenu::DrawBackground(CDC* pDC, UINT nState, COXImageInfo* pImageInfo,
								   CRect itemRect, CRect buttonRect)
{
	BOOL bSelected = nState & ODS_SELECTED;
	BOOL bChecked = nState & ODS_CHECKED;

	COLORREF clrBackground = ::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_MENU); 

	// Draw the image on a special background depending on selection, check state
	// and Default image or not
	COLORREF clrBtnBackground = ::GetSysColor(bSelected ? 
						(bChecked ? COLOR_MENU : 
						(pImageInfo->IsDefaultImgList() ? COLOR_HIGHLIGHT : COLOR_MENU)) :
						(bChecked ? COLOR_3DHILIGHT : COLOR_MENU)); 
	pDC->FillSolidRect(&buttonRect, clrBtnBackground);

	itemRect.left = __max(buttonRect.right, m_nGapLeftBitmap + m_nBitmapExtent + 
		(!bChecked ? (pImageInfo->IsDefaultImgList() ? -1 : 1) : 2));
	
	itemRect.bottom -= 1;
	pDC->FillSolidRect(&itemRect, clrBackground);
}

void COXBitmapMenu::DrawButton(CDC* pDC, UINT nState,  COXImageInfo* pImageInfo, 
							   CRect buttonRect)
{
	BOOL bSelected = nState & ODS_SELECTED;
	BOOL bDisabled = nState & ODS_DISABLED;
	BOOL bChecked = nState & ODS_CHECKED;

	// Draw edge around the image according to the state of the item
	// (The background has already been filled with the right color)
	if (bSelected && !bDisabled && !bChecked && !pImageInfo->IsDefaultImgList())
		pDC->DrawEdge(buttonRect, BDR_RAISEDINNER, BF_RECT);
	else if(bChecked)
		pDC->DrawEdge(buttonRect, BDR_SUNKENOUTER, BF_RECT);
}

void COXBitmapMenu::DrawImage(CDC* pDC, UINT nState, COXImageInfo* pImageInfo, 
							  CRect imageRect)
{
	BOOL bDisabled=nState & ODS_DISABLED;
	BOOL bChecked=nState & ODS_CHECKED;
	BOOL bSelected=nState & ODS_SELECTED;

	if (pImageInfo->IsDefaultImgList() && !bChecked)
		// The default checkmark is not drawn when it isn't checked
		return;

	int nIndex=pImageInfo->GetIndex();
	if(bSelected && pImageInfo->GetHotIndex()!=-1)
		nIndex=pImageInfo->GetHotIndex();

	if (!bDisabled)
	{
		pImageInfo->GetImageList()->Draw(pDC, nIndex, 
			imageRect.TopLeft(), ILD_NORMAL|ILD_TRANSPARENT); 
	}
	else
	{
		HICON hIcon = pImageInfo->GetImageList()->ExtractIcon(nIndex);
		pDC->DrawState(imageRect.TopLeft(), imageRect.Size(), hIcon, 
			DSS_DISABLED, (HBRUSH)NULL);
	}
}

void COXBitmapMenu::DrawText(CDC* pDC, UINT nState, 
							 CString sText, CRect text1Rect, CRect text2Rect)
{
	BOOL bSelected = nState & ODS_SELECTED;
	BOOL bDisabled = nState & ODS_DISABLED;

	// Split the text (in front of and after the tab)
	int nTabCharIndex = sText.Find('\t');
	CString sAfterTab;
	CString sBeforeTab;
	if(nTabCharIndex != -1)
	{
		sBeforeTab = sText.Left(nTabCharIndex);
		sAfterTab = sText.Mid(nTabCharIndex + 1);
	}
	else
	{
		sBeforeTab = sText;
		ASSERT(sAfterTab.IsEmpty());
	}

	// Draw the text
	// Sets the foreground color of the text based on its selection state
	COLORREF clrForeground = ::GetSysColor(bDisabled ? COLOR_GRAYTEXT : 
		(bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_MENUTEXT)); 
	COLORREF clrOldForeground = pDC->SetTextColor(clrForeground);


	// Sets the background color of text based on selection state
	COLORREF clrBackground = ::GetSysColor(bSelected ? COLOR_HIGHLIGHT : COLOR_MENU); 
	COLORREF clrOldBackground = pDC->SetBkColor(clrBackground);

	UINT nFlags = 0;
	CRect newText2Rect(0, 0, 0, 0);
	int nHorzAlign = 0;
	if (bDisabled && !bSelected)
		nFlags |= DSS_DISABLED;

	pDC->DrawState(text1Rect.TopLeft(), text1Rect.Size(), sBeforeTab.GetBuffer(0),
		nFlags, TRUE, sBeforeTab.GetLength(), (CBrush*)NULL);

	// ... First calculate text width so we can right-align it
	pDC->DrawText(sAfterTab, newText2Rect, DT_CALCRECT | DT_SINGLELINE);
	nHorzAlign = text2Rect.Width() - newText2Rect.Width();
	if(!sAfterTab.IsEmpty())
	{
		pDC->DrawState(text2Rect.TopLeft() + CPoint(nHorzAlign, 0), 
			newText2Rect.Size(), sAfterTab.GetBuffer(0),
			nFlags, TRUE, sAfterTab.GetLength(), (CBrush*)NULL);
	}

	pDC->SetTextColor(clrOldForeground); 
	pDC->SetBkColor(clrOldBackground); 
}

void COXBitmapMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	ASSERT(lpMeasureItemStruct != NULL);
	COXItemInfo* pItemInfo = (COXItemInfo*)lpMeasureItemStruct->itemData;
	ASSERT(AfxIsValidAddress(pItemInfo, sizeof(COXItemInfo)));
	COXImageInfo* pImageInfo = pItemInfo->GetImageInfo();

	int nImageWidth = 0;
	int nImageHeight = 0;
	if (pImageInfo != NULL)
	{
		IMAGEINFO ii;
		pImageInfo->GetImageList()->GetImageInfo(pImageInfo->GetIndex(),&ii);
		nImageWidth = ii.rcImage.right - ii.rcImage.left + 2;
		nImageHeight = ii.rcImage.bottom - ii.rcImage.top + 2;
	}
	
	lpMeasureItemStruct->itemWidth = m_nGapLeftBitmap + m_nBitmapExtent + m_nGapBitmapText + 
		m_nStringExtent +  (m_nAcceleratorExtent ? m_nGapTextAcclrtr : 0) + 
		m_nAcceleratorExtent + m_nGapAcclrtrRight;
	lpMeasureItemStruct->itemHeight = __max(nImageHeight + m_nGapVertBitmap, 
		m_nTextHeight + m_nGapVertText); 
}

void COXBitmapMenu::CalcExtents()
// --- In  : 
// --- Out : 
// --- Returns :
// --- Effect : Calculates the largest extents of bitsmaps, menu texts, and accelerator texts
{
	int nItemCount = GetMenuItemCount();

	CString sText;
	CString sBeforeTab;
	CString sAfterTab;
	int nTabCharIndex = 0;

	MENUITEMINFO mii;
	::ZeroMemory(&mii, sizeof(mii));
	mii.cbSize = sizeof(mii);

	CFont Font;
	NONCLIENTMETRICS ncm;
	::ZeroMemory(&ncm, sizeof(ncm));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);

	COXItemInfo* pItemInfo = NULL;
	COXImageInfo* pImageInfo = NULL;
	IMAGEINFO ii;
	::ZeroMemory(&ii, sizeof(ii));

	VERIFY(SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0));
	VERIFY(Font.CreateFontIndirect(&ncm.lfMenuFont));

	CWnd* pMainWnd = AfxGetThread()->GetMainWnd();
	CDC* pDC = pMainWnd->GetDC();
	CFont* pOldFont = pDC->SelectObject(&Font);

	CSize TextExt;
	for(int nItemIndex = 0; nItemIndex < nItemCount; nItemIndex++)
	{
		mii.fMask = MIIM_TYPE | MIIM_DATA | MIIM_SUBMENU;
		mii.cch = 300;
		mii.dwTypeData = sText.GetBuffer(mii.cch);
		// ... zero-terminate string
		mii.dwTypeData[0] = _T('\0');
		::GetMenuItemInfo(GetSafeHmenu(), nItemIndex, TRUE, &mii);
		sText.ReleaseBuffer();

		if(mii.fType & MFT_SEPARATOR)
			continue;

		pItemInfo = (COXItemInfo*)(mii.dwItemData);
		pImageInfo = pItemInfo->GetImageInfo();
		if(pImageInfo)
		{
			pImageInfo->GetImageList()->GetImageInfo(pImageInfo->GetIndex(),&ii);
			m_nBitmapExtent = __max(m_nBitmapExtent,ii.rcImage.right-ii.rcImage.left);
		}
		sText = pItemInfo->GetText();

		nTabCharIndex = sText.Find('\t');
		if(nTabCharIndex != -1)
		{
			sBeforeTab = sText.Left(nTabCharIndex);
			sAfterTab = sText.Mid(nTabCharIndex + 1);
		}
		else
		{
			sBeforeTab = sText;
			sAfterTab = "";
		}

		CRect text1Rect(0, 0, 0, 0);
		CRect text2Rect(0, 0, 0, 0);
		pDC->DrawText(sBeforeTab, text1Rect, DT_CALCRECT | DT_SINGLELINE);
		pDC->DrawText(sAfterTab, text2Rect, DT_CALCRECT | DT_SINGLELINE);

		m_nStringExtent = __max(m_nStringExtent, text1Rect.Width());
		m_nAcceleratorExtent = __max(m_nAcceleratorExtent, text2Rect.Width());
	}

	TextExt = pDC->GetTextExtent("A");
	m_nTextHeight = TextExt.cy;

	pDC->SelectObject(pOldFont);
	pMainWnd->ReleaseDC(pDC);
}

void COXBitmapMenu::AddItemInfo(COXItemInfo* pItemInfo)
{
	m_ItemInfoList.AddTail(pItemInfo);
}

/////////////////////////////////////////////////////////////////////////////
// COXBitmapMenu idle update through CBitmapMenuCmdUI class

class OX_CLASS_DECL CBitmapMenuCmdUI : public CCmdUI        // class private to this file !
{
public: // re-implementations only
	virtual void SetText(LPCTSTR lpszText);
};

void CBitmapMenuCmdUI::SetText(LPCTSTR lpszText)
{
	ASSERT(lpszText != NULL);
	ASSERT(AfxIsValidString(lpszText));
	
	ASSERT(m_pMenu->IsKindOf(RUNTIME_CLASS(COXBitmapMenu)));
	if (m_pMenu != NULL)
	{
		if (m_pSubMenu != NULL)
			return; // don't change popup menus indirectly

		MENUITEMINFO mii;
		::ZeroMemory(&mii, sizeof(mii));
		mii.cbSize = sizeof(mii);
		mii.fMask = MIIM_TYPE | MIIM_DATA;
		::GetMenuItemInfo(m_pMenu->GetSafeHmenu(), m_nIndex, TRUE, &mii);

		if (mii.fType == MFT_OWNERDRAW)
		{
			COXItemInfo* pItemInfo = (COXItemInfo*)(mii.dwItemData);
			ASSERT(pItemInfo != NULL);
			pItemInfo->SetText(lpszText);
		}
		else
			CCmdUI::SetText(lpszText);
	}
}

void COXBitmapMenu::OnUpdateCmdUI(CWnd* pWnd, UINT /* nIndex */, BOOL bSysMenu)
{
	// Code almost entirely copied from CFrameWnd::OnInitPopup
	if (bSysMenu)
		return;     // don't support system menu

	// check the enabled state of various menu items

	CBitmapMenuCmdUI state;
	state.m_pMenu = this;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == m_hMenu)
		state.m_pParentMenu = this;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(pWnd->m_hWnd)) != NULL)
	{
		CWnd* pParent = pWnd->GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(pWnd, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, ((CFrameWnd*)pWnd)->m_bAutoMenuEnable && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}