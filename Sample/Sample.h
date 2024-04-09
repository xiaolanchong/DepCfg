// LiteCfg.h : main header file for the LITECFG application
//

#if !defined(AFX_LITECFG_H__876560D4_B783_4730_9B6A_CEC196744C30__INCLUDED_)
#define AFX_LITECFG_H__876560D4_B783_4730_9B6A_CEC196744C30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLiteCfgApp:
// See LiteCfg.cpp for the implementation of this class
//

class CLiteCfgApp : public CWinApp
{
public:
	CLiteCfgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLiteCfgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CLiteCfgApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LITECFG_H__876560D4_B783_4730_9B6A_CEC196744C30__INCLUDED_)
