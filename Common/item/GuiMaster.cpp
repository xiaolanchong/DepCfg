// GuiMaster.cpp: implementation of the GuiMaster class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiMaster.h"
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

void	GuiPhysicMaster::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	CRectF rcItem = zoom.GetRect( GetRect() );

	CRectF rc = rcItem;
	int Width = rc.Width/5;

	rc.Deflate(Width, 0, 0, 0);

	const MasterSettings* itr = m_DB.GetMaster( GetID() );
	ASSERT(itr);
#ifdef DEPCFGLITE
	Pen pen(Color::Black, 2);

	if( !itr->m_NetState.IsSet() )
	{
		pen.SetColor( Color::LightGray );
	}
	else 
	{
		switch ( itr->m_NetState.Get() )
		{
		case ECameraNoError : pen.SetColor( Color::Black ); break;
		default : pen.SetColor( Color::Red );
		}
	}	
#else
	Pen pen(Color::Red, 1);
#endif	

	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );
	GuiRect::FillRect(gr, rc);
	gr.DrawRectangle( &pen, rc);

	std::map<DWORD, PointF> pt = GetSlotPoints();

	pt[0] = zoom.GetPoint( pt[0] );

	gr.DrawLine( &pen, rcItem.Left(), float(pt[0].Y), rc.Left(), float(pt[0].Y));

	CString str;

	if(itr->m_dwCameraID.IsSet())
		str.Format( _T("m%d"), itr->m_dwCameraID.Get() );
	else str = _T("m");

	GuiRect::DrawItemHeader( gr, rc, str );
}

void	GuiPhysicMaster::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rcItem = zoom.GetRect( GetRect() );

	CRectF rc = rcItem;
	int Width = rc.Width/5;

	rc.Deflate(Width, 0, 0, 0);

	CRect rcRect(rc.ToCRect());
	pDC->Rectangle( &rcRect );
	std::map<DWORD, PointF> pt = GetSlotPoints();

	pt[0] = zoom.GetPoint( pt[0] );

	pDC->MoveTo(  rcItem.Left(), pt[0].Y );
	pDC->LineTo (rc.Left(), float(pt[0].Y));	
}

std::pair<bool,DWORD>	GuiPhysicMaster::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{
	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first || !m_DB.IsFreeSlot( GetID(), p.second )) return std::make_pair(false, 0);


	const ComputerSettings* cs = m_DB.GetComputer(dwID);
	
	if(  !cs || !m_DB.IsFreeSlot( dwID , dwSlot ))
		return std::make_pair(false, 0);

	return p;
}


std::map<DWORD, PointF>	GuiPhysicMaster::GetSlotPoints() const 
{
	CRectF rcItem = GetRect() ;

	std::map<DWORD, PointF> pt;
	pt.insert( std::make_pair( 0, PointF(rcItem.Left(), rcItem.Center().Y)));
	return pt;
}

////////////////////////////////////////////////////////////////////////////////////////////

void	GuiLogicMaster::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	CRectF rcItem = zoom.GetRect( GetRect() );

	CRectF rc = rcItem;
	int Width = rc.Width/5;

	rc.Deflate(Width, 0, 0, 0);
	Pen pen(Color::Blue, 1);
	
	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );
	GuiRect::FillRect(gr, rc);
	gr.DrawRectangle( &pen, rc);

	std::map<DWORD, PointF> pt = GetSlotPoints();

	pt[1] = zoom.GetPoint( pt[1] );

	gr.DrawLine( &pen, rcItem.Left(), float(pt[1].Y), rc.Left(), float(pt[1].Y));

	CString str;

	const MasterSettings* itr = m_DB.GetMaster( GetID() );

	if(itr->m_dwCameraID.IsSet())
		str.Format( _T("m%d"), itr->m_dwCameraID.Get() );
	else str = _T("m");

	GuiRect::DrawItemHeader( gr, rc, str );
}

void	GuiLogicMaster::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rcItem = zoom.GetRect( GetRect() );

	CRectF rc = rcItem;
	int Width = rc.Width/5;

	rc.Deflate(Width, 0, 0, 0);

	CRect rcRect(rc.ToCRect());
	pDC->Rectangle( &rcRect );
	std::map<DWORD, PointF> pt = GetSlotPoints();

	PointF pt1 = zoom.GetPoint( pt[1] );

	pDC->MoveTo(  rcItem.Left(), pt1.Y );
	pDC->LineTo (rc.Left(), (pt1.Y));		
}

void	GuiMaster::SetProperties(PropertyCB* pPropCB) 
{
#ifndef DEPCFGLITE
	MasterSettings* itr = m_DB.GetMaster( GetID() );

	ASSERT( itr );

	CMasterProperty mp(pPropCB, this);
	mp.SetProperties( *itr );
#endif
}

void GuiMaster::OnPropertyChange(CString Name, const std::any& Value) 
{
#ifndef DEPCFGLITE
	CString str = std::any_cast<CString>(Value);

	MasterSettings *itr = m_DB.GetMaster( GetID() );

	ASSERT( itr );

	CMasterProperty mp(0,0);
	mp.OnPropertyChange( *itr, Name, str );

	((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument()->SetModifiedFlag();
//	AfxGetMainWnd()->SendMessage(WM_UPDATE_TABS);
#endif
}

std::set<DWORD>		GuiMaster::OnDeleteItem() const
{
	std::set<DWORD> s;
	return s;
}

std::pair<bool,DWORD>	GuiLogicMaster::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{

//	if( dwID == GetID() ) return std::make_pair(false, 0);

	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first || !m_DB.IsFreeSlot( GetID(), p.second )) return std::make_pair(false, 0);


	const CommutatorSettings* itrComp = m_DB.GetCommutator(dwID);
	
	if(  !itrComp )
		return std::make_pair(false, 0);
	return p;
}

std::map<DWORD, PointF>	GuiLogicMaster::GetSlotPoints() const 
{
	CRectF rcItem = GetRect() ;

	std::map<DWORD, PointF> pt;
	pt.insert( std::make_pair( 1, PointF(rcItem.Left(), rcItem.Center().Y)));
	return pt;
}

void	GuiMaster::SetReadOnlyProperty(ReadOnlyProperty* pROP)
{
#ifndef DEPCFGLITE
	MasterSettings* itr = m_DB.GetMaster( GetID() );

	ASSERT( itr );

	CMasterProperty mp(0, this);
	mp.SetReadOnlyProperties( *itr , pROP );	
#else
	CString strState = GuiRect::LoadStringInt( IDS_PROP_STATE );
	MasterSettings* itr = m_DB.GetMaster( GetID() );
	if( !itr->m_NetState.IsSet() )
	{
		CString strOk = GuiRect::LoadStringInt(IDS_PROP_NODATA);
		pROP->SetReadOnlyProperty( strState, strOk );	
	}
	else
	{
		std::shared_ptr< DepCfg::IResourceLoader > p = 
			DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
			RESOURCE_DLL_INTERFACE
			);
		pROP->SetReadOnlyProperty( strState, p->LoadCameraState( itr->m_NetState.Get() ) ); 
	}
#endif
}