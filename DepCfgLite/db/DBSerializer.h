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
#ifndef _D_B_SERIALIZER_3409536366849194_
#define _D_B_SERIALIZER_3409536366849194_

#include "OleDBConnection.h"
#include "../../Common/item/LogicReport.h"
#include "../interface/IDBSerializer.h"

MACRO_EXCEPTION( InsertException, DatabaseException)
MACRO_EXCEPTION( ClusterInsertException, InsertException)
MACRO_EXCEPTION( CameraInsertException, InsertException)
MACRO_EXCEPTION( CameraPTZInsertException, InsertException)
//======================================================================================//
//                                  class DBSerializer                                  //
//======================================================================================//

class DBSerializer :	protected COleDBConnection,
						public	DepCfg::IDBSerializer
{
	void	Connect(const std::wstring& sConStr);

	int		InsertCluster( CString sName, CString sIP );
	std::pair<bool, int> SelectCluster( CString sName, CString sIP );
	void	InsertCamera(  bool bUpdateOnly, int nClusterID, int nCameraID, CString sIP, bool bMaster);
	void	InsertCameraPTZ( bool bUpdateOnly,int nCameraID, DWORD dwPTZDevice, DWORD dwRS485ID ); 

	void	DeleteClusters( const std::vector<CStringW>& ExistClusters);
	void	DeleteCameras( const std::vector<int>& ExistCameras);
	CStringW CreateParenthesisQuery(const std::vector<CStringW>& ExistClusters, bool bQuoted);
public:
	DBSerializer();
	virtual ~DBSerializer();

private:
	void	SaveFile( const void* pData, size_t nSize);
	void	LoadFile( std::vector<BYTE>& BinArr ); 
	void	CreateDatabase( const std::vector< LogicReport::Cluster >& Clusters );
	void	Release() { delete this; };

	virtual void	LoadFile( const std::wstring& sConStr, std::vector<BYTE>& BinArr ); 
};

#endif // _D_B_SERIALIZER_3409536366849194_