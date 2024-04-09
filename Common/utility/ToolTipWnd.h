#if !defined(AFX_TOOLTIPWND_H__04C78ABE_39B9_47A3_87F8_889B1A1248C7__INCLUDED_)
#define AFX_TOOLTIPWND_H__04C78ABE_39B9_47A3_87F8_889B1A1248C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ToolTipWnd.h : header file
//
#include "Callback.h"

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CToolTipWnd window

class CToolTipWnd : public CStatic, public ReadOnlyProperty
{
	int		m_NameCol, m_ValueCol, m_Cell;
	std::vector< std::pair<CString, CString> >	m_Properties;

	void	Draw(Graphics& gr) const;

	CWnd*	m_pWnd;
// Construction
public:
	CToolTipWnd(CWnd* pWnd);

	BOOL Create(const CRect& rc, CWnd* pParent);

	void SetReadOnlyProperty( CString Name, CString Value );
	void EraseProperty( CString Name);
	void Clear();
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToolTipWnd)
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToolTipWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToolTipWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOOLTIPWND_H__04C78ABE_39B9_47A3_87F8_889B1A1248C7__INCLUDED_)
