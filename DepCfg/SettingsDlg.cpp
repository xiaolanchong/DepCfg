// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "depcfg.h"
#include "SettingsDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/, 
						   CString strServer, 
						   CString strDBName, 
						   CString strDir,
						   DWORD	dwVideoStandard)
	:	CDialog(CSettingsDlg::IDD, pParent), 
		m_strServer(strServer), 
		m_strDBName(strDBName), 
		m_strSharedDir(strDir),
		m_dwVideoStandard(dwVideoStandard)
{
	//{{AFX_DATA_INIT(CSettingsDlg)
	//}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsDlg)
	DDX_Control(pDX, IDC_VIDEO_STANDARD, m_wndVideoStandard);
	DDX_Text(pDX, IDC_SERVER, m_strServer);
	DDX_Text(pDX, IDC_DATABASE, m_strDBName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(CSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers


BOOL CSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	CRect rc;
	CWnd* pWnd = GetDlgItem(IDC_EDIT_DIRECTORY);
	m_Edit.SubclassWindow(pWnd->GetSafeHwnd());
	m_Edit.SetWindowText( m_strSharedDir );

	CString strVideo;
	strVideo.LoadString( IDS_VIDEO_STANDARD_0 );
	m_wndVideoStandard.InsertString( 0, strVideo );
	strVideo.LoadString( IDS_VIDEO_STANDARD_1 );
	m_wndVideoStandard.InsertString( 1, strVideo );
	m_wndVideoStandard.SetCurSel( m_dwVideoStandard );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	m_Edit.GetWindowText(m_strSharedDir);

	m_dwVideoStandard = m_wndVideoStandard.GetCurSel();
	CDialog::OnOK();
}
