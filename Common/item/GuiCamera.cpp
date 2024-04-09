// GuiCamera.cpp: implementation of the GuiCamera class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiCamera.h"
#include "..\utility\Helper.h"

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




////////////////////////////////////////////////////

void	GuiPhysicSlave::Draw(Graphics& gr, const Zoom& zoom)	const	
{


	CRectF rcItem = zoom.GetRect(GetRect());
	CRectF rc = rcItem;

	rc.Deflate( rc.Width/5, 0, 0, rc.Height/5);

	const SlaveSettings *ss = m_DB.GetSlave( GetID() );
	ASSERT(ss);

#ifdef  DEPCFGLITE
	Pen pen(Color::Black, 2);

	if( !ss->m_NetState.IsSet() )
	{
		pen.SetColor( Color::LightGray );
	}
	else if( ss->m_NetState.Get() != ECameraNoError )
	{
		pen.SetColor( Color::Red );
	}
	else if( !ss->m_PTZState.IsSet() || ss->m_PTZState.Get() != EPTZNoError )
	{
		pen.SetColor( Color::Red );
	}
#else
	Pen pen(Color::Red, 1);
#endif
	
	
	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );
	PointF center  = rc.Center();

	GuiRect::FillRect(gr, rc);
	gr.DrawRectangle( &pen, rc);
	
	std::map<DWORD, PointF> pt = GetSlotPoints();
	gr.DrawLine( &pen, rcItem.Left(), center.Y, rc.Left(), center.Y);
	gr.DrawLine( &pen, center.X, rcItem.Bottom(), center.X, rc.Bottom());

	CString str;

	if(ss->m_dwCameraID.IsSet())
		str.Format( _T("s%d"), ss->m_dwCameraID.Get() );
	else str = _T("s");

	FontFamily  fontFamily(L"Arial");
	Font        font(&fontFamily, 12, FontStyleBold, UnitPixel);
	   
	GuiRect::DrawItemHeader( gr, rc, str );

}

void	GuiPhysicSlave::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rcItem = zoom.GetRect(GetRect());
	CRectF rc = rcItem;

	rc.Deflate( rc.Width/5, 0, 0, rc.Height/5);
	

	PointF center  = rc.Center();

	CRect rcRect(rc.ToCRect());

	pDC->Rectangle( &rcRect);
//	std::map<DWORD, PointF> pt = GetSlotPoints();
	pDC->MoveTo( rcItem.Left(), center.Y);
	pDC->LineTo( rc.Left(), center.Y);
	pDC->MoveTo( center.X, rcItem.Bottom() );
	pDC->LineTo( center.X, rc.Bottom());
	
}

std::map<DWORD, PointF>	GuiPhysicSlave::GetSlotPoints() const 
{
	CRectF rc = GetRect();
	CRectF rcItem = GetRect();

	rc.Deflate( rc.Width/5, 0, 0, rc.Height/5);

	PointF center ( rc.X + rc.Width/2 , rc.Y + rc.Height/2 );

	std::map<DWORD, PointF> pt;
	pt.insert( std::make_pair( 0, PointF(rcItem.X, center.Y) ));
	pt.insert( std::make_pair( 1, PointF(center.X, rcItem.Y+rcItem.Height)) );
	return pt;
}


std::pair<bool,DWORD>	GuiPhysicSlave::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{

	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first || !m_DB.IsFreeSlot( GetID(), p.second )) return std::make_pair(false, 0);


	const ComputerSettings *cs = m_DB.GetComputer(dwID);

	if(  !cs ) return std::make_pair(false, 0);
	const SlaveSettings* ss = m_DB.GetSlave( GetID() );
	if( p.second == 1 ) 
	{
/*		if( !ss->m_dwPTZDevice.IsSet())		return std::make_pair(false, 0);

		bool bAll = m_DB.IsFreeSlot( dwID, dwSlot ) || m_DB.IsAllSlavePTZ( dwID, dwSlot, ss->m_dwPTZDevice.Get() );
		return std::make_pair(bAll, p.second);*/

	}
	else if( p.second == 0) return std::make_pair( m_DB.IsFreeSlot(dwID, dwSlot) , p.second);
	return p;
}


GuiItem::Action	GuiPhysicSlave::GetAction(PointF point)			const	
{
	CRectF rcDef(GetRect());
	
	std::pair<bool, DWORD> p = SlotFromPoint(  PointF(point.X, point.Y)  ) ;
	if(p.first  && m_DB.IsFreeSlot( GetID(), p.second )  ) 
	{
		return connect;
	}
	

	rcDef = GetDragRect();

	if(rcDef.Contains( point)) return GuiItem::drag;
	return none;	
}


void	GuiSlave::SetProperties(PropertyCB* pPropCB) 
{
#ifndef DEPCFGLITE

	SlaveSettings *ss = m_DB.GetSlave( GetID() );

	ASSERT( ss);

	CSlaveProperty sp(pPropCB, this)  ;
	sp.SetProperties(  *ss );
#endif
}

void GuiSlave::OnPropertyChange(CString Name, const std::any& Value) 
{
#ifndef DEPCFGLITE
	CString str = std::any_cast<CString>(Value);
//	AfxMessageBox( str );

	SlaveSettings* ss = m_DB.GetSlave( GetID() );

	CSlaveProperty sp( 0,0);
		
	sp.OnPropertyChange( *ss, Name, str );
	((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument()->SetModifiedFlag();
	AfxGetMainWnd()->Invalidate();
#endif
}


std::set<DWORD>		GuiSlave::OnDeleteItem() const
{
	std::set<DWORD> s;
	return s;
}


/////////////////////////////////////////////////////////////////////////

void	GuiLogicSlave::Draw(Graphics& gr, const Zoom& zoom)	const	
{


	CRectF rcItem = zoom.GetRect( GetRect());
	CRectF rc = rcItem;
	int Width = rc.Width/5;
	int Height = 0;

	rc.Deflate(Width, 0, 0, Height);
	Pen pen(Color::Blue, 1);
	
	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );
	GuiRect::FillRect(gr, rc);
	gr.DrawRectangle( &pen, rc);

	std::map<DWORD, PointF> pt = GetSlotPoints();

	pt[2] = zoom.GetPoint( pt[2] );
	gr.DrawLine( &pen, rcItem.Left(), (pt[2].Y), rc.Left(), float(pt[2].Y));

	CString sHeader;

	const SlaveSettings* itr = m_DB.GetSlave( GetID() );

	if(itr->m_dwCameraID.IsSet())
		sHeader.Format( _T("s%d"), itr->m_dwCameraID.Get() );
	else sHeader = _T("s");

	GuiRect::DrawItemHeader( gr, rc, sHeader );
}

void	GuiLogicSlave::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rcItem = zoom.GetRect( GetRect());
	CRectF rc = rcItem;
	int Width = rc.Width/5;
	int Height = 0;

	rc.Deflate(Width, 0, 0, Height);
	CRect rcRect(rc.ToCRect());

	pDC->Rectangle( &rcRect);
	std::map<DWORD, PointF> pt = GetSlotPoints();

	PointF pt2 = zoom.GetPoint( pt[2] );
	pDC->MoveTo( rcItem.Left(), (pt2.Y));
	pDC->LineTo( rc.Left(), float(pt2.Y));	
}

std::map<DWORD, PointF>	GuiLogicSlave::GetSlotPoints() const 
{
	CRectF rcItem = GetRect();
	CRectF rc = rcItem;
	rc.Inflate( rc.Width/5, 0, 0, rc.Height/5);
	
	std::map<DWORD, PointF> pt;
	pt.insert( std::make_pair( 2, PointF(rcItem.Left(), rcItem.Center().Y)) );
	return pt;
}

std::pair<bool,DWORD>	GuiLogicSlave::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{

//	if( dwID == GetID() ) return std::make_pair(false, 0);

	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first || !m_DB.IsFreeSlot( GetID(), p.second )) return std::make_pair(false, 0);


	const CommutatorSettings* itrComp = m_DB.GetCommutator(dwID);
	
	if(  !itrComp )
		return std::make_pair(false, 0);
	return p;
}

void	GuiSlave::SetReadOnlyProperty(ReadOnlyProperty* pROP)
{

#ifndef DEPCFGLITE
	SlaveSettings *ss = m_DB.GetSlave( GetID() );

	ASSERT( ss);

	CSlaveProperty sp(0, this)  ;
	sp.SetReadOnlyProperties(  *ss, pROP );	

#else
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
		RESOURCE_DLL_INTERFACE
		);

	SlaveSettings *ss = m_DB.GetSlave( GetID() );
	CString strState  = LoadStringInt(IDS_PROP_STATE  );
	CString strPtz = LoadStringInt(IDS_PROP_PTZSTATE  );

	ASSERT( ss);

	if( !ss->m_NetState.IsSet() )
	{
		CString strNoData = LoadStringInt( IDS_PROP_NODATA );
		pROP->SetReadOnlyProperty( strState, strNoData );
	}
	else
	{
		pROP->SetReadOnlyProperty( strState, p->LoadCameraState( ss->m_NetState.Get ()  ) );
	}

	if( !ss->m_PTZState.IsSet() )
	{
		CString strOk = LoadStringInt(IDS_PROP_NODATA);
		pROP->SetReadOnlyProperty( strPtz, strOk ); 
	}
	else
	{
		CString strOk = p->LoadPtzState(  ss->m_PTZState.Get() );
		pROP->SetReadOnlyProperty( strPtz, strOk ); 
	}

#endif
}
