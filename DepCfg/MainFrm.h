// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
	
#if !defined(AFX_MAINFRM_H__BEFFF78D_2090_4916_A216_4588928563B8__INCLUDED_)
#define AFX_MAINFRM_H__BEFFF78D_2090_4916_A216_4588928563B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut/OX3DTabView.h"
#include "ut/OXBitmapMenuOrganizer.h"
#include "ut/OXFrameWndDock.h"
#include "ut/OXFrameWndDock.h"
#include "ut/OXSizeToolBar.h"
#include "ut/OXSizeDockBar.h"

#include "ut/OXMenuBar.h"

#include "Mysizeba.h"
#include "ItemToolBar.h"

#include "ScaleComboBox.h"

#define MainFrameParent COXFrameWndSizeDock

class CMainFrame : public /*COXMenuBarFrame<*/MainFrameParent/*,COXSizeDockBar>*/
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

	CPropertyListBox*	GetPropertyBox() { return &m_wndDockBar.m_wndListBox; }
	void				SetPropertyBoxHeader(CString Name) { m_wndDockBar.SetWindowText(Name); m_wndDockBar.SendMessage(WM_NCPAINT);}

	void				ResetScale() { m_wndScaleBox.SetCurSel(1);}
	void				ResetProperty();
	void				DockControlBarLeftOf(CToolBar *leftOf, CToolBar* right);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar				m_wndStatusBar;
#if 0
	COXCoolToolBar			m_wndToolBar;
#else
	CToolBar				m_wndToolBar;
#endif
	CItemToolBar			m_wndItemBar;

	COX3DTabViewContainer	m_wndTabContainer;
	CImageList				m_ilContainer;
	COXBitmapMenuOrganizer  m_Organizer;

	CMyListBoxBar			m_wndDockBar;
//	CMyListBoxBar			m_wndPropBar;

	CScaleComboBox			m_wndScaleBox;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateItemSlave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateItemCommutator(CCmdUI* pCmdUI);
	afx_msg void OnUpdateItemComputer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateItemMaster(CCmdUI* pCmdUI);
	afx_msg void OnCancelMode();
	afx_msg void OnViewProperty();
	afx_msg void OnUpdateViewProperty(CCmdUI* pCmdUI);
	afx_msg void OnViewItem();
	afx_msg void OnUpdateViewItem(CCmdUI* pCmdUI);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDeleteItem();
	afx_msg void OnUpdateDeleteItem(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnUpdateTabs(WPARAM, LPARAM);

//	 afx_msg LRESULT OnPressed(WPARAM wParam, LPARAM lParam);

	 afx_msg LRESULT OnScale(WPARAM wParam, LPARAM lParam );
	 afx_msg LRESULT OnFit(WPARAM wParam, LPARAM lParam );
	 afx_msg void OnEnterMenuLoop( BOOL bIsTrackPopupMenu );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__BEFFF78D_2090_4916_A216_4588928563B8__INCLUDED_)
