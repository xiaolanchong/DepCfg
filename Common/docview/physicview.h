// DepCfgView.h : interface of the CDepCfgView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
#define AFX_PHYSICVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef DEPCFGLITE 
#include "../../depcfglite/docview/EditorView.h"
#else
#include "EditorView.h"
#endif

class CPhysicView : public CEditorView
{
protected: // create from serialization only
	CPhysicView();
	DECLARE_DYNCREATE(CPhysicView)

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPhysicView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPhysicView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPhysicView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSICVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
