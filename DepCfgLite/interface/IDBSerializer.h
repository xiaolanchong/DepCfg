//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Интерфейс сохранения/загрузки для БД
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_D_B_SERIALIZER_4743151167185440_
#define _I_D_B_SERIALIZER_4743151167185440_
//======================================================================================//
//                                 struct IDBSerializer                                 //
//======================================================================================//

#define DBSERIALIZER_INTERFACE	L"{15B35C2B-394E-4d17-8946-33843A909962}"

#include "../../Common/Exception.h"
#include "../../Common/item/LogicReport.h"

MACRO_EXCEPTION(DatabaseException	,		CommonException)
MACRO_EXCEPTION(DBConnectionException	,	DatabaseException)

namespace DepCfg
{

struct IDBSerializer
{
	virtual void	SaveFile( const void* pData, size_t nSize)	= 0;
	virtual void	LoadFile( std::vector<BYTE>& BinArr )		= 0; 
	virtual void	CreateDatabase( const std::vector< LogicReport::Cluster >& Clusters ) = 0; 

	virtual void	Release() = 0;

	virtual void	LoadFile( const std::wstring& sConStr, std::vector<BYTE>& BinArr )		= 0; 
};

}

#endif // _I_D_B_SERIALIZER_4743151167185440_