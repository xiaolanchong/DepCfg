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
#include "stdafx.h"
#include "ResourceLoader.h"

#include "../misc/DllRes.h"
//======================================================================================//
//                                 class ResourceLoader                                 //
//======================================================================================//

CStringW		ResourceExeLoader::LoadStringInt(UINT nID)
{
	CStringW s;
	s.LoadString( nID );
	return s;
}

HICON			ResourceExeLoader::LoadIconInt(UINT nID)
{
	return ::LoadIcon( AfxGetInstanceHandle(), MAKEINTRESOURCE( nID ));
}

//////////////////////////////////////////////////////////////////////////

CStringW		ResourceDllLoader::LoadStringInt(UINT nID)
{
	CDllRes res;
	return res.LoadStringFromDll(nID);
}

HICON			ResourceDllLoader::LoadIconInt(UINT nID)
{
	CDllRes res;
	return res.LoadIconFromDll(nID);
}

CStringW		ResourceDllLoader::LoadCompState(		DWORD dwState )
{
	CDllRes res;
	return res.LoadCompState(dwState);
}

CStringW		ResourceDllLoader::LoadCameraState(	DWORD dwState )
{
	CDllRes res;
	return res.LoadCameraState(dwState);
}

CStringW		ResourceDllLoader::LoadPtzState(		DWORD dwState )
{
	CDllRes res;
	return res.LoadPtzState(dwState);
}

EXPOSE_INTERFACE(ResourceExeLoader, DepCfg::IResourceLoader, RESOURCE_EXE_INTERFACE);
EXPOSE_INTERFACE(ResourceDllLoader, DepCfg::IResourceLoader, RESOURCE_DLL_INTERFACE);