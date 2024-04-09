#if !defined(AFX_SETTINGSDLG_H__716C12ED_643B_40DF_836A_2B566E8D30C6__INCLUDED_)
#define AFX_SETTINGSDLG_H__716C12ED_643B_40DF_836A_2B566E8D30C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

#include "ut/oxedit.h"
#include "ut/OXDropEdit.h"
#include "DirDialog.h"

class CBrowseEdit : public COXDropEdit<COXEdit>
{
public:

	void OnDropButton()
	{
		CDirDialog dlg;
		dlg.m_strTitle = _T(" ");
		GetWindowText(dlg.m_strSelDir );
		if( dlg.DoBrowse(this) )
		{
			SetWindowText(dlg.m_strPath );
		}
	}
};

class CSettingsDlg : public CDialog
{
// Construction
	//CButton			m_Button;
	CBrowseEdit			m_Edit;
		
	
public:
	CSettingsDlg(CWnd* pParent = NULL, 
				 CString strServer = _T(""), 
				 CString strDBName = _T(""), 
				 CString strDir=_T(""),
				 DWORD	 dwVideoStandard = 0);   // standard constructor

	CString m_strSharedDir;
// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	CComboBox	m_wndVideoStandard;
	CString	m_strServer;
	CString	m_strDBName;
	DWORD	m_dwVideoStandard;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	afx_msg void OnDirectory();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__716C12ED_643B_40DF_836A_2B566E8D30C6__INCLUDED_)
