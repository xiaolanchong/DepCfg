#if !defined(AFX_DEPCFGWND_H__435C13F3_CF08_415F_A051_00969393829B__INCLUDED_)
#define AFX_DEPCFGWND_H__435C13F3_CF08_415F_A051_00969393829B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DepCfgWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDepCfgWnd window

#include "Telecallback.h" 

class CPhysicLiteView;
class CListCtrl;

class AFX_EXT_CLASS CDepCfgWnd : public CSplitterWnd, private ITeleCallback
{
	CPhysicLiteView*	m_pView;
	CListCtrl*		m_pList;

	CImageList	m_ilNormal;	
// Construction
public:
	CDepCfgWnd();

// Attributes
public:

	void	ZoomIn();						//!< zoom in the view
	BOOL	CheckZoomIn() const;			//!< Can the view be zoomed in
	
	void	ZoomOut();						//!< zoom out the view
	BOOL	CheckZoomOut() const;			// < Can the view be zoomed out

	void	FitToSize();					//!< fit content to window client sizes

	//! info from events.dll, see sysstate.h
	//! \param	p	data from upper level provider
	void	State(void* p);					

	//! \param	path		full file name of the configuration
	//! \param	TimeOutSec	Timeout, when device will be offline, sec
	//! \return	success or failing	
	BOOL	SetProjectFile(	LPCWSTR path,	
							DWORD TimeOutSec = 5);	
	
	//! Connent observer to view
	//! \param	pCallback	registered interface pointer, if empty - connection string loads as usual
	//! \return success or failing	
	bool	ConnectTeleCallback(ITeleCallback* pCallback);

	//! Disconnect from the state observer
	//! \param	pCallback	registered interface pointer, if NULL disconnect all
	//! \return success or failing
	bool	DisconnectTeleCallback(ITeleCallback* pCallback = NULL);

	//! Open the configuration from the database
	//! \param	sConnectionString	database connection string, 
	//!								if the one is empty, DBBridge connection string is used instead
	//! \return success or failing
	bool	OpenFromDatabase(CStringW sConnectionString);				

private:
  virtual void OnCamera(LONG id,          // camera id
                        LPCWSTR state     // new state
                        ) ;
  virtual void OnPTZ(LONG id,             // camera id
                     LPCWSTR state        // new state
                     ) ;
  virtual void OnComputer(LPCWSTR name,   // computer name
                          LPCWSTR state   // new state
                          ) ; 

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgWnd)
	public:
	virtual BOOL Create( DWORD dwStyle, CWnd* pParentWnd, UINT nID= AFX_IDW_PANE_FIRST , CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDepCfgWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDepCfgWnd)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFGWND_H__435C13F3_CF08_415F_A051_00969393829B__INCLUDED_)
