// PhysicLiteView.h : interface of the CPhysicLiteView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PHYSICLITEVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
#define AFX_PHYSICLITEVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786)

#include "../../common/docview/PhysicView.h"
#include "../include/Telecallback.h"
#include <afxmt.h>
#include <afxtempl.h>

class CDepCfgLiteDoc;

class CPhysicLiteView : public CPhysicView
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

	CCriticalSection						m_CriticalSection;
protected:
	CDepCfgLiteDoc* GetDocument();
public:

	void	ZoomIn();						// zoom view
	BOOL	CheckZoomIn() const;			// Can view be zoomed
	
	void	ZoomOut();						// zoom out view
	BOOL	CheckZoomOut() const;			// 

	void	FitToSize();					// fit content to window client sizes

	void	State(void* p);					// info from events.dll, see sysstate.h
	BOOL	SetProjectFile(	LPCWSTR path,	// Full file name of configuration
							DWORD TimeOutSec = 5);	// Timeout, sec
	
	bool	ConnectTeleCallback(ITeleCallback* pCallback, BOOL bInternal);		// Connent observer to view
	bool	DisconnectTeleCallback(ITeleCallback* pCallback , BOOL bInternal);	// if NULL disconnect all
	
	bool	OpenFromDatabase(CStringW sConnectionString);

	CPhysicLiteView();
protected: // create from serialization only
	DECLARE_DYNCREATE(CPhysicLiteView)
public:
	virtual ~CPhysicLiteView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CPhysicView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PHYSICLITEVIEW_H__0F3E43A4_CF83_4B32_AE41_1256D475B8A3__INCLUDED_)
