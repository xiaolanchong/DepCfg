// ==========================================================================
//				Class Implementation : COXBitmapMenuOrganizer
// ==========================================================================

// Copyright © Dundas Software Ltd. 1997 - 1998, All Rights Reserved                      

// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXBitmapMenuOrganizer.h"

#include "afxpriv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#ifndef TB_GETIMAGELIST
#define TB_GETIMAGELIST			(WM_USER + 49)
#endif // TB_GETIMAGELIST

#ifndef TB_GETHOTIMAGELIST
#define TB_GETHOTIMAGELIST		(WM_USER + 53)
#endif // TB_GETHOTIMAGELIST

/////////////////////////////////////////////////////////////////////////////
// Definition of static members
CMap<HWND, HWND, COXBitmapMenuOrganizer*, COXBitmapMenuOrganizer*> COXBitmapMenuOrganizer::m_allMenuOrganizers;
// --- A map of all the frame windows that have been subclassed and that are associated with
//     a menu organizer object.  This object will handle the windows messages first


// Data members-------------------------------------------------------------
//Protected 
//	CMap<CString,CString,CImageList*,CImageList*> m_mapStringToImageList;
//  --- Holds mapping bitween the resource string and ImageLists to 
//		avoid multiple ImageLists added
//	CMap<WORD,WORD,COXImageInfo*,COXImageInfo*> m_mapCommandToImageInfo;
//  --- Keeps mapping between Command IDs of menu items and the Image 
//		in the ImageList to be displayed for the menu item
//private
//	CList<COXBitmapMenu*, COXBitmapMenu*> m_BitmapMenuList;
//  --- Keeps a list all the COXBitmapMenus instantiated. This just 
//		used for cleaning purpose.


// Member functions ---------------------------------------------------------
// public:

COXBitmapMenuOrganizer::COXBitmapMenuOrganizer()
	:
	m_pFrameWnd(NULL),
	m_hWnd(NULL),
	m_pfnSuper(NULL)
{
	Init();
}

BOOL COXBitmapMenuOrganizer::Init()
	// --- In  : 
	// --- Out : 
	// --- Returns : Succeeded or not
	// --- Effect : Creates the two bitmaps needed to draw a checkmark and a bullet
{
	CDC memDC;
	VERIFY(memDC.CreateCompatibleDC(NULL));

	CRect rcCheckMark(0, 0, GetSystemMetrics(SM_CXMENUCHECK), GetSystemMetrics(SM_CYMENUCHECK));
	// Convert from device to logical points
	memDC.DPtoLP(&rcCheckMark);		
	// The default image list will contain a checkmark and a bulletmark
	VERIFY(m_DefaultImgList.Create(rcCheckMark.right, rcCheckMark.bottom, TRUE, 2, 0)); 

	CBitmap bmTemp;
	VERIFY(bmTemp.CreateCompatibleBitmap(&memDC, rcCheckMark.right, rcCheckMark.bottom));
	
	// Create a checkmark bitmap
	CBitmap* pbmTempOld=memDC.SelectObject(&bmTemp);
	memDC.DrawFrameControl(&rcCheckMark, DFC_MENU, DFCS_MENUCHECK);
/*	CBitmap* pbmTemp=memDC.SelectObject(pbmTempOld);
	m_DefaultImgList.Add(pbmTemp, RGB(255,255,255));*/
	memDC.SelectObject(pbmTempOld);
	m_DefaultImgList.Add(&bmTemp, RGB(255,255,255));

	// Create a bulletmark bitmap
	pbmTempOld=memDC.SelectObject(&bmTemp);
	memDC.DrawFrameControl(&rcCheckMark, DFC_MENU, DFCS_MENUBULLET);
/*	pbmTemp=memDC.SelectObject(pbmTempOld);
	m_DefaultImgList.Add(pbmTemp, RGB(255,255,255));*/
	memDC.SelectObject(pbmTempOld);
	m_DefaultImgList.Add(&bmTemp, RGB(255,255,255));

	// Clean up
//	bmTemp.DeleteObject();
//	pbmTemp->DeleteObject();
	memDC.DeleteDC();

	return TRUE;
}

void COXBitmapMenuOrganizer::Empty(BOOL bCopyText /*=TRUE */)
{
	CString sKey;
	CImageList* pImageList;
	POSITION pos=m_mapStringToImageList.GetStartPosition();
	// Remove all the elements from m_mapStringToImageList and deletes them
	while(pos != NULL)
	{
		m_mapStringToImageList.GetNextAssoc(pos,sKey,pImageList);
		m_mapStringToImageList.RemoveKey(sKey);
		if (pImageList != &m_DefaultImgList)
			delete pImageList;
	}

	DWORD wKey;
	COXImageInfo* pImageInfo;
	pos=m_mapCommandToImageInfo.GetStartPosition();
	// Remove all the elements from m_mapCommandToImageInfo
	while(pos != NULL)
	{
		m_mapCommandToImageInfo.GetNextAssoc(pos,wKey,pImageInfo);
		m_mapCommandToImageInfo.RemoveKey(wKey);
		delete pImageInfo;
	}

	COXBitmapMenu* pBitmapMenu;
	MENUITEMINFO miiGet, miiPut;
	CString sText;

	// Deletes all the instantiated COXBitmapMenus
	while(!m_BitmapMenuList.IsEmpty())
	{
		pBitmapMenu=m_BitmapMenuList.RemoveHead();

		if (bCopyText)
		{
			int nCount=pBitmapMenu->GetMenuItemCount();

			// Put the strings of the menu's back in, the ownerdraw state has emptied them.
			for(int nIndex=0; nIndex < nCount; nIndex++)
			{
				::ZeroMemory(&miiGet, sizeof(miiGet));
				miiGet.cbSize=sizeof(miiGet);
				miiGet.fMask=MIIM_TYPE | MIIM_DATA;
				// Gets the item info
				::GetMenuItemInfo(pBitmapMenu->GetSafeHmenu(), nIndex, TRUE, &miiGet);

				if (miiGet.fType != MFT_OWNERDRAW)
					continue;

				COXItemInfo* pItemInfo=(COXItemInfo*)miiGet.dwItemData;
				ASSERT(pItemInfo != NULL);
				sText=pItemInfo->GetText();

				// Put the text and the text back into the menu
				::ZeroMemory(&miiPut, sizeof(miiPut));
				miiPut.cbSize=sizeof(miiPut);
				miiPut.fMask=MIIM_TYPE | MIIM_DATA;
				miiPut.fType=(miiGet.fType & ~MFT_OWNERDRAW) | MFT_STRING;
				miiPut.cch=sText.GetLength();
				miiPut.dwTypeData=sText.GetBuffer(miiPut.cch);
				miiPut.dwItemData=0;
				// Put the menuitem with the new data back in its place
				::SetMenuItemInfo(pBitmapMenu->GetSafeHmenu(), nIndex, TRUE, &miiPut);
				sText.ReleaseBuffer();
			}
		}

		pBitmapMenu->Detach();
		delete pBitmapMenu;
	}
}

COXBitmapMenuOrganizer::~COXBitmapMenuOrganizer()
{
	Empty(FALSE);

	// ... Detach a possible attached frame window
	DetachFrameWnd();
}

BOOL COXBitmapMenuOrganizer::AttachFrameWnd(CFrameWnd* pFrameWnd)
{
	if ((m_pFrameWnd==NULL) && (pFrameWnd != NULL))
	{
		m_pFrameWnd=pFrameWnd;
		VERIFY(SubclassFrameWindow(pFrameWnd));
		return TRUE;
	}
	else
	{
		TRACE0("COXBitmapMenuOrganizer::AttachFrameWnd : Failed because already attached or frame wnd is NULL\n");
		return FALSE;
	}
}

CFrameWnd* COXBitmapMenuOrganizer::DetachFrameWnd()
{
	CFrameWnd* pFrameWnd=m_pFrameWnd;
	if (pFrameWnd != NULL)
		UnsubclassFrameWindow();
	m_pFrameWnd=NULL;
	return pFrameWnd;
}

BOOL COXBitmapMenuOrganizer::SetMenuImage(UINT nCommandID, LPCTSTR pszBitmapID, 
										  int nBitmapIndex, int nCx)
{
	if (m_pFrameWnd==NULL)
	{
		TRACE0("COXBitmapMenuOrganizer::SetMenuImage : Must attach a frame window before calling this function\n");
		return FALSE;
	}

	CString sUResourse=GetUniqueResourceString(pszBitmapID);
	CImageList* pImageList;
	//Doesn't add if already the resource is asociated with a ImageList
	if(!m_mapStringToImageList.Lookup(sUResourse,pImageList))
	{
		pImageList=new CImageList;
		pImageList->Create(pszBitmapID,nCx,0,RGB(192,192,192)); 
		m_mapStringToImageList[sUResourse]=pImageList;
	}
	COXImageInfo* pImageInfo=new COXImageInfo(pImageList,nBitmapIndex);
	//establishes the mapping between image info and command
	m_mapCommandToImageInfo[nCommandID]=pImageInfo;
	return TRUE;
}

BOOL COXBitmapMenuOrganizer::SetMenuHotImage(UINT nCommandID, LPCTSTR pszBitmapID, 
											 int nBitmapIndex, int nCx)
{
	if(m_pFrameWnd==NULL)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotImage : Must attach a frame window before calling this function\n"));
		return FALSE;
	}

	COXImageInfo* pImageInfo=NULL;
	if(!m_mapCommandToImageInfo.Lookup(nCommandID,pImageInfo))
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotImage : normal image must be set for the item before calling this function\n"));
		return FALSE;
	}
	ASSERT(pImageInfo!=NULL);
	CImageList* pExistingImageList=pImageInfo->GetImageList();
	ASSERT(pExistingImageList!=NULL);

	CImageList imageList;
	if(!imageList.Create(pszBitmapID,nCx,0,RGB(192,192,192)))
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotImage : failed to load the bitmap\n"));
		return FALSE;
	}
	ASSERT((HIMAGELIST)imageList!=NULL);

	HICON hIcon=imageList.ExtractIcon(nBitmapIndex);
	ASSERT(hIcon!=NULL);

	int nIndex=pExistingImageList->Add(hIcon);
	if(nIndex==-1)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotImage : failed to add bitmap to menu item's image list\n"));
		return FALSE;
	}

	pImageInfo->SetHotIndex(nIndex);

	return TRUE;
}

BOOL COXBitmapMenuOrganizer::SetMenuBitmap(UINT nCommandID, LPCTSTR pszBitmapID)
{
	if (m_pFrameWnd==NULL)
	{
		TRACE0("COXBitmapMenuOrganizer::SetMenuBitmap : Must attach a frame window before calling this function\n");
		return FALSE;
	}

	CBitmap Bitmap;
	//to get the width of the bitmap
	if(!Bitmap.LoadBitmap(pszBitmapID)) 
		return FALSE;
	BITMAP BitmapInfo;
	Bitmap.GetBitmap(&BitmapInfo);
	Bitmap.DeleteObject();
	return SetMenuImage(nCommandID, pszBitmapID, 0, BitmapInfo.bmWidth);
}

BOOL COXBitmapMenuOrganizer::SetMenuBitmap(UINT nCommandID, UINT nBitmapID)
{
	return SetMenuBitmap(nCommandID,MAKEINTRESOURCE(nBitmapID));
}

BOOL COXBitmapMenuOrganizer::SetMenuHotBitmap(UINT nCommandID, LPCTSTR pszBitmapID)
{
	if (m_pFrameWnd==NULL)
	{
		TRACE0("COXBitmapMenuOrganizer::SetMenuBitmap : Must attach a frame window before calling this function\n");
		return FALSE;
	}

	CBitmap Bitmap;
	//to get the width of the bitmap
	if(!Bitmap.LoadBitmap(pszBitmapID)) 
		return FALSE;
	BITMAP BitmapInfo;
	Bitmap.GetBitmap(&BitmapInfo);
	Bitmap.DeleteObject();
	return SetMenuHotImage(nCommandID, pszBitmapID, 0, BitmapInfo.bmWidth);
}

BOOL COXBitmapMenuOrganizer::SetMenuHotBitmap(UINT nCommandID, UINT nBitmapID)
{
	return SetMenuHotBitmap(nCommandID,MAKEINTRESOURCE(nBitmapID));
}

BOOL COXBitmapMenuOrganizer::SetMenuIcon(UINT nCommandID, LPCTSTR pszIconID)
{
	if (m_pFrameWnd==NULL)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuIcon : Must attach a frame window before calling this function\n"));
		return FALSE;
	}

	HICON hIcon;
	CImageList* pImageList;
	CString sUResource=GetUniqueResourceString(pszIconID,FALSE);
	//if the Icon resource is not already added
	if(!m_mapStringToImageList.Lookup(sUResource,pImageList))
	{
		//Loads Icon
		hIcon=AfxGetApp()->LoadIcon(pszIconID);
		if(hIcon==NULL)
			return FALSE;
		
		pImageList=new CImageList;
		pImageList->Create(GetSystemMetrics(SM_CXICON),
			GetSystemMetrics(SM_CYICON),FALSE,1,0); 
		pImageList->Add(hIcon);
		//To differentiate between Icon and bitmap add "Icon" at the end
		m_mapStringToImageList[sUResource]=pImageList;
	}
	COXImageInfo* pImageInfo=new COXImageInfo(pImageList,0);
	//sets the mapping between Coomand ID and Image Info
	m_mapCommandToImageInfo[nCommandID]=pImageInfo;
	return TRUE;
}
	
BOOL COXBitmapMenuOrganizer::SetMenuIcon(UINT nCommandID, UINT nIconID)
{
	return SetMenuIcon(nCommandID,MAKEINTRESOURCE(nIconID));
}

BOOL COXBitmapMenuOrganizer::SetMenuHotIcon(UINT nCommandID, LPCTSTR pszIconID)
{
	if (m_pFrameWnd==NULL)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotIcon : Must attach a frame window before calling this function\n"));
		return FALSE;
	}

	COXImageInfo* pImageInfo=NULL;
	if(!m_mapCommandToImageInfo.Lookup(nCommandID,pImageInfo))
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotIcon : normal image must be set for the item before calling this function\n"));
		return FALSE;
	}
	ASSERT(pImageInfo!=NULL);
	CImageList* pExistingImageList=pImageInfo->GetImageList();
	ASSERT(pExistingImageList!=NULL);

	HICON hIcon;
	//Loads Icon
	hIcon=AfxGetApp()->LoadIcon(pszIconID);
	if(hIcon==NULL)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotIcon : failed to load the icon\n"));
		return FALSE;
	}
		
	int nIndex=pExistingImageList->Add(hIcon);
	if(nIndex==-1)
	{
		TRACE(_T("COXBitmapMenuOrganizer::SetMenuHotIcon : failed to add the icon to menu item's image list\n"));
		return FALSE;
	}

	pImageInfo->SetHotIndex(nIndex);

	return TRUE;
}
	
BOOL COXBitmapMenuOrganizer::SetMenuHotIcon(UINT nCommandID, UINT nIconID)
{
	return SetMenuHotIcon(nCommandID,MAKEINTRESOURCE(nIconID));
}

BOOL COXBitmapMenuOrganizer::AutoSetMenuImage()
{
	if (m_pFrameWnd==NULL)
	{
		TRACE0("COXBitmapMenuOrganizer::AutoSetMenuImage : Must attach a frame window before calling this function\n");
		return FALSE;
	}

	// Empty previous settings
	Empty();

	CToolBar* pToolBar=NULL;
	UINT nID=0;
	UINT nStyle=0;
	int iImage=0;
	int iHotImage=0;

	int nListIndex=0;
	CString sListIndex;
	// Iterate all the control bars and use only the toolbars
	POSITION pos=m_pFrameWnd->m_listControlBars.GetHeadPosition();
	while (pos != NULL)
	{
		pToolBar=DYNAMIC_DOWNCAST(CToolBar, 
			(CControlBar*)m_pFrameWnd->m_listControlBars.GetNext(pos));
		//If its a CToolBar
		if(pToolBar!=NULL)
		{
			// Get the original imagelist of the toolbar
			HIMAGELIST hOrgImageList=
				(HIMAGELIST)pToolBar->SendMessage(TB_GETIMAGELIST);
			CImageList orgImageList;
			if(!orgImageList.Attach(hOrgImageList))
			{
				TRACE(_T("COXBitmapMenuOrganizer::AutoSetMenuImage : Failed to retrieve image list from toolbars\n"));
				return FALSE;
			}

			int nOrgImageCount=orgImageList.GetImageCount();
			ASSERT(0<nOrgImageCount);

			// Get the hot imagelist of the toolbar
			HIMAGELIST hHotImageList=
				(HIMAGELIST)pToolBar->SendMessage(TB_GETHOTIMAGELIST);
			CImageList hotImageList;
			int nHotImageCount=0;
			if(hHotImageList!=NULL)
			{
				VERIFY(hotImageList.Attach(hHotImageList));
				nHotImageCount=hotImageList.GetImageCount();
				ASSERT(nHotImageCount==nOrgImageCount);
			}

			CImageList* pNewImageList=new CImageList;
			CRect buttonRect;
			pToolBar->GetItemRect(0, buttonRect);

			int nButtonIndex=0;
#if _MFC_VER > 0x0421
			VERIFY(pNewImageList->Create(&orgImageList));
#else
			IMAGEINFO imageInfo;
			VERIFY(orgImageList.GetImageInfo(0,&imageInfo));
			VERIFY(pNewImageList->Create(imageInfo.rcImage.right-imageInfo.rcImage.left,
				imageInfo.rcImage.bottom-imageInfo.rcImage.top,
				ILC_COLOR24|ILC_MASK,0,0));
			for(nButtonIndex=0; nButtonIndex<nOrgImageCount; nButtonIndex++)
			{
				HICON hIcon=orgImageList.ExtractIcon(nButtonIndex);
				ASSERT(hIcon!=NULL);
				VERIFY(pNewImageList->Add(hIcon)!=-1);
			}
#endif
			for(nButtonIndex=0; nButtonIndex<nHotImageCount; nButtonIndex++)
			{
				HICON hIcon=hotImageList.ExtractIcon(nButtonIndex);
				ASSERT(hIcon!=NULL);
				VERIFY(pNewImageList->Add(hIcon)!=-1);
			}

			ASSERT(pNewImageList->GetImageCount()==nHotImageCount+nOrgImageCount);

			// Close everything
			if(nHotImageCount>0)
				hotImageList.Detach();
			orgImageList.Detach();


			nListIndex++;
			sListIndex.Format(_T("%i"), nListIndex);
			m_mapStringToImageList[sListIndex]=pNewImageList;

			int nButtonCount=pToolBar->GetToolBarCtrl().GetButtonCount();
			// Iterate all buttons on the toolbar
			for(nButtonIndex=0; nButtonIndex < nButtonCount; nButtonIndex++)
			{
				nID=0;
				nStyle=0;
				iImage=0;
				iHotImage=-1;
				pToolBar->GetButtonInfo(nButtonIndex, nID, nStyle, iImage);
				if(nHotImageCount>0)
					iHotImage=iImage+nOrgImageCount;
				//If the Item is not a button
				if(nStyle==TBBS_SEPARATOR)
					continue;

				//Sets the mapping
				COXImageInfo* pOldImageInfo=NULL;
				COXImageInfo* pImageInfo=
					new COXImageInfo(pNewImageList,iImage,FALSE,iHotImage);
				// ... If nID was already in use, delete previous value
				if (m_mapCommandToImageInfo.Lookup(nID, pOldImageInfo))
					delete pOldImageInfo;
				// ... Set new value
				m_mapCommandToImageInfo[nID]=pImageInfo;
			}
		}
	}

	return TRUE;
}

void COXBitmapMenuOrganizer::OnInitMenuPopup(CMenu* pPopupMenu, UINT Index, BOOL bSysMenu)
{
	ASSERT(pPopupMenu->GetSafeHmenu() != NULL);
	// Replace the attached CMenu by a new COXBitmapMenu object
	// if necessary (if not done yet)
	CMenu* pMenu=pPopupMenu;

	COXBitmapMenu* pBitmapMenu=DYNAMIC_DOWNCAST(COXBitmapMenu, pMenu);
	//If a COXBitmapMenu is not already attched to the SubMenu
	BOOL bNewBitmapMenu=FALSE;
	if (pBitmapMenu==NULL)
	{
		pBitmapMenu=new COXBitmapMenu();
		
		//Detaches the CMenu
		HMENU hMenu=pMenu->Detach();
		
		//Attaches the COXBitmapMenu object to the menu handle
		pBitmapMenu->Attach(hMenu);
		m_BitmapMenuList.AddTail(pBitmapMenu);
		pPopupMenu=pBitmapMenu;

		bNewBitmapMenu=TRUE;
	}

	// Before we begin iterating the menuitems to make em ownerdrawn, be sure
	// the CmdUI mechanism doesn't want to add or change some menuitems (E.g. MRU list)
	pBitmapMenu->OnUpdateCmdUI(CWnd::FromHandle(m_hWnd), Index, bSysMenu);

	UINT uItemID;
	COXImageInfo* pImageInfo;
	int nCount=pPopupMenu->GetMenuItemCount();

	int nAmpIndex;
	MENUITEMINFO miiGet, miiPut;
	CString sText;

	//Make all the menu items OwnerDrawn except submenus
	for(int nIndex=0; nIndex < nCount; nIndex++)
	{
		// Even cascade menu item should be made owner drawn
		//  so that the text of these items is align properly
		// Therfore the next two lines are removed
		// if(pPopupMenu->GetSubMenu(nIndex)) 
		//	continue;

		::ZeroMemory(&miiGet, sizeof(miiGet));
		miiGet.cbSize=sizeof(miiGet);
		miiGet.fMask=MIIM_TYPE | MIIM_ID | MIIM_STATE | MIIM_DATA | MIIM_SUBMENU;
		miiGet.fType=MFT_STRING;
		miiGet.cch=300;
		miiGet.dwTypeData=sText.GetBuffer(miiGet.cch);
		// Gets the item info
		miiGet.dwTypeData[0]=_T('\0');
		::GetMenuItemInfo(pPopupMenu->GetSafeHmenu(), nIndex, TRUE, &miiGet);
		sText.ReleaseBuffer();

		if (miiGet.fType==MFT_SEPARATOR)
			continue;

		uItemID=miiGet.wID;

		//Makes the item owner-drawn and sets the ItemInfo to the ItemData
		// which in turn will be used in COXBitmapMenu while drawing
		::ZeroMemory(&miiPut, sizeof(miiPut));
		miiPut.cbSize=sizeof(miiPut);
		miiPut.fMask=MIIM_TYPE | MIIM_DATA;
		miiPut.fType=(miiGet.fType & ~MFT_STRING) | MFT_OWNERDRAW;

		// Gets the ImageInfo for the command if exist
		if(!m_mapCommandToImageInfo.Lookup(uItemID,pImageInfo))
		{
			CImageList* pImageList;
			CString sUResource=GetUniqueResourceString(uItemID,FALSE);
			
			// if the Normal menuitem is not already added
			if(!m_mapStringToImageList.Lookup(sUResource, pImageList))
				m_mapStringToImageList[sUResource]=pImageList=&m_DefaultImgList;

			pImageInfo=new COXImageInfo(pImageList, 0, TRUE);
			//sets the mapping between MenuItem ID and Image Info
			m_mapCommandToImageInfo[uItemID]=pImageInfo;
		}

		COXItemInfo* pItemInfo=NULL;
		if (miiGet.dwItemData != 0 && !bNewBitmapMenu)
		{
			pItemInfo=(COXItemInfo*)miiGet.dwItemData;
			miiPut.dwItemData=(DWORD)pItemInfo;
			if(miiGet.fType==MFT_STRING)
			{
				pItemInfo->SetText(sText);
			}
		}
		else
		{
			COXItemInfo* pItemInfo=new COXItemInfo(pImageInfo, sText);
			miiPut.dwItemData=(DWORD)pItemInfo;
			pBitmapMenu->AddItemInfo(pItemInfo);
		}

		// Put the menuitem with the new data back in its place
		::SetMenuItemInfo(pPopupMenu->GetSafeHmenu(), nIndex, TRUE, &miiPut);

		// A mapping is maintaned for keyboard acces chars of menu items and the commands IDs.
		nAmpIndex=sText.Find('&'); 
		TCHAR LoCase=_T('\0');
		if(nAmpIndex != -1)
		{
			LoCase=(TCHAR)towlower(sText[nAmpIndex+1]);
			(pBitmapMenu->m_KeyAccessMap)[LoCase]=nIndex;
		}
	}

	//Calculates the various internal extents
	pBitmapMenu->CalcExtents();
}


LRESULT COXBitmapMenuOrganizer::OnMenuChar(UINT nChar, UINT /* nFlags */, CMenu* pMenu)
{
	//to check wether its a COXBitmapMenu
	COXBitmapMenu* pBitmapMenu=DYNAMIC_DOWNCAST(COXBitmapMenu, pMenu);
	int nLoWord =0;
	int nHiWord =0;
	//if its a COXBitmapMenu
	if(pBitmapMenu != NULL)
	{
		int nIndex=0;
		//sets the index of the menu item for the corresponding char
		if(pBitmapMenu->m_KeyAccessMap.Lookup((TCHAR)nChar,nIndex))
		{
			nHiWord=2;
			nLoWord=nIndex;
		}
	}
	return MAKELRESULT(nLoWord,nHiWord);
}

// Measure item implementation relies on unique control/menu IDs
BOOL COXBitmapMenuOrganizer::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	// ... Handle the cases we can handle
	BOOL bHandled=TRUE;
	CWnd* pWnd=CWnd::FromHandle(m_hWnd);
	ASSERT(pWnd != NULL);
	if (lpMeasureItemStruct->CtlType==ODT_MENU)
	{
		ASSERT(lpMeasureItemStruct->CtlID==0);
		CMenu* pMenu;

		_AFX_THREAD_STATE* pThreadState=AfxGetThreadState();
		if (pThreadState->m_hTrackingWindow==m_hWnd)
		{
			// start from popup
			pMenu=CMenu::FromHandle(pThreadState->m_hTrackingMenu);
		}
		else
		{
			// start from menubar
			pMenu=pWnd->GetMenu();
		}

		pMenu=OwnFindPopupMenuFromID(pMenu, lpMeasureItemStruct->itemID);
		if (pMenu != NULL)
			pMenu->MeasureItem(lpMeasureItemStruct);
		else
		{
			TRACE0("COXBitmapMenuOrganizer::OnMeasureItem: Unknown Menu : passing to Default()\n");
			bHandled=FALSE;
		}

	}
	else
	{
		CWnd* pChild=pWnd->GetDescendantWindow(lpMeasureItemStruct->CtlID, TRUE);
		if (pChild != NULL && pChild->SendChildNotifyLastMsg())
			return bHandled;     // eaten by child
	}
	return bHandled;
}

CString COXBitmapMenuOrganizer::GetUniqueResourceString(UINT uResourceID,BOOL bBitmapIcon)
{
	CString sRet;
	CString sNum;
	sNum.Format(_T("%u"), uResourceID);
	sNum.ReleaseBuffer();
	sRet=CString((TCHAR)CHAR_MAX)+sNum+ 
		CString(bBitmapIcon ? (TCHAR)CHAR_MAX : (TCHAR)CHAR_MIN);
	return sRet;
}

CString COXBitmapMenuOrganizer::GetUniqueResourceString(LPCTSTR pszResource,BOOL bBitmapIcon)
{
	if(HIWORD((DWORD)pszResource)==0)
	{
		return GetUniqueResourceString(LOWORD((DWORD)pszResource),bBitmapIcon);
	}
	else
		return CString(pszResource)+
			CString(bBitmapIcon ? (TCHAR)CHAR_MAX : (TCHAR)CHAR_MIN);
}

BOOL COXBitmapMenuOrganizer::SubclassFrameWindow(CFrameWnd* pFrameWnd)
	// --- In  : pFrameWnd : The frame  window 
	// --- Out : 
	// --- Returns : Whether it was successful or not
	// --- Effect : This function subclasses the frame window 
{
	ASSERT(pFrameWnd != NULL);
	ASSERT(pFrameWnd->GetSafeHwnd() != NULL);
	ASSERT_VALID(pFrameWnd);

	if (m_pfnSuper != NULL)
	{
		// Already subclasses, check that hWnd and window proc is correct
		if ( (m_hWnd != pFrameWnd->GetSafeHwnd()) || 
		     ((WNDPROC)::GetWindowLong(pFrameWnd->GetSafeHwnd(), GWL_WNDPROC) != GlobalMenuOrganizerProc) )
		{
			TRACE0("COXBitmapMenuOrganizer::SubclassFrameWindow : Failed because already subclassed by other window proc\n");
			return FALSE;
		}
		return TRUE;
	}

	ASSERT(m_hWnd==NULL);
	ASSERT(m_pfnSuper==NULL);
#ifdef _DEBUG
	COXBitmapMenuOrganizer* pDummyLayoutManager=NULL;
	// ... Should not yet be in list of subclassed frame windows
	ASSERT(!m_allMenuOrganizers.Lookup(m_hWnd, pDummyLayoutManager));
#endif // _DEBUG

	// ... Subclass window (Windows way, not MFC : because may already be subclessed by MFC)
	m_hWnd=pFrameWnd->GetSafeHwnd();
	m_pfnSuper=(WNDPROC)::GetWindowLong(pFrameWnd->GetSafeHwnd(), GWL_WNDPROC);
	::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)GlobalMenuOrganizerProc);

	// ... Store in the global map
	m_allMenuOrganizers.SetAt(m_hWnd, this);

	ASSERT_VALID(this);
	return (m_hWnd != NULL);;
}

void COXBitmapMenuOrganizer::UnsubclassFrameWindow()
	// --- In  : 
	// --- Out : 
	// --- Returns : 
	// --- Effect : This function unsubclasses the window 
	//				It removes this object from the map
	//				When it is the last in the list it restores the original
	//				windows procedure
{
	ASSERT_VALID(this);

	if (m_hWnd != NULL)
	{
		// Put back original window procedure
		ASSERT(m_pfnSuper != NULL);
		ASSERT(m_pfnSuper != GlobalMenuOrganizerProc);
		// ... GlobalLayoutManagerProc is not used anymore : set WNDPROC back to original value
		::SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pfnSuper);
		// ... Remove use from global map
		m_allMenuOrganizers.RemoveKey(m_hWnd);
		m_hWnd=NULL;
		m_pfnSuper=NULL;
	}

	ASSERT(m_hWnd==NULL);
	ASSERT(m_pfnSuper==NULL);

	ASSERT_VALID(this);
}

LRESULT CALLBACK COXBitmapMenuOrganizer::GlobalMenuOrganizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	// --- In  : hWnd : 
	//			 uMsg : 
	//			 wParam : 
	//			 lParam :
	// --- Out : 
	// --- Returns : The result of the message
	// --- Effect : This is the global windows procedure of all the menu organizers
	//              objects that have subclasses a window
{
	COXBitmapMenuOrganizer* pBitmapMenuOrganizer=NULL;

	VERIFY(m_allMenuOrganizers.Lookup(hWnd, pBitmapMenuOrganizer));
	ASSERT_VALID(pBitmapMenuOrganizer);
	return pBitmapMenuOrganizer->MenuOrganizerProc(hWnd, uMsg, wParam, lParam);
}

LRESULT COXBitmapMenuOrganizer::MenuOrganizerProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	// --- In  : hWnd : 
	//			 uMsg : 
	//			 wParam : 
	//			 lParam :
	// --- Out : 
	// --- Returns : The result of the message
	// --- Effect : This is the member function called by the windows procedure of the 
	//				COXBitmapMenuOrganizer object
{
	ASSERT_VALID(this);
	ASSERT(hWnd==m_hWnd);

	// Handling before base class
	BOOL bHandled=FALSE;
	LRESULT result=0;

	// Let the original message procedure handle it first of all
	ASSERT(m_pfnSuper != NULL);
	ASSERT(m_pfnSuper != GlobalMenuOrganizerProc);
	result=m_pfnSuper(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_MENUCHAR:
		{
			CMenu* pMenu=CMenu::FromHandle((HMENU)lParam);
			int nChar=towlower(LOWORD(wParam));
			int nFlags=HIWORD(wParam);
			result=OnMenuChar(nChar, nFlags, pMenu);
			bHandled=TRUE;
			break;
		}
	case WM_INITMENUPOPUP:
		{
			CMenu* pPopupMenu=CMenu::FromHandle((HMENU)wParam);
			UINT nIndex=(UINT) LOWORD(lParam); 
			BOOL bSysMenu=(BOOL) HIWORD(lParam);

			// Don't make system menu's owner-drawn.  We can't retrieve them
			// in OnMeasureitem.
			if (bSysMenu)
			{
				TRACE0("COXBitmapMenuOrganizer::MenuOrganizerProc: System Menu skipping Ownerdraw\n");
				bHandled=FALSE;
				break;
			}

			OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

			//			IMPORTANT			//
			// Here is the code that causes problem if you change the text of menu item
			// in OnUpdateXXX routine. As a matter of fact we use standard way of updating
			// items in the OnInitMenuPopup so we can comment below code

/*			// The CMenu object where pPopupMenu is pointing at has been detached
			// in OnInitMenuPopup, so we'll have to create a new temp menu
			pPopupMenu=CMenu::FromHandle((HMENU)wParam);
			// updating the user interface.		
			ASSERT(pPopupMenu->IsKindOf(RUNTIME_CLASS(COXBitmapMenu)));
			((COXBitmapMenu*)pPopupMenu)->OnUpdateCmdUI(CWnd::FromHandle(m_hWnd),
				nIndex, bSysMenu);
*/
			// the default msg proc further only deals with CmdUI and that is already
			// done in OnUpdateCmdUI, so halt further processing
			bHandled=TRUE;
			break;
		}
		case WM_MEASUREITEM:
		{
			int nIDCtl=(int)wParam;
			LPMEASUREITEMSTRUCT lpMeasureItemStruct=(LPMEASUREITEMSTRUCT)(LPTSTR)lParam;
			bHandled=OnMeasureItem(nIDCtl, lpMeasureItemStruct);
			break;
		}
		default:
			// Do nothing special
			;
	}

/*	if (!bHandled)
		{
		// Let the original message procedure handle it
		ASSERT(m_pfnSuper != NULL);
		ASSERT(m_pfnSuper != GlobalMenuOrganizerProc);
		result=m_pfnSuper(hWnd, uMsg, wParam, lParam);
		}
*/
	// Handling after base class
	/*
	switch (uMsg)
		{
		default:
			// Do nothing special
			;
		}
	*/

	return result;
}

CMenu* COXBitmapMenuOrganizer::OwnFindPopupMenuFromID(CMenu* pMenu, UINT nID)
{
	ASSERT_VALID(pMenu);
	// walk through all items, looking for ID match
	UINT nItems=pMenu->GetMenuItemCount();
	UINT nItemID=0;
	for (int iItem=0; iItem < (int)nItems; iItem++)
	{
		// Always check the menu item itself first
		nItemID=pMenu->GetMenuItemID(iItem);
		if (nItemID==(UINT)-1)
			// ... If it is a cascade menu, use its handle instead of its ID (which is always -1)
			//     So appearently Windows passes the HMENU instead of the ID for cascade manus
			//     in MEASUREITEMSTRUCT when sending WM_MEASUREITEM
			nItemID=(UINT)pMenu->GetSubMenu(iItem)->GetSafeHmenu();

		if (nItemID==nID)
		{
			// ... it is an item inside our popup
			pMenu=CMenu::FromHandlePermanent(pMenu->m_hMenu);
			return pMenu;
		}

		// Then check child menus
		CMenu* pPopup=pMenu->GetSubMenu(iItem);
		if (pPopup != NULL)
		{
			// ... Recurse to child popup
			pPopup=OwnFindPopupMenuFromID(pPopup, nID);
			// ... Check popups on this popup
			if (pPopup != NULL)
				return pPopup;
		}
	}
	// ... Not found
	return NULL;
}
