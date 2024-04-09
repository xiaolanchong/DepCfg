// DllRes.cpp: implementation of the CDllRes class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DllRes.h"
//#include "../../Servers/RecogSvr/sysstate.h"
#include "../Common/item/ProjectDB.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CString	CDllRes::LoadCompState( DWORD dwState )
{
	switch (dwState)	
	{
	case sBattery_online : 
		return LoadStringFromDll( IDS_PROP_COMP_ONLINE); 
	case sBattery_battery :
		return LoadStringFromDll( IDS_PROP_COMP_BATTERY);
	case sBattery_offline:
		return LoadStringFromDll( IDS_PROP_COMP_OFFLINE);
	default: 
//		ASSERT(FALSE);
		return LoadStringFromDll(IDS_PROP_STATE_UNKNOWN);
	}
}

CString	CDllRes::LoadCameraState( DWORD dwState )
{
		switch ( dwState)
		{
		case ECameraNoError : 
			{
				return LoadStringFromDll(IDS_PROP_STATE_OK);;
			}
		case ECameraStopped : 
			{
				return LoadStringFromDll(IDS_PROP_STATE_STOPPED);
			}
		case ECameraInit : 
			{
				return LoadStringFromDll(IDS_PROP_STATE_INIT);
			}
		case ECameraError :
			{
				return LoadStringFromDll(IDS_PROP_STATE_ERROR);
			}
		case ECameraNoImage : 
			{
				 return LoadStringFromDll(IDS_PROP_STATE_NOIMAGE);
			}			
		case ECameraBlind :
			{
				return LoadStringFromDll(IDS_PROP_STATE_BLIND);
			}
		default: 
//			ASSERT(FALSE);
			return LoadStringFromDll(IDS_PROP_STATE_UNKNOWN);
		}
}

CString	CDllRes::LoadPtzState( DWORD dwState )
{
		switch (dwState )
		{
		case EPTZNoError : 
			{
				return LoadStringFromDll(IDS_PROP_PTZSTATE_OK);
			}
		case EPTZNotConnected : 
			{
				return LoadStringFromDll(IDS_PROP_PTZSTATE_NOTCONNECTED);
			}
		case EPTZFailed : 
			{
				 return LoadStringFromDll(IDS_PROP_PTZSTATE_FAILED);

			}			
		case EPTZTimeout : 
			{
				 return LoadStringFromDll(IDS_PROP_PTZSTATE_TIMEOUT);
			}
		default: 
//			ASSERT(FALSE);
			return LoadStringFromDll(IDS_PROP_STATE_UNKNOWN);		
		}
}

std::vector<BYTE>		CDllRes::LoadBinResource( LPCTSTR szType, UINT nID )
{
	HRSRC hRc = FindResourceEx( m_hDll, szType, MAKEINTRESOURCE(nID), /*m_wLangID */MAKELANGID(LANG_NEUTRAL,
		SUBLANG_DEFAULT));
	DWORD zz = GetLastError();
	if(!hRc) throw ResourceException();
	HGLOBAL hGl		=	LoadResource( m_hDll, hRc );
	if(!hGl) throw ResourceException();
	DWORD dwSize	=	SizeofResource( m_hDll, hRc);
	void* pRes		=	LockResource( hGl );
	if(!pRes) throw ResourceException();
	return std::vector<BYTE>( (const BYTE*)pRes, (const BYTE*)pRes + dwSize  );
}

CString CDllRes::LoadStringFromDll( UINT nID )
#if 0
try
{
	std::vector<BYTE>	buf = LoadBinResource( RT_STRING, nID );
	buf.push_back(0);
	buf.push_back(0);
	return CString((const wchar_t*)&buf[0]);
}
catch( ResourceException )
#endif
{
	const size_t BufSize = 255;
	TCHAR	szBuf[BufSize];
	LoadString( m_hDll, nID, szBuf, BufSize );
	return CString ( szBuf);
}

HICON	CDllRes::LoadIconFromDll( UINT nID)
#if 0
try
{
	std::vector<BYTE> z = LoadBinResource( RT_ICON, nID );

	HICON hIcon = CreateIconFromResource( &z[0], z.size(), TRUE, 0x00030000);
	if( !hIcon ) throw ResourceException();
	else return hIcon;
}
catch (ResourceException) 
#endif
{
	return ::LoadIcon( m_hDll, MAKEINTRESOURCE(nID));
}