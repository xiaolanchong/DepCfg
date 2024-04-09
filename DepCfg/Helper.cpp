
//#include "stdafx.h"
#include "CTypeAccessor.h"
#include "Helper.h"
#define APPNAME "DepCfg"
// ¬ывести TRACE на произвольное количество аргументов
/*void TRACE(LPCTSTR szFormat, ...)
{

	va_list i;
	va_start(i, szFormat);  
	int Length;

	TCHAR Buffer[4096];

	Length = _vsntprintf_s(Buffer, sizeof(Buffer)/sizeof(TCHAR),_TRUNCATE, szFormat, i);

	va_end(i);

	// Print
	OutputDebugString(Buffer);
} */

//¬ыводит информацию об ошибке
void AtlTraceRelease(HRESULT hRes)
{

	CDBErrorInfo ErrorInfo;
	ULONG        cRecords;
	HRESULT      hr;
	ULONG        i;
	CComBSTR     bstrDesc, bstrHelpFile, bstrSource;
	GUID         guid;
	DWORD        dwHelpContext;
	WCHAR        wszGuid[40];

	// If the user passed in an HRESULT then trace it
	if (hRes != S_OK)
	{
		TRACE(_T("%s::OLE DB Error Record dump for hr = 0x%x\n"), APPNAME, hRes);
	}

	LCID lcLocale = GetSystemDefaultLCID();

	hr = ErrorInfo.GetErrorRecords(&cRecords);
	if (FAILED(hr) && ErrorInfo.m_spErrorInfo == NULL)
	{
		TRACE(_T("%s::No OLE DB Error Information found: hr = 0x%x\n"),APPNAME, hr);
	}
	else
	{

		for (i = 0; i < cRecords; i++)
		{
			hr = ErrorInfo.GetAllErrorInfo(i, lcLocale, &bstrDesc, &bstrSource, &guid,
				&dwHelpContext, &bstrHelpFile);
			if (FAILED(hr))
			{
				TRACE(_T("%s::OLE DB Error Record dump retrieval failed: hr = 0x%x\n"),APPNAME, hr);
				return;
			}
			StringFromGUID2(guid, wszGuid, sizeof(wszGuid) / sizeof(WCHAR));
			TRACE(_T("%s:: Row #: %4d Source: \"%s\" Description: \"%s\" Help File: \"%s\" Help Context: %4d GUID: %s\n"),
				APPNAME, i, static_cast<TCHAR*>(COLE2T(bstrSource)), static_cast<TCHAR*>(COLE2T(bstrDesc)), static_cast<TCHAR*>(COLE2T(bstrHelpFile)), dwHelpContext, static_cast<TCHAR*>(COLE2T(wszGuid)));
			bstrSource.Empty();
			bstrDesc.Empty();
			bstrHelpFile.Empty();
		}
		TRACE(_T("%s::OLE DB Error Record dump end\n"),APPNAME);
	}

}
// форматирование строки с произвольным количеством аргументов
std::wstring Formats(LPCTSTR szFormat, ...)
{
	va_list i;
	va_start(i, szFormat);  
	int Length;

	TCHAR Buffer[4096];

	Length = _vsntprintf_s(Buffer, sizeof(Buffer)/sizeof(TCHAR),_TRUNCATE, szFormat, i);

	va_end(i);

	std::wstring sBuffer(Buffer);

	return sBuffer;
}
