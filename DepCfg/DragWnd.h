#if !defined(AFX_DRAGWND_H__65D6A587_5B21_4B8C_AFDD_2CC1D1AFDF69__INCLUDED_)
#define AFX_DRAGWND_H__65D6A587_5B21_4B8C_AFDD_2CC1D1AFDF69__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DragWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDragWnd window

class CDragWnd : public CWnd
{
// Construction
public:
	CDragWnd();

// Attributes
public:

	CPoint m_ptOldMouse;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDragWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL Create(const CRect& rc, CWnd* pParent);

	virtual ~CDragWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDragWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRAGWND_H__65D6A587_5B21_4B8C_AFDD_2CC1D1AFDF69__INCLUDED_)
