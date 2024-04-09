// ScaleComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "depcfg.h"
#include "ScaleComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleComboBox

CScaleComboBox::CScaleComboBox()
{
}

CScaleComboBox::~CScaleComboBox()
{
}


BEGIN_MESSAGE_MAP(CScaleComboBox, CComboBox)
	//{{AFX_MSG_MAP(CScaleComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditchange)
	ON_CONTROL_REFLECT(CBN_EDITUPDATE, OnEditupdate)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleComboBox message handlers

int CScaleComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	CString strText;
	AddString(_T("150%"));
	AddString(_T("100%"));
	AddString(_T("75%"));
	AddString(_T("50%"));
	AddString(_T("25%"));
	AddString(_T("10%"));

	strText.LoadString( IDS_FIT_TO );
	AddString( strText );
//	AddString(_T("Fit to"));
	SetCurSel(1);

	LOGFONT lf;
	GetParent()->GetFont()->GetLogFont(&lf);

	m_Font.CreateFontIndirect(&lf);
	SetFont( &m_Font );
	return 0;
}

void CScaleComboBox::OnEditchange() 
{
	// TODO: Add your control notification handler code here
	
}

void CScaleComboBox::OnEditupdate() 
{
	// TODO: Add your control notification handler code here
	
}

void CScaleComboBox::OnSelchange() 
{
	// TODO: Add your control notification handler code here
	int nIndex = GetCurSel();
	float size;
	switch( nIndex )

	{
	case 0:	size = 1.5f ;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 1:	size = 1.0f ;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 2: size = 0.75f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 3: size = 0.5f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 4: size = 0.25f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 5: size = 0.1f;
		{	
			AfxGetMainWnd()->SendMessage(WM_SCALE, *(DWORD*)&size);
			break;
		}
	case 6: 
		{	
			AfxGetMainWnd()->SendMessage(WM_FIT);
			SetCurSel(-1);
			break;
		}
	}
}

DWORD	CScaleComboBox::GetDesireWidth() 
{
//	CResourceMgr & mgr = GetResourceMgr();
	CDC* pDC = GetDC();
	CString strText;
	CRect rcItem;
	DWORD dwWidth = 0;
	TCHAR	szBuf[MAX_PATH];
	//NOTENOTE: use current font in m_font
	for( int i=0; i < GetCount(); ++i )
	{
		//NOTENOTE: if has CBS_HASSTRINGS style
		int z = GetLBText(i, szBuf);
//		LPCTSTR szText = (LPCTSTR)GetItemData( i );
		pDC->DrawText( szBuf, z, rcItem,  DT_CENTER|DT_SINGLELINE|DT_VCENTER|DT_CALCRECT );
		dwWidth = std::max<DWORD>( rcItem.Width(), dwWidth );
	}
	//NOTENOTE: add gap in 20px but hack
	//FIXME: 
	if(dwWidth < 50)
	dwWidth += 15;
	ReleaseDC(pDC);
	return dwWidth;
}