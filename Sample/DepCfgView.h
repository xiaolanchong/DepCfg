#if !defined(AFX_DEPCFGVIEW_H__757292AD_EF14_4AFE_A484_63E466E3C876__INCLUDED_)
#define AFX_DEPCFGVIEW_H__757292AD_EF14_4AFE_A484_63E466E3C876__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DepCfgView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDepCfgView view

#include "..\Include\DepCfgWnd.h"

class CDepCfgView : public CView
{
	CDepCfgWnd		m_Child;
protected:
	CDepCfgView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDepCfgView)

// Attributes
public:

	void	ZoomIn();						// zoom view
	BOOL	CheckZoomIn() const;			// Can view be zoomed
	
	void	ZoomOut();						// zoom out view
	BOOL	CheckZoomOut() const;			// 

	void	FitToSize();					// fit content to window client sizes

	void	State(void* p);					// info from events.dll, see sysstate.h
	BOOL	SetProjectFile(	LPCWSTR path,	// Full file name of configuration
							DWORD TimeOutSec = 5);	// Timeout, sec
	
	bool	ConnectTeleCallback(ITeleCallback* pCallback);				// Connent observer to view
	bool	DisconnectTeleCallback(ITeleCallback* pCallback = NULL);	// if NULL disconnect all

	bool	OpenFromDatabase();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDepCfgView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CDepCfgView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFGVIEW_H__757292AD_EF14_4AFE_A484_63E466E3C876__INCLUDED_)
