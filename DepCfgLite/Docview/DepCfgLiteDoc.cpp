// DepCfgDoc.cpp : implementation of the CDepCfgDoc class
//
#pragma  warning(disable : 4786)
#include "stdafx.h"

#include "DepCfgLiteDoc.h"
#include "../Common/utility/Helper.h"
#include "..\interface\DepInterface.h"
#include "..\interface\IDBSerializer.h"
#include "..\interface\IResourceLoader.h"
#include "..\..\common\utility\XMLParser.h"

#include <atlbase.h>

//#include "EditorView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDepCfgLiteDoc

IMPLEMENT_DYNCREATE(CDepCfgLiteDoc, CDepCfgDoc)

BEGIN_MESSAGE_MAP(CDepCfgLiteDoc, CDepCfgDoc)
	//{{AFX_MSG_MAP(CDepCfgDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc construction/destruction

CDepCfgLiteDoc::CDepCfgLiteDoc(): m_dwTimeOutSec(5)
{
	// TODO: add one-time construction code here
}

CDepCfgLiteDoc::~CDepCfgLiteDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc serialization

BOOL	CDepCfgLiteDoc::Load( LPCWSTR path, DWORD TimeOutSec )
{
	try
	{
		CString strFileName = Helper::Convert( std::wstring( path ) );
		CFile File( strFileName, CFile::modeRead);
		int size = File.GetLength();
		std::vector<char> v;
		v.resize( size +1 );
		
		File.Read( &v[0], size );
		v.push_back('\0');

		CDepCfgDoc::Load(v);
	}
	catch(CommonException)
	{
	return FALSE;
	}
	catch ( CFileException* pEx)
	{
		pEx->Delete();
		return FALSE;
	}
	m_dwTimeOutSec = TimeOutSec;
	InitTicks();	
	return TRUE;
}

bool	CDepCfgLiteDoc::LoadFromDB(CStringW sConnectionString, DWORD TimeOutSec)
try
{
	std::shared_ptr< DepCfg::IDBSerializer > pInt = 
		DepCfg::CreateInternalInterface<DepCfg::IDBSerializer>( DBSERIALIZER_INTERFACE );

	std::wstring sCS( sConnectionString.IsEmpty() ? L"" : (LPCWSTR)sConnectionString );
	std::vector<BYTE> BinArr;
	pInt->LoadFile( sCS, BinArr );
	std::vector<char> CharArr( BinArr.begin(), BinArr.end() );
	CharArr.push_back( '\0' );

	CDepCfgDoc::Load(CharArr);
	m_dwTimeOutSec = TimeOutSec;
	InitTicks();
	return true;
}
catch(XMLException)
{
	return false;
}
catch(DatabaseException)
{
	return false;
};


/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc diagnostics

#ifdef _DEBUG
void CDepCfgLiteDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDepCfgLiteDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////////////////


void	CDepCfgLiteDoc::DeleteItems(const std::set<DWORD> & Items)
{
	CDepCfgDoc::DeleteItems(Items);
	m_Ticks.clear();
}

/////////////////////////////////////////////////////////////////////////

BOOL	CDepCfgLiteDoc::ParseState( void* pData )
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);

	
	BOOL Result = FALSE;
	ComputerState* pCompState = (ComputerState*)pData;
	
	const char* sHostName = pCompState->sHostName;

#ifdef _UNICODE
	WCHAR buf[MAX_PATH];
	mbstowcs( buf, sHostName, MAX_PATH );
#else 
	LPCSTR buf = sHostName; 
#endif

	if( !lstrlen(buf) ) return Result;

	std::set<ComputerSettings>::iterator cs = m_DB.m_Computers.begin();
	for( ;cs != m_DB.m_Computers.end(); ++cs )
	{
		//NOTENOTE: 10/12/2004 change Compare string - not IP but DNSName
		const Property<CString>& propDnsName = cs->m_strDnsName; // not strIP
		const Property<CString>& propIPName = cs->m_strIP;

		CString strDnsName	= propDnsName.IsSet() ? propDnsName.Get() : _T("");
		CString strIPName	= propIPName.IsSet() ? propIPName.Get() : _T("");

		if(  !strDnsName.IsEmpty() || !strIPName.IsEmpty() )
		{			
			int resDns	= CompareString (LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, strDnsName, -1, buf, -1 );
			int resIP	= CompareString (LOCALE_SYSTEM_DEFAULT, NORM_IGNORECASE, strIPName, -1, buf, -1 ); 
			
			LPCTSTR strResName = 0;
			if( resDns  == CSTR_EQUAL )		strResName = strDnsName;
			else if( resIP == CSTR_EQUAL)	strResName = strIPName;

			if( resDns == CSTR_EQUAL || resIP == CSTR_EQUAL )
			{
				m_Ticks[ cs->GetID() ] = 0 ;
				DWORD OldState = cs->m_dwNetState.Get();
				((ComputerSettings&)*cs).m_dwNetState = pCompState->sBattery;
				Result = TRUE;

				CString strState = p->LoadCompState( pCompState->sBattery );
				if( OldState != pCompState->sBattery && pCompState->sBattery != sBattery_online )
				{
					FireCompEvent( strResName, strState );
				}
				break;
			}
		}
	}

	int Cams= pCompState->sCamera;

	for( int i =0 ; i < Cams;++i )
	{
		CameraState* pCamState = (CameraState*)((BYTE*)pData + sizeof(ComputerState) + sizeof(CameraState) * i);
		MasterSettings * ms = m_DB.GetMasterFromCameraID( pCamState->csCameraID );
		SlaveSettings * ss = m_DB.GetSlaveFromCameraID( pCamState->csCameraID );

		if( ms )
		{
			Property<ECamState> OldState = ms->m_NetState;
			m_Ticks[ms->GetID()] = 0;
			ms->m_NetState = pCamState->csCamState;
			Result = TRUE;
			CString strState = p->LoadCameraState(pCamState->csCamState );
			if( m_FirstTimes[ms->GetID()])
			{
				if(!OldState.IsSet() || OldState.Get() != pCamState->csCamState )
				{
					// filter event
					if( pCamState->csCamState != ECameraNoError )
						FireCameraEvent( pCamState->csCameraID, strState);
				}
				
			}
			else
			{
				m_FirstTimes[ms->GetID()] = true;
			}
		}
		else if(ss)
		{
			Property<ECamState> OldState = ss->m_NetState;
			Property<EPTZState> OldPtzState = ss->m_PTZState;
			m_Ticks[ss->GetID()] = 0;
			ss->m_NetState = pCamState->csCamState;
			ss->m_PTZState = pCamState->csPTZState;
			Result = TRUE;
			CString strState, strPtz;
			strState = p->LoadCameraState(pCamState->csCamState );
			strPtz = p->LoadPtzState( pCamState->csPTZState );
			if( m_FirstTimes[ss->GetID()] )
			{
				if( !OldState.IsSet() || OldState.Get() != pCamState->csCamState)	
				{
					// filter event
					if( pCamState->csCamState != ECameraNoError )
						FireCameraEvent( pCamState->csCameraID, strState );
				}
				if( !OldPtzState.IsSet() || OldPtzState.Get() != pCamState->csPTZState )	
				{
					// filter event
					if( pCamState->csPTZState != EPTZNoError )
						FireSlaveEvent( pCamState->csCameraID, strPtz );
				}
			}
			else
			{
				m_FirstTimes[ss->GetID()] = true;
			}
		}
	}
	return Result;
}

void	CDepCfgLiteDoc::InitTicks()
{
	m_Ticks.clear();
	m_FirstTimes.clear();
	std::set<ComputerSettings>::iterator cs = m_DB.m_Computers.begin();
	for( ;cs != m_DB.m_Computers.end(); ++cs )
	{
		m_Ticks[ cs->GetID() ] = 0 ;
		((ComputerSettings&)*cs).m_dwNetState = sBattery_offline;
	}

	std::set<MasterSettings>::iterator mi = m_DB.m_Masters.begin();
	for( ;mi != m_DB.m_Masters.end(); ++mi )
	{
		m_Ticks[ mi->GetID() ] = 0 ;
		m_FirstTimes[mi->GetID() ] = false;
	}

	std::set<SlaveSettings>::iterator si = m_DB.m_Slaves.begin();
	for( ;si != m_DB.m_Slaves.end(); ++si )
	{
		m_Ticks[ si->GetID() ] = 0 ;
		m_FirstTimes[si->GetID() ] = false;
	}
}

BOOL	CDepCfgLiteDoc::Tick()
{

	BOOL Result = FALSE;
	std::map<DWORD, DWORD>::iterator itr = m_Ticks.begin();
	for( ;itr != m_Ticks.end(); ++itr )
	{
		DWORD dwTicks = ++itr->second;

		ComputerSettings* cs = m_DB.GetComputer( itr->first );
		MasterSettings* ms = m_DB.GetMaster( itr->first );
		SlaveSettings* ss = m_DB.GetSlave( itr->first );

		if(dwTicks < m_dwTimeOutSec * 1000) continue;
		
		if( cs && cs->m_dwNetState.Get() != sBattery_offline)
		{
			DWORD OldState = cs->m_dwNetState.Get();
			cs->m_dwNetState = sBattery_offline; // offline
			Result = TRUE;

			CString strState = LoadStringInt( IDS_PROP_COMP_OFFLINE );

			if(OldState != sBattery_offline)
				FireCompEvent( cs->m_strIP.Get(), strState );
		}
		else if( ms && ms->m_NetState.IsSet())
		{
			DWORD OldState = ms->m_NetState.Get();
			ms->m_NetState.Reset();
			Result = TRUE;

			CString strState = LoadStringInt( IDS_PROP_NODATA );
		//	if(OldState != sBattery_offline)
				FireCameraEvent( ms->m_dwCameraID.Get(), strState );
		}
		else if(ss && ss->m_NetState.IsSet())
		{
		//	DWORD OldState = ss->m_NetState.Get();
			ss->m_NetState.Reset();
			ss->m_PTZState.Reset();
			Result = TRUE;

				CString strState = LoadStringInt( IDS_PROP_NODATA );
			//if(OldState != sBattery_offline)
				FireCameraEvent( ss->m_dwCameraID.Get(), strState );
				FireSlaveEvent( ss->m_dwCameraID.Get(), strState );
		}
	}
	return Result;
}

bool	CDepCfgLiteDoc::Connect( ITeleCallback* pCallback, BOOL bInternal )
{
	
	if( bInternal )
		m_InternalObservers.push_back( pCallback );
	else
		m_Observers.push_back( pCallback);
	return true;
}

bool	CDepCfgLiteDoc::Disconnect( ITeleCallback* pCallback, BOOL bInternal )
{
	if( bInternal )
	{
		if( !pCallback) m_Observers.clear();
		else
		{
			std::vector<ITeleCallback*>::iterator itr = std::find( m_Observers.begin(), m_Observers.end(), pCallback );
			if( itr != m_Observers.end()) m_Observers.erase( itr );
		}
	}
	else
	{
		if( !pCallback) m_Observers.clear();
		else
		{
			std::vector<ITeleCallback*>::iterator itr = std::find( m_Observers.begin(), m_Observers.end(), pCallback );
			if( itr != m_Observers.end()) m_Observers.erase( itr );
		}
	}
	return true;
}

void	CDepCfgLiteDoc::FireCompEvent( CString name, CString state )
{
	size_t i;
	USES_CONVERSION;
	std::wstring strName	= T2CW( name );
	std::wstring strState	= T2CW( state );
	for( i=0; i < m_Observers.size(); ++i)
	{
		m_Observers[i]->OnComputer( strName.c_str(), strState.c_str() );
	}
	for( i=0; i < m_InternalObservers.size(); ++i)
	{
		m_InternalObservers[i]->OnComputer( strName.c_str(), strState.c_str());
	}

}

void	CDepCfgLiteDoc::FireCameraEvent( LONG id, CString state )
{
	USES_CONVERSION;
	size_t i;
	std::wstring strState	= T2CW( state );
	for( i=0; i < m_Observers.size(); ++i)
	{
		m_Observers[i]->OnCamera( id, strState.c_str()  );
	}
	for( i=0; i < m_InternalObservers.size(); ++i)
	{
		m_InternalObservers[i]->OnCamera(id, strState.c_str()  );
	}
}

void	CDepCfgLiteDoc::FireSlaveEvent( LONG id, CString state )
{
	USES_CONVERSION;
	size_t i;
	std::wstring strState	= T2CW( state );
	for( i=0; i < m_Observers.size(); ++i)
	{
		m_Observers[i]->OnPTZ( id, strState.c_str()  );
	}
	for( i=0; i < m_InternalObservers.size(); ++i)
	{
		m_InternalObservers[i]->OnPTZ( id, strState.c_str()  );
	}
}