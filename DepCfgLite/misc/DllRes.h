// DllRes.h: interface for the CDllRes class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_)
#define AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ResourceException : std::exception
{
public:
	const char* what() const { return "Resource exception"; }
};

class CDllRes
{
	HINSTANCE	m_hDll;
	WORD		m_wLangID;

	std::vector<BYTE> LoadBinResource( LPCTSTR szType, UINT nID )	;

public:
	CDllRes();
	~CDllRes();

	CString	LoadCompState( DWORD dwState );
	CString	LoadCameraState( DWORD dwState );
	CString	LoadPtzState( DWORD dwState );

	CString LoadStringFromDll( UINT nID );

	HICON	LoadIconFromDll( UINT nID);
protected:
  HINSTANCE m_hInstOld;
};
 

#endif // !defined(AFX_DLLRES_H__50F39BE3_2A0C_438F_B07F_EFBCA14D8C2F__INCLUDED_)
