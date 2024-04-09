// stdafx.cpp : source file that includes just the standard includes
//	DepCfg.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#if 0 // 2006-02-27 fixed in OXSzMiniDockFrmWnd.h
#if _MSC_VER > 1200

AFX_STATIC_DATA SIZE _afx_sizeMiniSys = { 0, 0 };

void CMiniFrameWnd::InvertSysMenu()
{
	CSize sizeBorder(GetSystemMetrics(SM_CXBORDER),
		GetSystemMetrics(SM_CYBORDER));
	CSize sizeFrame(GetSystemMetrics(SM_CXFRAME),
		GetSystemMetrics(SM_CYFRAME));

	CRect rect(sizeBorder.cx, sizeBorder.cy,
		_afx_sizeMiniSys.cx - sizeBorder.cx, _afx_sizeMiniSys.cy);
	if (GetStyle() & (MFS_4THICKFRAME | MFS_THICKFRAME | WS_THICKFRAME))
		rect.OffsetRect(sizeFrame.cx - sizeBorder.cx, sizeFrame.cy - sizeBorder.cy);

	CDC* pDC = GetWindowDC();
	pDC->InvertRect(rect);
	ReleaseDC(pDC);
}

#endif
#endif
