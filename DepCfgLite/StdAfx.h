// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__64BD5B27_DBB2_4C3A_8431_0ADF1511F554__INCLUDED_)
#define AFX_STDAFX_H__64BD5B27_DBB2_4C3A_8431_0ADF1511F554__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

//bug in winxp sp2, there is no FreeAddrInfoW, so emulate win2003 sp1

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0502		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0502		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0502 // Change this to the appropriate value to target Windows Me or later.
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h>        // MFC Automation classes
#endif // _AFX_NO_OLE_SUPPORT


#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC database classes
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO database classes
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#if 0

#ifndef FreeAddrInfo 
#define FreeAddrInfo(x)
#endif

#ifndef GetAddrInfo 
inline  int GetAddrInfo( LPCTSTR , LPCTSTR , void* ,  void* )
{
	return 0;
}
#endif

#endif

#include <afxsock.h>		// MFC socket extensions

#include <vector>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "res\resource.h"

#include "../../../RecogSvr/trunk/sysstate.h"
#include "../../../DBBridge/trunk/DBBridge.h"

#if _MSC_VER > 1200
#pragma  warning(disable : 4290)
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__64BD5B27_DBB2_4C3A_8431_0ADF1511F554__INCLUDED_)
