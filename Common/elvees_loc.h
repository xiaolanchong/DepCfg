#ifndef __LOCALE_FAF480F4_H__
#define __LOCALE_FAF480F4_H__

//#include <atlbase.h>

namespace ElVEES_NLS
{

inline WORD GetLangID()
{
	const DWORD c_DefLCID = MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT );
	DWORD dwLangID = c_DefLCID;
	CRegKey	reg;
	LONG res;
	res = reg.Open( HKEY_LOCAL_MACHINE, _T("SOFTWARE\\ELVEES\\Common"), KEY_READ ) ; 
	if( res == ERROR_SUCCESS )
	{
		res = reg.QueryDWORDValue( _T("LangLCID"), dwLangID );
		if( res != ERROR_SUCCESS )
		{
			dwLangID = c_DefLCID;
		}
	}
	return WORD( dwLangID );
}

inline void SetNLSLocale()
{

	SetThreadLocale( MAKELCID(GetLangID(), SORT_DEFAULT));
}

}

#endif // __LOCALE_FAF480F4_H__