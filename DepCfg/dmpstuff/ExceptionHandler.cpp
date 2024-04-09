// ExceptionHandler.cpp  Version 1.3
//
// Copyright � 1998 Bruce Dawson
//
// This source file contains the exception handler for recording error
// information after crashes. See ExceptionHandler.h for information
// on how to hook it in.
//
// Author:       Bruce Dawson
//               brucedawson@cygnus-software.com
//
// Modified by:  Hans Dietrich
//               hdietrich2@hotmail.com
//
// Version 1.3:  - Added minidump output
//
// Version 1.1:  - reformatted output for XP-like error report
//               - added ascii output to stack dump
//
// A paper by the original author can be found at:
//     http://www.cygnus-software.com/papers/release_debugging.html
//
///////////////////////////////////////////////////////////////////////////////

// Disable warnings generated by the Windows header files.
#pragma warning(disable : 4514)
#pragma warning(disable : 4201)

#define _WIN32_WINDOWS 0x0500	// for IsDebuggerPresent

// does not require MFC;  use 'Not using precompiled headers'

#include "stdafx.h"
//#include <windows.h>
//#include <tchar.h>
#include <dbghelp.h>

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

#define APP_NAME	"DepCfg"

///////////////////////////////////////////////////////////////////////////////
// lstrrchr (avoid the C Runtime )
static TCHAR * lstrrchr(LPCTSTR string, int ch)
{
	TCHAR *start = (TCHAR *)string;

	while (*string++)                       /* find end of string */
		;
											/* search towards front */
	while (--string != start && *string != (TCHAR) ch)
		;

	if (*string == (TCHAR) ch)                /* char found ? */
		return (TCHAR *)string;

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// DumpMiniDump
static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo)
{
	if (excpInfo == NULL) 
	{
		// Generate exception to get proper context in dump
		__try 
		{
			OutputDebugString(_T("raising exception\r\n"));
			RaiseException(EXCEPTION_BREAKPOINT, 0, 0, NULL);
		} 
		__except(DumpMiniDump(hFile, GetExceptionInformation()),
				 EXCEPTION_CONTINUE_EXECUTION) 
		{
		}
	} 
	else
	{
		OutputDebugString(_T("writing minidump\r\n"));
		MINIDUMP_EXCEPTION_INFORMATION eInfo;
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		// note:  MiniDumpWithIndirectlyReferencedMemory does not work on Win98
		MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithIndirectlyReferencedMemory,
			excpInfo ? &eInfo : NULL,
			NULL,
			NULL);
	}
}

///////////////////////////////////////////////////////////////////////////////
// GetFilePart
static TCHAR * GetFilePart(LPCTSTR source)
{
	TCHAR *result = lstrrchr(source, _T('\\'));
	if (result)
		result++;
	else
		result = (TCHAR *)source;
	return result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// RecordExceptionInfo
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, 
								LPCTSTR lpszMessage)
{
	static bool bFirstTime = true;
	if (!bFirstTime)	// Going recursive! That must mean this routine crashed!
		return EXCEPTION_CONTINUE_SEARCH;
	bFirstTime = false;

	// Create a filename to record the error information to.
	// Storing it in the executable directory works well.

	TCHAR szModuleName[MAX_PATH*2];
	ZeroMemory(szModuleName, sizeof(szModuleName));
	if (GetModuleFileName(0, szModuleName, _countof(szModuleName)-2) <= 0)
		lstrcpy(szModuleName, _T("Unknown"));

	TCHAR *pszFilePart = GetFilePart(szModuleName);

	///////////////////////////////////////////////////////////////////////////
	//
	// write minidump
	//
	///////////////////////////////////////////////////////////////////////////

	// Replace the filename with our minidump file name
/*	CString Time;
	time_t time;
	time(&time);
	Time.Format( "%Y" )*/
  CString dumpName = _T(APP_NAME) _T("_") + COleDateTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H-%M"))+_T(".dmp");
									
	lstrcpy(pszFilePart, dumpName);

	// Create the file
	HANDLE hMiniDumpFile = CreateFile(
		szModuleName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH,
		NULL);

	// Write the minidump to the file
	if (hMiniDumpFile != INVALID_HANDLE_VALUE)
	{
		DumpMiniDump(hMiniDumpFile, pExceptPtrs);

		// Close file
		CloseHandle(hMiniDumpFile);
	}

	// return the magic value which tells Win32 that this handler didn't
	// actually handle the exception - so that things will proceed as per
	// normal.
	return EXCEPTION_CONTINUE_SEARCH;
}
