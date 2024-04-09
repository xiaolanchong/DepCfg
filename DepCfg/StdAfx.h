// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__B6B1C6C3_85ED_45DD_B2CF_9BDBF48A2CFE__INCLUDED_)
#define AFX_STDAFX_H__B6B1C6C3_85ED_45DD_B2CF_9BDBF48A2CFE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if( _MSC_VER == 1000)
#pragma warning( disable : 4786)
#endif

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER				// Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0502		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0502		// Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0502 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500	// Change this to the appropriate value to target IE 5.0 or later.
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#if 0

#ifndef FreeAddrInfo 
#define FreeAddrInfo(x)
#endif

#ifndef GetAddrInfo 
inline int GetAddrInfo( LPCTSTR , LPCTSTR , void* ,  void* )
{
	return 0;
}
#endif

#endif

#include <afxsock.h>

#include "res\resource.h"	// Main resources

//#define _AFX_NO_OLE_SUPPORT

#define __OXMFCIMPL_H__
#include <../src/mfc/afximpl.h>

//class CFrameWnd;
#define _OX_OLEIMPL2__
#include <AFXOLE.H>
#include <../src/mfc/oleimpl2.h>

#include <algorithm>
#include <memory>

using std::min;
using std::max;

#if _MSC_VER > 1200
#pragma  warning(disable : 4290)
#endif

#ifdef ORWELL_DB
#include "../../../DBBridge/trunk/DBBridge.h"
#endif

// SAFE macro
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(i) {if(i) {i->Release(); i=NULL;}}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) {if(p){ delete p; p = NULL; }}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) {if(p){ delete[] p; p = NULL; }}
#endif

#define APPNAME "DepCfg"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__B6B1C6C3_85ED_45DD_B2CF_9BDBF48A2CFE__INCLUDED_)
