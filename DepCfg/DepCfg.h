// DepCfg.h : main header file for the DEPCFG application
//

#if !defined(AFX_DEPCFG_H__5D17343A_487A_463F_8AE2_A8A5224F542E__INCLUDED_)
#define AFX_DEPCFG_H__5D17343A_487A_463F_8AE2_A8A5224F542E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDepCfgApp:
// See DepCfg.cpp for the implementation of this class
//
#include  <gdiplus.h>
using namespace Gdiplus;

class CDepCfgApp : public CWinApp
{
	class GdiPlusInit
	{
		ULONG_PTR					gdiplusToken;
	public:
		GdiPlusInit()
		{
			GdiplusStartupInput gdiplusStartupInput;
			// Initialize GDI+.
			GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		}
		~GdiPlusInit()
		{
			GdiplusShutdown(gdiplusToken);
		}
	} m_GdiInit;
public:
	CDepCfgApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CDepCfgApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFG_H__5D17343A_487A_463F_8AE2_A8A5224F542E__INCLUDED_)
