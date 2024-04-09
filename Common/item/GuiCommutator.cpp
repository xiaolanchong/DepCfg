// GuiCommutator.cpp: implementation of the GuiCommutator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiCommutator.h"
#include "..\utility\Helper.h"
#ifndef DEPCFGLITE
#include "CameraProperty.h"
#endif


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void	GuiCommutator::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	CRectF rc = zoom.GetRect(GetRect());
	CRectF rcItem(rc);
	int Width = rc.Width/5;
	Pen pen(Color::Blue, 1);
	
	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );

	rc.Deflate( 0, 0, Width, 0 );
	GuiRect::FillRect(gr, rc);
	gr.DrawRectangle( &pen, rc);

	std::map<DWORD, PointF>	pt  = GetSlotPoints(); 
	for(size_t i=0 ; i<pt.size(); i++)
	{
		pt[i] = zoom.GetPoint(pt[i]); 
		gr.DrawLine( &pen, rc.Right(), pt[i].Y, rcItem.Right(), pt[i].Y);
	}

	const CommutatorSettings* cs = m_DB.GetCommutator( GetID()  );
	CString Value = cs->m_strName.IsSet()? cs->m_strName.Get() : CString(); 

	GuiRect::DrawItemHeader( gr, rc, Value );
}

void	GuiCommutator::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rc = zoom.GetRect(GetRect());
	CRectF rcItem(rc);
	int Width = rc.Width/5;

	rc.Deflate( 0, 0, Width, 0 );
	CRect rcRect(rc.ToCRect());
	pDC->Rectangle(&rcRect);

	std::map<DWORD, PointF>	pt  = GetSlotPoints(); 
	for(size_t i=0 ; i<pt.size(); i++)
	{
		pt[i] = zoom.GetPoint(pt[i]); 
		pDC->MoveTo( rc.Right(), pt[i].Y);
		pDC->LineTo(	 rcItem.Right(), pt[i].Y);
	}	
}

std::pair<bool,DWORD>	GuiCommutator::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{
	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first || !m_DB.IsFreeSlot( GetID(), p.second )) return std::make_pair(false, 0);


	const SlaveSettings*itrSlave = m_DB.GetSlave(dwID);
	const MasterSettings* itrMaster = m_DB.GetMaster(dwID);
	
	if( !itrSlave &&  !itrMaster  )
		return std::make_pair(false, 0);

	return p;
}

std::map<DWORD, PointF>	GuiCommutator::GetSlotPoints() const 
{
	std::map<DWORD, PointF> pt;
	CRectF rcItem = GetRect();
	DWORD dwSlots = m_DB.GetCommutatorSlotNumber( GetID() );
	for(size_t i=0 ; i< dwSlots; i++)
	{
		pt.insert(std::make_pair( i, PointF(rcItem.Right(), rcItem.Top() + rcItem.Height*(i+1)/(dwSlots+1) )) );
	}
	return pt;
}

#ifndef DEPCFGLITE
void	GuiCommutator::SetProperties(PropertyCB* pPropCB) 
{
	CCommutatorProperty cp(  &m_DB, GetID(), pPropCB, this );

	CommutatorSettings *cs = m_DB.GetCommutator( GetID() );

	ASSERT( cs);

	cp.SetProperties(  *cs );
}

void GuiCommutator::OnPropertyChange(CString Name, const boost::any& Value) 
{
	CCommutatorProperty cp( &m_DB, GetID(),  0, 0 );

	CString val = boost::any_cast<CString>(Value);
	CommutatorSettings *cs = m_DB.GetCommutator( GetID() );

	ASSERT( cs);

	cp.OnPropertyChange( *cs , Name,  val);
	((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument()->SetModifiedFlag();
	AfxGetMainWnd()->Invalidate();
}

void	GuiCommutator::SetReadOnlyProperty(ReadOnlyProperty* pROP)
{
	CCommutatorProperty cp(  &m_DB, GetID(), 0, 0 );
	CommutatorSettings *cs = m_DB.GetCommutator( GetID() );
	ASSERT( cs);
	cp.SetReadOnlyProperties(  *cs , pROP);	
}
#else

void	GuiCommutator::SetProperties(PropertyCB* pPropCB) 
{

}

void GuiCommutator::OnPropertyChange(CString Name, const boost::any& Value) 
{
}

void	GuiCommutator::SetReadOnlyProperty(ReadOnlyProperty* pROP)
{
}

#endif

std::set<DWORD>		GuiCommutator::OnDeleteItem() const
{
	std::set<DWORD> s;
	return s;
}

std::pair<bool, DWORD>	GuiCommutator::SlotFromPoint	(PointF point) const
{
	CRectF rc = GetRect();
	DWORD dwSlots = m_DB.GetCommutatorSlotNumber( GetID() );
	SizeF sizeConnect = SizeF(25.0f, rc.Height/dwSlots);
	if(dwSlots < 3) sizeConnect.Height = 25.0f;
	std::map<DWORD, PointF> pt = GetSlotPoints();
	std::map<DWORD, PointF>::const_iterator itr = pt.begin();
	for( ; itr != pt.end() ; ++itr)
	{
		RectF rc( itr->second, sizeConnect );
		rc.Offset(-sizeConnect.Width/2,-sizeConnect.Height/2);
		if(rc.Contains( point )) return std::make_pair( true, itr->first);
	}
	return std::make_pair( false, 0 );
}

void	GuiCommutator::DrawSlots( Graphics& gr, const Zoom& zoom ) const
{

}

void	GuiCommutator::DrawSlots( CDC* pDC, const Zoom& zoom ) const
{

}