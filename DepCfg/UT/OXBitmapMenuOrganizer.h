// ==========================================================================
// 					Class Specification : COXBitmapMenuOrganizer
// ==========================================================================

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved                                            

// //////////////////////////////////////////////////////////////////////////

// Properties:
//	NO	Abstract class 
//	YES	Derived from CObject

//	NO	Is a Cwnd.                     
//	NO	Two stage creation 
//	NO	Has a message map
//	NO	Needs a resource 

//	NO	Persistent objects (saveable on disk)      
//	NO	Uses exceptions

// //////////////////////////////////////////////////////////////////////////

// Desciption :         
// COXBitmapMenuOrganizer is the main class that maintains the relation
// between the menu items and corresponding bitmaps to display with menu
// items.

// Remark:
// Each windows having a menu attched to it should contain a 
// COXBitmapMenuOranizer Object

// Prerequisites (necessary conditions):
// OnInitMenuPopup(WM_INITMENUPOPUP), OnMenuChar(WM_MENUCHAR) should be
// handled by the windows to which the menu is attached and passes to the
// OXBitmapMenuOrganizer.

// Note that this class makes use of the TB_GETIMAGELIST message. Therefore this code 
// will only work if you have the new version of the common control dll
// installed (i.e. you have Internet Explorer 3/4 or Office 97 installed)
/////////////////////////////////////////////////////////////////////////////


#ifndef OXBITMAPMENUORGANIZER_H
#define OXBITMAPMENUORGANIZER_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define OX_CLASS_DECL

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#define __AFXTEMPL_H_H
#endif
#include "OXBitmapMenu.h"


class OX_CLASS_DECL COXBitmapMenuOrganizer : public CObject
{

//Data members --------------------------------------------------------

protected :
	CMap<CString,LPCTSTR,CImageList*,CImageList*> m_mapStringToImageList;
	CMap<DWORD,DWORD,COXImageInfo*,COXImageInfo*> m_mapCommandToImageInfo;
	CFrameWnd* m_pFrameWnd;

	HWND	m_hWnd;
	WNDPROC m_pfnSuper;
	static CMap<HWND, HWND, COXBitmapMenuOrganizer*, COXBitmapMenuOrganizer*> m_allMenuOrganizers;

	CImageList	m_DefaultImgList;
	
private :
	CList<COXBitmapMenu*, COXBitmapMenu*> m_BitmapMenuList;

//Member functions-----------------------------------------------------------------------
public :

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object Constructions and defaults are initialised
	COXBitmapMenuOrganizer();

	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object is cleaned up and destructed 
	virtual ~COXBitmapMenuOrganizer();


	// --- In  :
	// --- Out : 
	// --- Returns :
	// --- Effect : Object is cleaned up, but no frame window is detached
	void Empty(BOOL bCopyText = TRUE);

	
	// --- In  : pFrameWnd	-	The frame window which menus will be organized by 
	//							this menu organizer
	// --- Out : 
	// --- Returns :
	// --- Effect : This function must be called before any other action 
	//				can be performed
	BOOL AttachFrameWnd(CFrameWnd* pFrameWnd);

	// --- In  : 
	// --- Out : 
	// --- Returns:	The frame window which menus were being organized by this menu
	//				organizer
	// --- Effect : 
	CFrameWnd* DetachFrameWnd();


	// --- In  :  nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//			  pszBitmapID	-	The resource string of the bitmap whose 
	//								image has to set to the menu item
	//			  nBitmapIndex	-	The index of the image in the 
	//								bitmap to set to the item
	//			  nCx			-	Width of each button in the bitmap
	// --- Out : 
	// --- Returns: returns TRUE if successfull otherwise FALSE
	// --- Effect : Sets the given image in bitmap to the menu item. 
	BOOL SetMenuImage(UINT nCommandID, LPCTSTR pszBitmapID, 
		int nBitmapIndex, int nCx);

	// --- In  :  nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//			  pszBitmapID	-	The resource string of the bitmap whose 
	//								image has to set to the menu item
	//			  nBitmapIndex	-	The index of the image in the 
	//								bitmap to set to the item 
	//			  nCx			-	Width of each button in the bitmap
	// --- Out : 
	// --- Returns: returns TRUE if successfull otherwise FALSE
	// --- Effect : Sets the given image in the bitmap to the menu item. 
	//				This image will be displayed when the item is selected
	BOOL SetMenuHotImage(UINT nCommandID, LPCTSTR pszBitmapID, 
		int nBitmapIndex, int nCx);


	// --- In  :	nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//				pszBitmapID	-	The resource string of the bitmap 
	//								to set to the menu item
	//				nBitmapID	-	The resource ID of the bitmap 
	//								to set to the menu item
	// --- Out : 
	// --- Returns:	return TRUE if successfull otherwise FALSE
	// --- Effect:	Sets the given bitmap to the menu item. 
 	BOOL SetMenuBitmap(UINT nCommandID, LPCTSTR pszBitmapID);
	BOOL SetMenuBitmap(UINT nCommandID, UINT nBitmapID);

	// --- In  :  nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//			  pszBitmapID	-	The resource string of the bitmap 
	//								to set to the menu item
	//				nBitmapID	-	The resource ID of the bitmap 
	//								to set to the menu item
	// --- Out : 
	// --- Returns:	return TRUE if successfull otherwise FALSE
	// --- Effect:	Sets the given bitmap to the menu item. 
	//				This image will be displayed when the item is selected
 	BOOL SetMenuHotBitmap(UINT nCommandID, LPCTSTR pszBitmapID);
 	BOOL SetMenuHotBitmap(UINT nCommandID, UINT nBitmapID);


	// --- In  :	nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//				pszIconID	-	The resource String of the icon
	//								to set to the menu item
	//				nIconID		-	The resource ID of the icon
	//								to set to the menu item
	// --- Out : 
	// --- Returns: return TRUE if successfull otherwise FALSE
	// --- Effect : Sets the given icon to the menu item. 
	BOOL SetMenuIcon(UINT nCommandID, LPCTSTR pszIconID);
	BOOL SetMenuIcon(UINT nCommandID, UINT nIconID);


	// --- In  :	nCommandID	-	ID of the menu item to which you have 
	//								to set the image
	//				pszIconID	-	The resource String of the icon
	//								to set to the menu item
	//				nIconID		-	The resource ID of the icon
	//								to set to the menu item
	// --- Out : 
	// --- Returns: return TRUE if successfull otherwise FALSE
	// --- Effect : Sets the given icon to the menu item. 
	//				This image will be displayed when the item is selected
	BOOL SetMenuHotIcon(UINT nCommandID, LPCTSTR pszIconID);
	BOOL SetMenuHotIcon(UINT nCommandID, UINT nIconID);


	// --- In  :  
	// --- Out : 
	// --- Returns : return TRUE if successfull otherwise FALSE
	// --- Effect : Sets the toolbar images to the menu items automatically
	BOOL AutoSetMenuImage();


	// --- In  :  All params are same as in CFrameWnd::OnInitMenuPopup
	// --- Out : 
	// --- Returns :
	// --- Note : OnInitMenuPopup message has to be handled in the Window to 
	//			  witch bitmapmenu has been attached and before calling 
	//			  CWnd::OnInitMneuPopup in the handler you have to call 
	//			  this function
	// --- Effect : The Menu items to which bitmaps are set, are 
	//				changed to owner-drawn style
	virtual void OnInitMenuPopup(CMenu* pPopupMenu, UINT Index, BOOL bSysMenu);

	// Allows COXBitmapMenuOrganizer to decide upon what to do 
	// with the char, for further info refer to : Q121623 in Kb
	// --- In  :  All params are same as in CFrameWnd::OnMenuChar
	// --- Out : 
	// --- Returns :
	// --- Note : OnMenuChar message has to be handled in the Window to 
	//			  witch bitmapmenu has been attached call this funtion 
	//			  and return the value returned from this function
	// --- Effect : The Menu items to which bitmaps are set, are 
	//				changed to owner-drawn style
	virtual LRESULT OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu);


	// --- In  :  All params are same as in CFrameWnd::OnMeasureItem
	// --- Out : 
	// --- Returns : Whether the message has been handled or not
	// --- Note : 
	// --- Effect : 
	virtual BOOL OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);

protected:
	BOOL Init();

	CString GetUniqueResourceString(LPCTSTR pszResource,BOOL bBitmapIcon=TRUE);
	CString GetUniqueResourceString(UINT uResourceID,BOOL bBitmapIcon=TRUE);

	BOOL SubclassFrameWindow(CFrameWnd* pFrameWnd);
	void UnsubclassFrameWindow();
	static LRESULT CALLBACK GlobalMenuOrganizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MenuOrganizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static CMenu* OwnFindPopupMenuFromID(CMenu* pMenu, UINT nID);

};

//This a private class as a data structure used in both 
//COXBitmapMenu and COXBitmapMenuOrganizer
class OX_CLASS_DECL COXImageInfo 
{
protected:
	CImageList* m_pImageList;
	int			m_nIndex;
	BOOL		m_bDefaultImgList;
	int			m_nHotIndex;

public :
	COXImageInfo(CImageList* pImageList,int nIndex, BOOL bDefaultImgList=FALSE,
		int nHotIndex=-1)
		:
		m_pImageList(pImageList),
		m_nIndex(nIndex),
		m_bDefaultImgList(bDefaultImgList),
		m_nHotIndex(nHotIndex)

	{
	}

	inline CImageList* GetImageList() const
	{
		ASSERT((m_pImageList == NULL) || 
			(AfxIsValidAddress(m_pImageList, sizeof(*m_pImageList), FALSE)));
		return m_pImageList;
	}

	inline void SetImageList(CImageList* pImageList)
	{
		ASSERT((pImageList == NULL) || 
			(AfxIsValidAddress(pImageList, sizeof(*pImageList), FALSE)));
		m_pImageList=pImageList;
	}

	inline int GetIndex() const
	{
		return m_nIndex;
	}

	inline void SetIndex(int nIndex)
	{
		m_nIndex=nIndex;
	}

	inline int GetHotIndex() const
	{
		return m_nHotIndex;
	}

	inline void SetHotIndex(int nHotIndex)
	{
		m_nHotIndex=nHotIndex;
	}

	inline BOOL IsDefaultImgList() const
	{
		return m_bDefaultImgList;
	}

	inline void SetDefaultImgList(BOOL bDefaultImgList)
	{
		m_bDefaultImgList=bDefaultImgList;
	}
};

//This a private class as a data structure used in both 
//COXBitmapMenu and COXBitmapMenuOrganizer
class OX_CLASS_DECL COXItemInfo 
{
protected:
	COXImageInfo* m_pImageInfo;
	CString m_sText;

public :
	COXItemInfo(COXImageInfo* pImageInfo, CString sText)
		:
		m_pImageInfo(pImageInfo),
		m_sText(sText)
	{
	}

	inline void SetImageInfo(COXImageInfo* pImageInfo)
	{
		ASSERT((pImageInfo == NULL) || 
			(AfxIsValidAddress(pImageInfo, sizeof(*pImageInfo), FALSE)));
		m_pImageInfo=pImageInfo;
	}
	
	inline COXImageInfo* GetImageInfo() const
	{
		ASSERT((m_pImageInfo == NULL) || 
			(AfxIsValidAddress(m_pImageInfo, sizeof(*m_pImageInfo), FALSE)));
		return m_pImageInfo;
	}

	inline void SetText(CString sTxt)
	{
		m_sText = sTxt;
	}
	
	inline CString GetText() const
	{
		return m_sText;
	}
};


#endif //OXBITMAPMENUORGANIZER_H
/////////////////////////////////////////////////////////////////////////////
