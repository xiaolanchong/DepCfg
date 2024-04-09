//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Интерфейс загрузки ресурсов
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 30.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _I_RESOURCE_LOADER_7966283800890805_
#define _I_RESOURCE_LOADER_7966283800890805_
//======================================================================================//
//                                struct IResourceLoader                                 //
//======================================================================================//

#define RESOURCE_EXE_INTERFACE	L"{215D4E54-F8A0-4C96-AAF7-A5A1F38C7107}"
#define RESOURCE_DLL_INTERFACE	L"{59AB19BA-2F8C-43D5-8FA1-543A8824CF31}"

namespace DepCfg
{

struct IResourceLoader
{
	virtual CStringW		LoadStringInt(UINT nID)	= 0;
	virtual HICON			LoadIconInt(UINT nID)		= 0;

	virtual CStringW			LoadCompState(		DWORD dwState ) = 0;
	virtual CStringW			LoadCameraState(	DWORD dwState ) = 0;
	virtual CStringW			LoadPtzState(		DWORD dwState ) = 0;

	virtual void	Release() = 0;
};

}

#endif // _I_RESOURCE_LOADER_7966283800890805_