//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Çàãðóçêà ðåñóðñîâ èç dll & exe
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 30.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _RESOURCE_LOADER_2887146871971916_
#define _RESOURCE_LOADER_2887146871971916_

#include "../interface/IResourceLoader.h"
#include "../misc/interfaceEx.h"

//======================================================================================//
//                                 class ResourceLoader                                 //
//======================================================================================//

class ResourceExeLoader : public DepCfg::IResourceLoader
{
public:
	ResourceExeLoader() {};
	virtual ~ResourceExeLoader() {};

private:

	virtual CStringW		LoadStringInt(UINT nID);
	virtual HICON			LoadIconInt(UINT nID);

	virtual CStringW			LoadCompState(		DWORD dwState ) { return CStringW();}
	virtual CStringW			LoadCameraState(	DWORD dwState ) { return CStringW();}
	virtual CStringW			LoadPtzState(		DWORD dwState ) { return CStringW();}

	virtual void			Release() { delete this;}
};

class ResourceDllLoader : public DepCfg::IResourceLoader
{
public:
	ResourceDllLoader() {};
	virtual ~ResourceDllLoader() {};

private:

	virtual CStringW		LoadStringInt(UINT nID);
	virtual HICON			LoadIconInt(UINT nID);

	virtual CStringW			LoadCompState(		DWORD dwState );
	virtual CStringW			LoadCameraState(	DWORD dwState );
	virtual CStringW			LoadPtzState(		DWORD dwState );

	virtual void			Release() { delete this;}
};

#endif // _RESOURCE_LOADER_2887146871971916_