// GuiRect.cpp: implementation of the GuiRect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiRect.h"

#include "../../DepCfgLite/interface/DepInterface.h"
#include "../../DepCfgLite/interface/IResourceLoader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStringW	GuiRect::LoadStringInt( UINT nID )
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);
	return p->LoadStringInt(nID);
}

void	GuiRect::Draw(Graphics& gr)	const	
{
	Pen pen(Color(0,0,0),1);
	
	if(GetState() == highlighted) 
	{
		pen.SetWidth( 2 ); 
	//	ASSERT(FALSE);
	}
	gr.DrawRectangle( &pen, m_rcItem.X, m_rcItem.Y, m_rcItem.Width, m_rcItem.Height );
/*	if(GetState() == selected)
	{
		DrawFrame(gr);
	}*/
}


void	GuiRect::StartAction(PointF point)		
{
	m_CurAction = GetAction(point);
}

void	GuiRect::DoAction(SizeF size)			
{
	switch(m_CurAction)
	{
		case drag : m_rcItem.Offset( size.Width, size.Height );
		break;
	//	case resize : m_rcItem.Inflate( /*0, 0, */size.cx, size.cy);
		break;
	}
}

void	GuiRect::EndAction(PointF size)			
{
	m_CurAction = none;
}

void	GuiRect::CancelAction()
{
	m_CurAction = none;
}

PointF					GuiRect::PointFromSlot	(DWORD dwSlot) const throw(SlotException) 
{
	std::map<DWORD, PointF> pt = GetSlotPoints();
	std::map<DWORD, PointF>::const_iterator itr = pt.find( dwSlot );
	if(itr == pt.end()) throw SlotException("Invalid slot number");
	return itr->second;
}

std::pair<bool, DWORD>	GuiRect::SlotFromPoint	(PointF point) const
{
	const SizeF sizeConnect = SizeF(25, 25);
	std::map<DWORD, PointF> pt = GetSlotPoints();
	std::map<DWORD, PointF>::const_iterator itr = pt.begin();;
	for( ; itr != pt.end() ; ++itr)
	{
		RectF rc( itr->second, sizeConnect );
		rc.Offset(-sizeConnect.Width/2,-sizeConnect.Height/2);
		if(rc.Contains( point )) return std::make_pair( true, itr->first);
	}
	return std::make_pair( false, 0 );
}

GuiItem::Action	GuiRect::GetAction(PointF point)			const	
{
	CRectF rcDef(GetRect());
	
	std::pair<bool, DWORD> p = SlotFromPoint(  PointF(point.X, point.Y)  ) ;
	if(p.first && m_DB.IsFreeSlot( GetID(), p.second )) return connect;

	rcDef = GetDragRect();

	if(rcDef.Contains( point)) return GuiItem::drag;
	return none;	
}

void	GuiRect::FillRect( Gdiplus::Graphics& gr, const CRectF& rc) const
{
	//CRectF rcf(rc);
	//rcf.Deflate( 1, 1, 1, 1);
	COLORREF dwColor = GetSysColor(COLOR_WINDOW);
	Gdiplus::Color cl = Gdiplus::Color::MakeARGB( GetRValue(dwColor), GetGValue(dwColor), GetGValue(dwColor), 0xff );
	Gdiplus::SolidBrush br(cl);
	gr.FillRectangle(&br, rc);
}

void	GuiRect::DrawItemHeader( Graphics& gr, const CRectF& rc, CString sHeader ) const
{
	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);

	StringFormat stringFormat;
	stringFormat.SetLineAlignment(StringAlignmentCenter);
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetTrimming( StringTrimmingEllipsisCharacter   );
	stringFormat.SetFormatFlags( StringFormatFlagsNoWrap );

	Color cl(0, 0, 0);
	SolidBrush br( cl );

	USES_CONVERSION;
	LPCWSTR szHeader = CT2CW( sHeader );

	gr.DrawString( szHeader, -1, &font, RectF(rc), &stringFormat, &br);
}

