// GuiComputer.cpp: implementation of the GuiComputer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiComputer.h"
#include "..\utility\Helper.h"
#include "..\..\DepCfgLite\interface\IResourceLoader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

	


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void	GuiComputer::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	CRectF rc = zoom.GetRect(GetRect());
	CRectF rcItem (rc);
	int Width = rc.Width/5;
	int Height = rc.Height;

	const ComputerSettings* cs = m_DB.GetComputer( GetID()  );
#ifndef DEPCFGLITE
	Pen pen(Color::Red, 1);
#else
	Pen pen(Color::Black, 2);

	if( !cs->m_dwNetState.IsSet() )
	{
		pen.SetColor( Color::LightGray );
	}
	else if( cs->m_dwNetState.Get() == 2 )
	{
		pen.SetColor(Color::Red);
	}
#endif

	rc.Deflate(0, 0, Width, 0);
	
	GuiRect::FillRect(gr, rc);

	if (GetState() == highlighted)
		pen.SetWidth( 2);
	else if(GetState() == selected)
		pen.SetDashStyle( DashStyleDot );
	gr.DrawRectangle( &pen, rc);

	std::map<DWORD, PointF>	pt  = GetSlotPoints(); 
	for(int i=0 ; i < c_MaxConputerSlots; i++)
	{
		pt[i] = zoom.GetPoint(pt[i]);
		gr.DrawLine( &pen, rc.Right(), (pt[i].Y), rcItem.Right(), (pt[i].Y));
	}


	// modified for dns name
	CString Value = CString();
	
	if( cs->m_strDnsName.IsSet() && !cs->m_strDnsName.Get().IsEmpty() )
	{
		Value = cs->m_strDnsName.Get();
	} 
	else if( cs->m_strIP.IsSet() )
	{
		Value = cs->m_strIP.Get();
	}

	GuiRect::DrawItemHeader( gr, rc, Value );
}

void	GuiComputer::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CRectF rc = zoom.GetRect(GetRect());
	CRectF rcItem (rc);
	int Width = rc.Width/5;
	int Height = rc.Height;

	rc.Deflate(0, 0, Width, 0);

	CRect rcRect( rc.ToCRect() );
	
	rcRect.InflateRect(0, 0, 1, 1);
	pDC->Rectangle( &rcRect );

	std::map<DWORD, PointF>	pt  = GetSlotPoints(); 
	for(int i=0 ; i < c_MaxConputerSlots; i++)
	{
		pt[i] = zoom.GetPoint(pt[i]);
		//.DrawLine( &pen, rc.Right(), (pt[i].Y), rcItem.Right(), (pt[i].Y));

		pDC->MoveTo( rc.Right(),  pt[i].Y);
		pDC->LineTo( rcItem.Right(), pt[i].Y );
	}


}

std::pair<bool,DWORD>	GuiComputer::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{
	std::pair<bool, DWORD> p = SlotFromPoint(point) ;
	if( !p.first/* || !m_DB.IsFreeSlot( GetID(), p.second )*/) return std::make_pair(false, 0);


	const SlaveSettings*	Slave	= m_DB.GetSlave(dwID);
	const MasterSettings*	Master	= m_DB.GetMaster(dwID);
	
	bool	bFree = m_DB.IsFreeSlot( GetID(), p.second);

	if( Master  )
		return std::make_pair( bFree , p.second);
	else if( Slave )
	{
		if( dwSlot == 0 ) return std::make_pair( bFree , p.second);
		else return p;
	}
	return std::make_pair(false, 0);
}

std::map<DWORD, PointF>	GuiComputer::GetSlotPoints() const 
{
	std::map<DWORD, PointF> pt;
	CRectF rcItem = GetRect();
	for(int i=0 ; i < c_MaxConputerSlots; i++)
	{
		pt.insert( std::make_pair( i, PointF(rcItem.Right(), rcItem.Top() + rcItem.Height*(i+1)/(c_MaxConputerSlots + 1) )) );
	}
	return pt;
}

void	GuiComputer::SetProperties(PropertyCB* pPropCB) 
{
	const ComputerSettings* itr = m_DB.GetComputer( GetID() );
	CString Value = itr->m_strIP.IsSet() ? itr->m_strIP.Get() : CString();
	pPropCB->SetIpAddressProperty( LoadStringInt( IDS_PROP_COMPUTER_IP ), Value, this );
	Value = itr->m_strDnsName.IsSet() ? itr->m_strDnsName.Get() : CString();
	pPropCB->SetEditProperty( LoadStringInt( IDS_PROP_COMPUTER_DNS_NAME ), Value, this);
}

void GuiComputer::OnPropertyChange(CString Name, const boost::any& Value) 
{
	CString str = boost::any_cast<CString>(Value);
	ComputerSettings* itr = m_DB.GetComputer( GetID() );

	CString strIP		= LoadStringInt( IDS_PROP_COMPUTER_IP ),
			strDnsName	= LoadStringInt( IDS_PROP_COMPUTER_DNS_NAME );
	if( Name == strIP )
	{
		itr->m_strIP		= str;
	}
	else if( Name == strDnsName )
	{
		itr->m_strDnsName	= str;
	}

	((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument()->SetModifiedFlag();
	CWnd* pWnd = AfxGetMainWnd();
	pWnd->Invalidate();
}


std::set<DWORD>		GuiComputer::OnDeleteItem() const
{
	std::set<DWORD> s;
	return s;
}

void	GuiComputer::SetReadOnlyProperty(ReadOnlyProperty* pROP)
{
	const ComputerSettings* itr = m_DB.GetComputer( GetID() );
	CString Value = itr->m_strIP.IsSet() ? itr->m_strIP.Get() : CString();
	pROP->SetReadOnlyProperty( LoadStringInt( IDS_PROP_COMPUTER_IP ), Value );	

	Value = itr->m_strDnsName.IsSet() ? itr->m_strDnsName.Get() : CString();
	pROP->SetReadOnlyProperty( LoadStringInt( IDS_PROP_COMPUTER_DNS_NAME ), Value);
}

GuiItem::Action	GuiComputer::GetAction(PointF point)			const	
{
	CRectF rcDef(GetRect());
	
	std::pair<bool, DWORD> p = SlotFromPoint(  PointF(point.X, point.Y)  ) ;
	if(p.first  /*m_DB.IsFreeSlot( GetID(), p.second )*/  ) 
	{

		if(	m_DB.IsFreeSlot( GetID(), p.second )  || 
			m_DB.IsAllSlave( GetID(), p.second ) )
			return connect;
	}
	

	rcDef = GetDragRect();

	if(rcDef.Contains( point)) return GuiItem::drag;
	return none;	
}