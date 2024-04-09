// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__E62B04B4_15D0_4826_BAD5_5E065B5BED46__INCLUDED_)
#define AFX_MAINFRM_H__E62B04B4_15D0_4826_BAD5_5E065B5BED46__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef ORWELL_CAM
#include "..\..\..\Events\trunk\Events.h"
#endif

#include "DepCfgView.h"

#ifdef ORWELL_CAM
using namespace Elvees::Win32;
#endif

class CMainFrame : public CFrameWnd, private ITeleCallback
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	//}}AFX_VIRTUAL

// Implementation

#ifdef ORWELL_CAM
	static void __cdecl CallbackProc(
	EOrwEvent eventID,    // see above
	void *pData,          // event specific data
	long datalen); 
#endif
	
	static		CMainFrame * pThis; 	
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CDepCfgView*    m_wndView;

void OnCamera(LONG id,          // camera id
                        LPCWSTR state     // new state
                        )
{
//	AfxMessageBox(state  );
	OutputDebugStringW( state );
	OutputDebugStringW( L"\n");
}

void OnPTZ(LONG id,             // camera id
                     LPCWSTR state        // new state
                     ) 
{
	OutputDebugStringW( state );
	OutputDebugStringW( L"\n");
}

void OnComputer(LPCWSTR name,   // computer name
                          LPCWSTR state   // new state
                          ) 

{
	OutputDebugStringW( state );
	OutputDebugStringW( L"\n");
}


// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnZoomin();
	afx_msg void OnUpdateZoomin(CCmdUI* pCmdUI);
	afx_msg void OnZoomout();
	afx_msg void OnUpdateZoomout(CCmdUI* pCmdUI);
	afx_msg void OnFitto();
	void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__E62B04B4_15D0_4826_BAD5_5E065B5BED46__INCLUDED_)
