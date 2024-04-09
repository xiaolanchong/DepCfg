// DepCfgWnd.cpp : implementation file
//

#include "stdafx.h"

#include "../include/DepCfgWnd.h"
#include "Docview/PhysicLiteView.h"
#include "../Common/utility/Helper.h"
#include <afxcview.h>
//using namespace Elvees::Win32;

#include "interface/DepInterface.h"
#include "interface/IResourceLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgWnd


CDepCfgWnd::CDepCfgWnd()
{
}

CDepCfgWnd::~CDepCfgWnd()
{
}


BEGIN_MESSAGE_MAP(CDepCfgWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CDepCfgWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDepCfgWnd message handlers

BOOL CDepCfgWnd::Create( DWORD dwStyle, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL Result = CSplitterWnd::CreateStatic(pParentWnd, 2, 1, dwStyle, nID);
	if( !Result) return Result;

	CreateView( 0, 0, RUNTIME_CLASS(CPhysicLiteView), CSize(50, 500), pContext );
	CreateView( 1, 0, RUNTIME_CLASS(CListView), CSize(20, 100), pContext );


	m_pList = &static_cast< CListView* >( GetPane( 1, 0 ) )->GetListCtrl();
	m_pView = static_cast< CPhysicLiteView* >( GetPane( 0, 0 ) );

	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);

	m_ilNormal.Create(16,16, ILC_MASK, 1, 1);
	HICON hIc = p->LoadIconInt( IDI_INFO_OBJECT);
	m_ilNormal.Add( hIc  );
	
	m_pList->ModifyStyle(  0, LVS_REPORT/*|LVS_SORTDESCENDING*/ );
	m_pList->SetImageList( &m_ilNormal, LVSIL_SMALL );
	CString strColumn;

	strColumn = p->LoadStringInt( IDS_COLUMN_TIME );
	m_pList->InsertColumn( 0, strColumn, LVCFMT_LEFT, 150  );
	strColumn = p->LoadStringInt( IDS_COLUMN_OBJECT );
	m_pList->InsertColumn( 1, strColumn, LVCFMT_LEFT, 150  );
	strColumn = p->LoadStringInt( IDS_COLUMN_INFO );
	m_pList->InsertColumn( 2, strColumn, LVCFMT_LEFT, 300 );

	
	m_pList->SetExtendedStyle( LVS_EX_FULLROWSELECT );
	m_pView->ConnectTeleCallback( this, TRUE );

	return TRUE;
}

void	CDepCfgWnd::ZoomIn()
{
	m_pView->ZoomIn();
}

BOOL	CDepCfgWnd::CheckZoomIn() const
{
	return m_pView->CheckZoomIn();
}
	
void	CDepCfgWnd::ZoomOut()
{
	m_pView->ZoomOut();
}

BOOL	CDepCfgWnd::CheckZoomOut() const
{
	return m_pView->CheckZoomOut();
}

void	CDepCfgWnd::FitToSize()
{
	m_pView->FitToSize();
}

void	CDepCfgWnd::State(void* p)
{
	m_pView->State(p);
}


BOOL	CDepCfgWnd::SetProjectFile(	LPCWSTR path,	// Full file name of configuration
							DWORD TimeOutSec )	// Timeout, sec
{
	return m_pView->SetProjectFile( path, TimeOutSec );
//	return
}
	
bool	CDepCfgWnd::ConnectTeleCallback(ITeleCallback* pCallback)
{
	return m_pView->ConnectTeleCallback( pCallback, FALSE );
}

bool	CDepCfgWnd::DisconnectTeleCallback(ITeleCallback* pCallback )
{
	return m_pView->DisconnectTeleCallback( pCallback, FALSE);
}

bool	CDepCfgWnd::OpenFromDatabase(CStringW sConnectionString)
{
	return m_pView->OpenFromDatabase( sConnectionString );
}

///////////////////////////////////////////////////////////////////


static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   // lParamSort contains a pointer to the list view control.
   // The lParam of an item is just its index.
	CListCtrl* pListCtrl = (CListCtrl*) lParamSort;
	CString    strItem1 = pListCtrl->GetItemText(lParam1, 0);
	CString    strItem2 = pListCtrl->GetItemText(lParam2, 0);

	COleDateTime time1 =  COleVariant(strItem1) ;
	COleDateTime time2 = COleVariant(strItem2) ;

	return time2 > time1 ? -1 : 1;
}

void CDepCfgWnd::OnCamera(LONG id,          // camera id
                        LPCWSTR state     // new state
                        ) 
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);


	int nIndex = m_pList->GetItemCount();

	CString strTime = COleDateTime::GetCurrentTime().Format(/*_T("%H-%M-%S %d.%m.%Y")*/LOCALE_NOUSEROVERRIDE, LCID(LANG_USER_DEFAULT) );

	CString Fmt;
	CString FormatString = p->LoadStringInt( IDS_EVENT_CAMERA ); 
	Fmt.Format( FormatString, id );
	m_pList->InsertItem(nIndex, strTime, 0);
	m_pList->SetItem(nIndex, 1,LVIF_TEXT, Fmt,NULL,NULL,NULL,NULL);

	CString strState = Helper::Convert( std::wstring(state) );
	m_pList->SetItem(nIndex, 2,LVIF_TEXT, strState,NULL,NULL,NULL,NULL);

	m_pList->SortItems( MyCompareProc, reinterpret_cast<DWORD>( m_pList ) );
}

void CDepCfgWnd::OnPTZ(LONG id,             // camera id
                     LPCWSTR state        // new state
                     ) 
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);

	int nIndex = m_pList->GetItemCount();

	CString Fmt;
	CString FormatString = p->LoadStringInt( IDS_EVENT_PTZ ); 
	Fmt.Format( FormatString, id );

	CString strTime = COleDateTime::GetCurrentTime().Format(/*_T("%H-%M-%S %d.%m.%Y")*/LOCALE_NOUSEROVERRIDE, LCID(LANG_USER_DEFAULT) );
	m_pList->InsertItem(nIndex, strTime, 0);
	m_pList->SetItem(nIndex, 1,LVIF_TEXT, Fmt,NULL,NULL,NULL,NULL);
	CString strState = Helper::Convert( std::wstring(state) );
	m_pList->SetItem(nIndex, 2,LVIF_TEXT, strState,NULL,NULL,NULL,NULL);

	m_pList->SortItems( MyCompareProc, reinterpret_cast<DWORD>( m_pList ) );
}

void CDepCfgWnd::OnComputer(LPCWSTR name,   // computer name
                          LPCWSTR state   // new state
                          ) 
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);

	int nIndex = m_pList->GetItemCount();

	CString Fmt;

	CString FormatString = p->LoadStringInt( IDS_EVENT_COMPUTER ); 
	Fmt.Format( FormatString, (LPCTSTR)name );

	CString strTime = COleDateTime::GetCurrentTime().Format(/*_T("%H-%M-%S %d.%m.%Y")*/LOCALE_NOUSEROVERRIDE, LCID(LANG_USER_DEFAULT) );
	m_pList->InsertItem(nIndex, strTime, 0);
	m_pList->SetItem(nIndex, 1,LVIF_TEXT, Fmt,NULL,NULL,NULL,NULL);
	CString strState = Helper::Convert( std::wstring(state) );
	m_pList->SetItem(nIndex, 2,LVIF_TEXT, strState,NULL,NULL,NULL,NULL);
	
	m_pList->SortItems( MyCompareProc, reinterpret_cast<DWORD>( m_pList ) );
}
