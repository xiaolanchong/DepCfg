// mysizeba.h : header file
//
// Docktest demo of Dockable Views functionality
// CMyListBoxBar window

#include "ut\OXSizeCtrlBar.h"
#include "PropertyListBox.h"

#define ID_LISTBOXBAR 10011

class CMyListBoxBar : public COXSizeControlBar
{
// Construction
public:
	BOOL Create(CWnd * pParentWnd, const CString& sTitle = _T(""),
		const UINT nID=ID_LISTBOXBAR);
	DECLARE_DYNAMIC(CMyListBoxBar);

	CMyListBoxBar();

// Attributes
public:
	CPropertyListBox  		m_wndListBox;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListBoxBar)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	protected:
		virtual void OnSizedOrDocked(int cx, int cy, BOOL bFloating, int flags);

// Implementation
public:
	virtual ~CMyListBoxBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListBoxBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
