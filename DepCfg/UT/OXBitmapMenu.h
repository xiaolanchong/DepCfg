// ==========================================================================
// 					Class Specification : COXBitmapMenu
// ==========================================================================

// Header file : OXBitmapMenu.h

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved
                         
// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class 
//	YES	Derived from CMenu

//	NO	Is a Cwnd. (Its a CMenu)
//	NO	Two stage creation 
//	NO	Has a message map
//	NO	Needs a resource 

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
// COXBitmapMenu implements drawing of the menu items

// Remark:
// User doesn't need to use this object directly. COXBitmapMenuOrganizer
// handles creation and destruction of COXBitmapMenu Objects.

// Prerequisites (necessary conditions):
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BITMAPMENU_H
#define BITMAPMENU_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "OXDllExt.h"

#include <afxtempl.h>


class COXImageInfo;
class COXItemInfo;
class COXBitmapMenuOrganizer;

class OX_CLASS_DECL COXBitmapMenu : public CMenu
{

friend class COXBitmapMenuOrganizer;
DECLARE_DYNAMIC(COXBitmapMenu)

//Data members --------------------------------------------------------
public:
	static int m_nGapLeftBitmap;
	// --- The gap between left edge of menu item and bitmap in pixels
	static int m_nGapBitmapText;
	// --- The gap between bitmap and menu text in pixels
	static int m_nGapTextAcclrtr;
	// --- The gap between menu text and accelerator
	static int m_nGapAcclrtrRight;
	// --- The gap between accelerator and right edge of menu item
	static int m_nGapVertText;	
	// --- Extra vertical space on top and bottom of text in pixels
	static int m_nGapVertBitmap;	
	// --- Extra vertical space on top and bottom of bitmap in pixels

protected:
	CMap<TCHAR, TCHAR, int, int> m_KeyAccessMap;
	int m_nBitmapExtent;
	int m_nTextHeight;
	int m_nAcceleratorExtent;
	int m_nStringExtent;
	CList<COXItemInfo*, COXItemInfo*> m_ItemInfoList;

//Member functions-----------------------------------------------------------------------
public:
	COXBitmapMenu();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object Constructions and defaults are initialised

	virtual ~COXBitmapMenu();
	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Cleanup and Object Destruction

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// --- In  : lpDrawItemStruct : Its automatically passed by framework
	// --- Out : 
	// --- Returns :
	// --- Effect : Draws the item, including the bitmap

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	// --- In  : lpMeasureItemStruct : Its automatically passed by framework
	// --- Out : lpMeasureItemStruct : sets the itemsize in lpMeasureItemStruct
	// --- Returns : 
	// --- Effect : 


protected:
	virtual void DistributeSpace(UINT nState, COXImageInfo* pImageInfo, CRect itemRect, 
		CRect& buttonRect, CRect& imageRect, CRect& text1Rect, CRect& text2Rect);
	virtual void DrawBackground(CDC* pDC, UINT nState, COXImageInfo* pImageInfo, CRect itemRect, CRect buttonRect);
	virtual void DrawButton(CDC* pDC, UINT nState,  COXImageInfo* pImageInfo, CRect buttonRect);
	virtual void DrawImage(CDC* pDC, UINT nState, COXImageInfo* pImageInfo, 
		CRect imageRect);
	virtual void DrawText(CDC* pDC, UINT nState, CString sText, CRect text1Rect, 
		CRect text2Rect);
	
	void CalcExtents();
	void AddItemInfo(COXItemInfo* pItemInfo);

	void OnUpdateCmdUI(CWnd* pWnd, UINT nIndex, BOOL bSysMenu);

};

/////////////////////////////////////////////////////////////////////////////
#endif