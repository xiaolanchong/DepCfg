#if !defined(AFX_EDITORVIEW_H__602FA153_9DBD_412B_9A4B_10E5FFB1C30E__INCLUDED_)
#define AFX_EDITORVIEW_H__602FA153_9DBD_412B_9A4B_10E5FFB1C30E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditorView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditorView view

#include "../item/ProjectDB.h"
#include "DepCfgDoc.h"

#include "../utility/ToolTipWnd.h"

#include  <gdiplus.h>
using namespace Gdiplus;
using boost::shared_ptr;

//class CDepCfgDoc;



class CEditorView : public CScrollView, protected PropertyCB
{

protected:
	CEditorView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CEditorView)

	struct EditorState
	{ 
		CPoint						ptOldMouse;
		CSize						sizeScroll;
		bool						bAction;

		bool						bToolTip;
		EditorState() : bAction(false), bToolTip(false) {}
	} m_EditorState;


	void	OnPrint(CDC* pDC);

	Zoom m_Zoom;

	virtual void SetNumEditProperty(CString Name, CString Value, PropertyObserver* pObserver) ;
	virtual void SetEditProperty(CString Name, CString Value, PropertyObserver* pObserver) ;
	virtual void SetComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver);
	virtual void SetEditComboProperty(CString Name, std::vector<CString> Values, int nDefValue, PropertyObserver* pObserver) ;
	virtual void SetIpAddressProperty(CString Name, CString Value, PropertyObserver* pObserver);

		// 0 - false, 1 - true, -1 - undefined
	virtual void SetBoolProperty(CString Name, int  nDefValue, PropertyObserver* pObserver) ;
	virtual void SetHeader(CString Name) ;
	virtual void SetStatic( CString Name, CString Value );

// Attributes
public:

	CGuiDB*		m_pDB;

	CToolTipWnd	m_wndToolTip;

	void	OnDraw(CDC* pDC);
	void	AdjustScroll(bool fit = false);
	void	HideToolTip();

	void	ScrollTimer();
	void	ToolTipTimer();
	void	MoveToolTip(CPoint pt);
	void	StartEdit();
	void	EndEdit();

	void	DeleteCurrentItem();
	bool	CheckCurrentItem();
//	CSize	AdjustScroll(CPoint ptMouse, CSize sizeMouse);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorView)
	public:
	virtual void OnInitialUpdate();
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Attributes
public:
	CDepCfgDoc* GetDocument();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditorView)
	afx_msg void OnCancelMode();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	afx_msg LRESULT OnScale(WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnFit(WPARAM wParam, LPARAM lParam );
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DepCfgView.cpp
inline CDepCfgDoc* CEditorView::GetDocument()
   { return (CDepCfgDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITORVIEW_H__602FA153_9DBD_412B_9A4B_10E5FFB1C30E__INCLUDED_)
