// LogicView.h : interface of the CDepCfgView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPCFGVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
#define AFX_DEPCFGVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include  "../Common/docview/EditorView.h"

class CLogicView : public CEditorView
{
protected: // create from serialization only
	CLogicView();
	DECLARE_DYNCREATE(CLogicView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogicView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLogicView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFGVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
