//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	database serializer for dep file & db insertion
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 13.04.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "DBAccessor.h"
#include "DBSerializer.h"
#include "../misc/interfaceEx.h"


#define ELVEES_VERSION

namespace
{

struct PTZDevice
{
	int			nPanMax, nTiltMax, PresetMax;
	LPCTSTR		szPlugin;
};

static PTZDevice g_PTZArray[4] = 
{
	{ 7, 7, 63,		_T("DomeCam.dll")	},
	{ 6, 6, 255,	_T("PKT.dll")		},
	{ 7, 7, 90,		_T("Pelco.dll")		},
	{ 7, 7, 255,	_T("DomeCam.dll")	}
};

}


//======================================================================================//
//                                  class DBSerializer                                  //
//======================================================================================//
DBSerializer::DBSerializer()
{
}

DBSerializer::~DBSerializer()
{
}

void	DBSerializer::SaveFile( const void* pData, size_t nSize)
try
{
	std::wstring sEmpty;
	Connect( sEmpty );
	std::vector<BYTE> Data( (const BYTE*)pData, (const BYTE*)pData + nSize );
	bool res = WriteBlobData<DBA_Globals>( m_dbSession, NULL, Data );
	if( !res ) throw DatabaseException("Failed to save dep file");
	Close();
}
catch( DatabaseException )
{
	Close();
	throw;
};

void	DBSerializer::LoadFile( const std::wstring& sConStr, std::vector<BYTE>& BinArr )
try
{
	Connect(sConStr);
	bool res = ReadBlobData<DBA_Globals>( m_dbSession, NULL, BinArr );
	if( !res ) throw DatabaseException("Failed to load dep file");
	Close();
}
catch( DatabaseException )
{
	Close();
	throw;
};

void	DBSerializer::LoadFile( std::vector<BYTE>& BinArr )
{
	std::wstring sEmpty;
	LoadFile( sEmpty, BinArr );
}


#ifdef USE_OWN_CONNECTION_STRING

#pragma code_seg(push, r1)
LPCTSTR c_szPassword = _T("t*e_AVafrEzUtaTha2rEmUJE@ew#xaze");
#pragma code_seg(pop, r1) 

#endif


void	DBSerializer::Connect(const std::wstring& sConStr)
{
#ifdef USE_OWN_CONNECTION_STRING
	TCHAR szBuf[1024];
	DWORD dwSize = sizeof( szBuf );
	DWORD dwType = REG_SZ;
	DWORD res = SHGetValue( HKEY_LOCAL_MACHINE, _T("Software\\ElVEES\\Common"), _T("DbInitString"), &dwType, szBuf, &dwSize );
	if( res != ERROR_SUCCESS  ) throw  DBConnectionException("Invalid connection string");
	std::tstring sCS (szBuf);
#ifdef ELVEES_VERSION
	sCS += _T(";User ID=OrwellSAdmin;Password=");
	sCS += c_szPassword;
#endif
#elif defined(ORWELL_DB)

	if( !sConStr.empty() ) 
	{
		Open( sConStr, false );
		return;
	}
	
	DWORD dwSize = 0;
	long res = GetO2KDBString( NULL, &dwSize, true);
	if(res != DBB_S_OK)
	{
		throw DatabaseException("failed to get the connection string");
	}
	std::vector<WCHAR> Buf(dwSize);
	res = GetO2KDBString( &Buf[0], &dwSize, true);
	if(res != DBB_S_OK)
	{
		throw DatabaseException("failed to get the connection string");
	}
	std::wstring sCS( Buf.begin(), Buf.end() );
#else
	std::wstring sCS;
#endif
	Open( sCS, false );
}

void	DBSerializer::CreateDatabase( const std::vector< LogicReport::Cluster >& Clusters )
{
	std::wstring sEmpty;
	Connect( sEmpty );
	std::vector<CStringW> ExistingClusters;
	std::vector<int> ExistingCameras;
	for( size_t i = 0; i < Clusters.size(); ++i )
	{
		ExistingClusters.push_back( Clusters[i].sName );
		for( size_t j = 0; j < Clusters[i].Masters.size(); ++j )
		{	
			ExistingCameras.push_back( Clusters[i].Masters[j].nCameraID );
		}
		for( size_t j = 0; j < Clusters[i].Slaves.size(); ++j )
		{
			ExistingCameras.push_back( Clusters[i].Slaves[j].nCameraID );
		}
	}

	DeleteClusters( ExistingClusters );
	DeleteCameras( ExistingCameras );

	for( size_t i = 0; i < Clusters.size(); ++i )
	{
		int nClusterID = InsertCluster( Clusters[i].sName, Clusters[i].sIP );
		CString s;
		s.Format( _T("Cluster: Name=%s, IP=%s\n"), (LPCTSTR)Clusters[i].sName, (LPCTSTR)Clusters[i].sIP );
		OutputDebugString(s);
		for( size_t j = 0; j < Clusters[i].Masters.size(); ++j )
		{
			const LogicReport::Master& ms = Clusters[i].Masters[j];
			InsertCamera( false, nClusterID, ms.nCameraID, ms.sIP, true );
			CString s;
			s.Format( _T("\tMaster: ID=%d, IP=%s\n"), 
							ms.nCameraID, (LPCTSTR)ms.sIP );
			OutputDebugString(s);
		}
		for( size_t j = 0; j < Clusters[i].Slaves.size(); ++j )
		{
			const LogicReport::Slave& ss = Clusters[i].Slaves[j];
			InsertCamera( false, nClusterID, ss.nCameraID, ss.sIP, false );
			InsertCameraPTZ( false, ss.nCameraID, ss.dwPTZDevice, ss.dwRS485ID );

			CString s;
			s.Format( _T("\tSlave: ID=%d, IP=%s, PTZ=%d, RS485=%d\n"), 
				ss.nCameraID, (LPCTSTR)ss.sIP, ss.dwPTZDevice, ss.dwRS485ID );
			OutputDebugString(s);
		}
	}
	Close();
}

int		DBSerializer::InsertCluster( CString sName, CString sIP )
{
	std::pair<bool, int> p = SelectCluster( sName, sIP );
	if(p.first) return p.second;

	CCommand<CAccessor<DBA_Cluster> > A_Cluster;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw ClusterInsertException("Statistics table connection error");
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_INSERT );
	propset.AddProperty( DBPROP_SERVERDATAONINSERT, true);

	hr = A_Cluster.Open(m_dbSession, NULL, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw ClusterInsertException("table connection error");
	}

	int nClusterID;
	A_Cluster.m_sClusterName	= sName;
	A_Cluster.m_sClusterIP		= sIP;
	hr = A_Cluster.Insert(0, true);	
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		A_Cluster.FreeRecordMemory();
		throw ClusterInsertException("set data error");
	}
	hr = A_Cluster.GetData( 1 );
	if( FAILED(hr) ) 
	{
		AtlTraceErrorRecords(hr);
		A_Cluster.FreeRecordMemory();
		throw ClusterInsertException("get data error");
	}
	nClusterID = A_Cluster.m_nClusterID;
	A_Cluster.FreeRecordMemory();
	A_Cluster.Close();
	return nClusterID;
}

std::pair<bool, int> DBSerializer::SelectCluster( CString sName, CString sIP )
{
	CCommand<CAccessor<DBA_Cluster> > A_Cluster;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw ClusterInsertException("Statistics table connection error");
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE );

	CStringW sQuery(L"SELECT ClusterID, ClusterIP FROM Cluster WHERE ClusterName=\'");
	sQuery += sName;
	sQuery += L"\'";

	hr = A_Cluster.Open(m_dbSession, sQuery, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw ClusterInsertException("table connection error");
	}

	hr = A_Cluster.MoveNext();
	if( hr != S_OK)
	{
		return std::make_pair(false, -1);
	}

	hr = A_Cluster.GetData(2);
	A_Cluster.m_sClusterIP		= sIP;
	hr = A_Cluster.SetData(3);	
	if( FAILED(hr) )
	{
		AtlTraceErrorRecords(hr);
		A_Cluster.FreeRecordMemory();
		throw ClusterInsertException("set data error");
	}
	return std::make_pair(true, A_Cluster.m_nClusterID);
}

void	DBSerializer::InsertCamera( bool bUpdateOnly, int nClusterID, int nCameraID, CString sIP, bool bMaster)
{
	CCommand<CAccessor<DBA_Camera> > A_Camera;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw CameraInsertException("Statistics table connection error");
	}

	HRESULT hr;

	CString sQuery;
	sQuery.Format(	L"SELECT ClusterID, CameraType, CameraComputer "
		L"FROM Camera WHERE CameraID=%d", nCameraID );

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, bUpdateOnly ? DBPROPVAL_UP_CHANGE : DBPROPVAL_UP_INSERT );

	hr = A_Camera.Open(m_dbSession, bUpdateOnly ? (LPCWSTR)sQuery : NULL, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw CameraInsertException("table connection error");
	}
	if( bUpdateOnly && A_Camera.MoveNext() != S_OK )
	{
		AtlTraceErrorRecords(hr);
		A_Camera.FreeRecordMemory();
		throw CameraInsertException("No data");		
	}

	A_Camera.m_nCameraID	= nCameraID;
	A_Camera.m_nClusterID	= nClusterID;
	A_Camera.m_nCameraType	= bMaster ? 0 : 1;
	A_Camera.m_sComputerIP	= sIP;
	hr =	bUpdateOnly? A_Camera.SetData(1) :	A_Camera.Insert(0, true);	
	if( FAILED(hr) ) 
	{
		if( bUpdateOnly )
		{
			AtlTraceErrorRecords(hr);
			A_Camera.FreeRecordMemory();
			throw CameraInsertException("set data error");
		}
		else
			InsertCamera( true, nClusterID,  nCameraID,  sIP,  bMaster );
	}
}

void	DBSerializer::InsertCameraPTZ( bool bUpdateOnly, int nCameraID, DWORD dwPTZDevice, DWORD dwRS485ID )
{
	if( dwPTZDevice >= sizeof( g_PTZArray )/ sizeof(g_PTZArray[0]) )
		throw CameraPTZInsertException( "Wrong PTZ device id" );

	CCommand<CAccessor<DBA_CameraPTZ> > A_CameraPTZ;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw CameraPTZInsertException("CameraPTZ table connection error");
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, bUpdateOnly? DBPROPVAL_UP_CHANGE : DBPROPVAL_UP_INSERT );

	CStringW sQuery;
	if( bUpdateOnly )
	{
		sQuery.Format(	L"SELECT CameraID, PTZ_PresetMax, PTZ_TiltMax, PTZ_PanMax, PTZ_Plugin, PTZ_RS485ID "
						L"FROM CameraPTZ WHERE CameraID=%d", nCameraID );
	}

	hr = A_CameraPTZ.Open(m_dbSession, bUpdateOnly? (LPCWSTR)sQuery : NULL, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw CameraPTZInsertException("table connection error");
	}

	const PTZDevice& dev = g_PTZArray[dwPTZDevice];

	if( bUpdateOnly && A_CameraPTZ.MoveNext() != S_OK )
	{
		AtlTraceErrorRecords(hr);
		A_CameraPTZ.FreeRecordMemory();
		throw CameraPTZInsertException("No data");		
	}

	A_CameraPTZ.m_nCameraID		= nCameraID;
	A_CameraPTZ.m_nPresetMax	= dev.PresetMax;
	A_CameraPTZ.m_nTiltMax		= dev.nTiltMax;
	A_CameraPTZ.m_nPanMax		= dev.nPanMax;
	A_CameraPTZ.m_sPlugin		= dev.szPlugin;
	A_CameraPTZ.m_nRS485ID		= dwRS485ID;

	hr = bUpdateOnly? A_CameraPTZ.SetData(1) :	A_CameraPTZ.Insert(0, true);	
	if( FAILED(hr) ) 
	{
		if( bUpdateOnly )
		{
			AtlTraceErrorRecords(hr);
			A_CameraPTZ.FreeRecordMemory();
			throw CameraPTZInsertException("set data error");
		}
		else
			InsertCameraPTZ( true, nCameraID, dwPTZDevice, dwRS485ID );
	}
}

void	DBSerializer::DeleteClusters( const std::vector<CStringW>& ExistClusters)
{
	CCommand<CNoAccessor > A_Cluster;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw ClusterInsertException("Cluster table connection error");
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_DELETE );

	CStringW sQuery(L"DELETE FROM Cluster ");
	if(!ExistClusters.empty())
	{
		sQuery +=	L"WHERE ClusterName NOT IN " ;
		sQuery +=	CreateParenthesisQuery(ExistClusters, true);
	}

	hr = A_Cluster.Open(m_dbSession, sQuery, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw ClusterInsertException("table connection error");
	}
}

void	DBSerializer::DeleteCameras( const std::vector<int>& ExistCameras)
{
	CCommand<CNoAccessor > A_Camera;
	if( !m_dbSession.m_spOpenRowset || !CheckConnection())
	{
		throw CameraInsertException("Camera table connection error");
	}

	HRESULT hr;

	CDBPropSet  propset(DBPROPSET_ROWSET);
	propset.AddProperty(DBPROP_IRowsetChange, true);
	propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_DELETE );

	std::vector<CStringW> ExistingNames;
	for( size_t i =0; i < ExistCameras.size(); ++i )
	{
		CStringW s;
		s.Format(L"%d", ExistCameras[i]);
		ExistingNames.push_back( s );
	}

	CStringW sQuery(L"DELETE FROM Camera WHERE CameraID <= 1000000 ");
	if(!ExistCameras.empty())
	{
		sQuery +=	L"AND CameraID NOT IN " ;
		sQuery +=	CreateParenthesisQuery(ExistingNames, false);
	}

	hr = A_Camera.Open(m_dbSession, sQuery, &propset);
	if(FAILED( hr ) ) 
	{
		TraceError(hr);
		throw CameraInsertException("Camera table open error");
	}
}

CStringW DBSerializer::CreateParenthesisQuery(const std::vector<CStringW>& Names, bool bQuoted)
{
	ASSERT( !Names.empty() );
	CStringW sQuery(L"( ");
	if(!Names.empty() ) 
	{
		if(bQuoted) sQuery += L"\'";
		sQuery += Names[0];
		if(bQuoted) sQuery += L"\'";
	}
	for( size_t i =1; i < Names.size() ; ++i )
	{
		sQuery += L", ";
		if(bQuoted) sQuery += L"\'";
		sQuery += Names[i];
		if(bQuoted) sQuery += L"\'";
	}
	sQuery += L" )";
	return sQuery;
}

EXPOSE_INTERFACE( DBSerializer, DepCfg::IDBSerializer, DBSERIALIZER_INTERFACE );