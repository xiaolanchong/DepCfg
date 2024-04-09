// DepCfgDoc.h : interface of the CDepCfgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPCFGLITEDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
#define AFX_DEPCFGLITEDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include "../include/Telecallback.h"
#include "../Common/docview/DepCfgDoc.h"


struct XMLElem;
class  CIniFile; 

class CDepCfgLiteDoc : public CDepCfgDoc
{
	DWORD	m_dwTimeOutSec;	
protected: // create from serialization only
	CDepCfgLiteDoc();
	DECLARE_DYNCREATE(CDepCfgLiteDoc)

	virtual void	DeleteItems(const std::set<DWORD> & Items);

public:
	std::vector<ITeleCallback*>	m_Observers;
	std::vector<ITeleCallback*>	m_InternalObservers;

	BOOL	Load( LPCWSTR path, DWORD TimeOutSec);
	bool	LoadFromDB(CStringW sConnectionString, DWORD TimeOutSec);
	BOOL	ParseState( void* pData );

	BOOL	Tick();
	bool	Connect( ITeleCallback* pCallback , BOOL bInternal);
	bool	Disconnect( ITeleCallback* pCallback, BOOL bInternal);

private:

	void	InitTicks();

	std::map<DWORD, DWORD >	m_Ticks;
	std::map<DWORD, bool >	m_FirstTimes;

	void	FireCompEvent( CString name, CString state );
	void	FireCameraEvent( LONG id, CString state );
	void	FireSlaveEvent( LONG id, CString state );

public:	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgLiteDoc)
	public:
//	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDepCfgLiteDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDepCfgDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFGLITEDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
