// DepCfgDoc.h : interface of the CDepCfgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPCFGDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
#define AFX_DEPCFGDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../item/ProjectDB.h"
#include <istream>
#include <sstream>
#include <ostream>

MACRO_EXCEPTION(DocumentException, CommonException);
MACRO_EXCEPTION(ExportException, DocumentException);
MACRO_EXCEPTION(ImportException, DocumentException);
//MACRO_EXCEPTION(ImportException, DocumentException);

struct XMLElem;
class  CIniFile; 

inline CString GetProperty(const Property<CString>& p)
{
	return p.IsSet() ? p.Get() : CString();
}

inline CString GetProperty(const Property<DWORD>& p)
{
	CString s;
	if(p.IsSet()) s.Format( _T("%d"), p.Get() );
	return s;
}

inline CString GetProperty(const Property<bool>& p)
{
	CString s;
	if(p.IsSet()) s.Format( _T("%d"), p.Get() );
	return s;
}

inline CString GetProperty(DWORD p)
{
	CString s;
	s.Format( _T("%d"), p );
	return s;
}

inline void SetAttr(const std::wstring& Value, Property<DWORD>& p)
{
	if ( !Value.empty() ) p = (DWORD)_wtoi(Value.c_str());
}

inline void SetAttr(const std::wstring& Value, Property<CString>& p)
{
	if ( !Value.empty() ) p = /*Helper::Convert*/(Value.c_str());
}

inline void SetAttr(const std::wstring& Value, Property<bool>& p)
{
	if ( !Value.empty() ) p = _wtoi(Value.c_str()) > 0;
}


class CDepCfgDoc : public CDocument
{
protected: // create from serialization only
	CDepCfgDoc();
	DECLARE_DYNCREATE(CDepCfgDoc)

	CStringW	LoadStringInt( UINT nID );
// Attributes
public:

// Operations
public:
	CLogicDB	m_DB;
	CGuiDB		m_LogicDB, m_PhysicDB;

	virtual void	DeleteItems(const std::set<DWORD> & Items);

protected:

	void	Load(const std::vector<char>& os);
#ifdef USE_OLD_FORMAT
	void	LoadProject(XMLElem* e, double fVersionNumber);
#endif
	void	LoadComputer(XMLElem* e, double fVersionNumber);
	void	LoadSlave(XMLElem* e, double fVersionNumber);
	void	LoadMaster(XMLElem* e, double fVersionNumber);
	void	LoadConnection(XMLElem* e, double fVersionNumber);
	void	LoadCommutator(XMLElem* e, double fVersionNumber);
#ifndef USE_AUTO_COMP2COMM
	void	LoadComp2Comm(XMLElem* e, double fVersionNumber);
#endif
	void	LoadGuiComputer(XMLElem* e, double fVersionNumber);
	void	LoadGuiPhySlave(XMLElem* e, double fVersionNumber);
	void	LoadGuiPhyMaster(XMLElem* e, double fVersionNumber);
	void	LoadGuiLogSlave(XMLElem* e, double fVersionNumber);
	void	LoadGuiLogMaster(XMLElem* e, double fVersionNumber);
	void	LoadGuiCommutator(XMLElem* e, double fVersionNumber);
	void	LoadGuiConnection(XMLElem* e, CGuiDB& db, double fVersionNumber);

	SizeF	GetPhysicPos() const;
	SizeF	GetLogicPos() const;

public:	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDepCfgDoc();
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

#endif // !defined(AFX_DEPCFGDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
