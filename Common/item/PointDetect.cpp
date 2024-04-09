// PointDetect.cpp: implementation of the PointDetect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "PointDetect.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace
{

bool	PtInRect( CPoint Point, CPoint pt1, CPoint pt2, int Gap )
{
	CSize a = pt2 - pt1;
	//CSize b = Point - pt1;
/*
	double c = (a.cx * b.cx + a.cy * b.cy)/double(a.cx * a.cx + a.cy * a.cy);

	CSize d ( b.cx - int( c*a.cx ), b.cy - int( c*a.cy ) ) ;
*/
	double d2 = a.cx / sqrt( double( a.cx * a.cx + a.cy * a.cy ));
	double d1 = a.cy / sqrt( double(a.cx * a.cx + a.cy * a.cy ));
	CSize off (int(Gap * d1), int (Gap * d2) );
	CPoint pt[4] = 
	{
		pt1 + off, pt2 + off,
		pt2 - off, pt1 - off
	};
	CRgn rgn;
	rgn.CreatePolygonRgn( pt, 4, WINDING );
	return rgn.PtInRegion( Point ) == TRUE;
}

}

bool	PtInLine( CPoint Point, CPoint* Carcas, int Num, int Gap )
{
//	for( int i = 0; i < 0; i++)
	return PtInRect( Point, Carcas[0], Carcas[1], Gap );
}

