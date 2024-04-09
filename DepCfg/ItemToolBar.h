#if !defined(AFX_ITEMTOOLBAR_H__1ECE7364_E68C_4712_AF72_65DA01C4BCA6__INCLUDED_)
#define AFX_ITEMTOOLBAR_H__1ECE7364_E68C_4712_AF72_65DA01C4BCA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ItemToolBar.h : header file
//

#include "ut/OXCoolToolBar.h"

#define WM_PRESS WM_USER + 1

/////////////////////////////////////////////////////////////////////////////
// CItemToolBar window

class CItemToolBar : public 
						#if 0
							COXCoolToolBar			
						#else
							CToolBar				
						#endif
{
// Construction
public:
	CItemToolBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CItemToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CItemToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CItemToolBar)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	afx_msg LPARAM OnPressButton(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ITEMTOOLBAR_H__1ECE7364_E68C_4712_AF72_65DA01C4BCA6__INCLUDED_)
