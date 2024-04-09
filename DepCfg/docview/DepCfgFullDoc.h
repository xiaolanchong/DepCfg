// DepCfgFullDoc.h : interface of the CDepCfgDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEPCFGFULLDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
#define AFX_DEPCFGFULLDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../Common/docview/DepCfgDoc.h"
#include "..\..\common\utility\XMLparser.h"

class  CIniFile; 
struct XMLElem;

MACRO_EXCEPTION  (XMLVersionDBException, XMLException)

class CDepCfgFullDoc : public CDepCfgDoc
{
protected: // create from serialization only
	CDepCfgFullDoc();
	DECLARE_DYNCREATE(CDepCfgFullDoc)

// Attributes
public:

// Operations
public:
#if 0
	virtual void	DeleteItems(const std::set<DWORD> & Items);
#endif
protected:

	mutable class Report
	{
		std::vector<CString> m_Strings;
	public:
		CString GetReport() const;
		void	AddString(CString str) { m_Strings.push_back(str);}
		void	Clear() { m_Strings.clear();}
	} m_Report;


	void	Save(std::ostream& os)	const;
#ifdef USE_OLD_FORMAT
	void	SaveProject(std::ostream& os) const;
#endif
	void	SaveComputer(std::ostream& os) const;
	void	SaveSlave(std::ostream& os) const;
	void	SaveMaster(std::ostream& os) const;
	void	SaveConnection(std::ostream& os) const;
	void	SaveCommutator(std::ostream& os) const;
#ifndef USE_AUTO_COMP2COMM
	void	SaveComp2Comm(std::ostream& os) const;
#endif
	void	SaveGuiBase(std::ostream& os, const CGuiDB& db) const;

	void	VerifyComputer() const;
#ifdef USE_OLD_FORMAT
	void	VerifyProject() const;
#endif
	void	VerifyCamera() const;
	void	VerifyCommutator() const;
#ifdef USE_OLD_FORMAT
	void	ExportProject(CIniFile& iniFile) const;
#endif
	void	ExportCommutator(CIniFile& iniFile) const;
	void	ExportMaster(CIniFile& iniFile) const;
	void	ExportSlave(CIniFile& iniFile) const;

	bool	Verify();

	struct ProjectImport
	{
		DWORD	m_dwCameraNumber, m_dwCommutatorNumber, m_dwVideoStandard;
	};
#ifdef USE_OLD_FORMAT
	ProjectImport	ImportProject( const CIniFile& iniFile )					throw (ImportException);
#endif
	void	ImportCommutator( const CIniFile& iniFile, DWORD dwCount )	throw (ImportException);
	void	ImportCommonCamera( const std::string Section, const CIniFile& iniFile, CameraSettings&cs , bool master );
	void	ImportCamera( const CIniFile& iniFile, DWORD dwCount )		throw (ImportException);

public:	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDepCfgFullDoc)
	public:
//	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual void DeleteContents();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDepCfgFullDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDepCfgFullDoc)
	afx_msg void OnItemCommutator();
	afx_msg void OnItemComputer();
	afx_msg void OnItemMaster();
	afx_msg void OnItemSlave();
	afx_msg void OnFileExport();
	afx_msg void OnFileImport();
	afx_msg void OnSettings();

	afx_msg void OnFileDBSave();
	afx_msg void OnFileDBLoad();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEPCFGDOC_H__9A6D8942_955D_469E_9369_12EA5FF4B0F3__INCLUDED_)
