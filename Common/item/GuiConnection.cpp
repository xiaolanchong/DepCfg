// GuiConnection.cpp: implementation of the GuiConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "GuiConnection.h"
#include "GuiComputer.h"

#include "PointDetect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void	GuiConnection::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	Pen pen(Color(0,0,0),1);

	if(m_State == selected) pen.SetWidth(2);

	const Connection *itr  = m_DB.GetConnection( GetID() );

	GuiItem* First = m_GuiDB.GetFromID( itr->First );
	GuiItem* Second = m_GuiDB.GetFromID( itr->Second );

	PointF pt1 = First->PointFromSlot(itr->FirstSlot);
	PointF pt2 = Second->PointFromSlot(itr->SecondSlot);

	pt1 = zoom.GetPoint(pt1);
	pt2 = zoom.GetPoint(pt2);

	gr.DrawLine( &pen, pt1, pt2 ); 

/*
	PointF ptF(  (pt1.X + pt2.X)/2, pt1.Y )  , ptS ( (pt1.X + pt2.X)/2, pt2.Y);

	gr.DrawLine( &pen, pt1, ptF );
	gr.DrawLine( &pen, ptF, ptS );
	gr.DrawLine( &pen, ptS, pt2 );*/
}

void	GuiConnection::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	const Connection *itr  = m_DB.GetConnection( GetID() );

	GuiItem* First = m_GuiDB.GetFromID( itr->First );
	GuiItem* Second = m_GuiDB.GetFromID( itr->Second );

	PointF pt1 = First->PointFromSlot(itr->FirstSlot);
	PointF pt2 = Second->PointFromSlot(itr->SecondSlot);

	pt1 = zoom.GetPoint(pt1);
	pt2 = zoom.GetPoint(pt2);

	pDC->MoveTo (pt1.X, pt1.Y ); 	
	pDC->LineTo (pt2.X, pt2.Y );
}

GuiItem::Action	GuiConnection::GetAction(PointF point)			const
{

	const Connection*  itr  = m_DB.GetConnection( GetID() );

	GuiItem* First = m_GuiDB.GetFromID( itr->First );
	GuiItem* Second = m_GuiDB.GetFromID( itr->Second );

	PointF pt1 = First->PointFromSlot(itr->FirstSlot);
	PointF pt2 = Second->PointFromSlot(itr->SecondSlot);

	CPoint pt[]= { CPoint(pt1.X, pt1.Y), CPoint(pt2.X, pt2.Y) };
	bool res = PtInLine( CPoint(point.X, point.Y), pt, 2, 10 );


	return res? resize : none;
}

void	GuiConnection::StartAction(PointF point)		
{

}


void	GuiConnection::DoAction(SizeF size)			
{

}

void	GuiConnection::EndAction(PointF size)			
{
	m_CurAction = none;
}

void	GuiConnection::CancelAction()
{
	m_CurAction = none;
}

std::pair<bool,DWORD>	GuiConnection::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{
	return std::make_pair(false, 0);
}

PointF	GuiConnection::PointFromSlot(DWORD dwSlot) const throw(SlotException)
{

	throw SlotException("Connection slot");
	return  PointF();
}


std::set<DWORD>		GuiConnection::OnDeleteItem() const
{
	std::set<DWORD> s;
	s.insert(GetID());
	return s;
}

/////////////////////////////

void	GuiLine::Draw(Graphics& gr, const Zoom& zoom)	const	
{
	Pen pen(Color(0,0,0), 1);
//	pen.SetDashStyle(  DashStyleDash);

	PointF	pt1 = zoom.GetPoint(m_ptStart), 
			pt2 = zoom.GetPoint(m_ptEnd);
	gr.DrawLine( &pen, pt1, pt2  );
}

void	GuiLine::DrawReversable(CDC* pDC, const Zoom& zoom)	const
{
	CPen pen;
	pen.CreatePen( PS_SOLID, 1, Color::Black);

	PointF	pt1 = zoom.GetPoint(m_ptStart), 
			pt2 = zoom.GetPoint(m_ptEnd);	
	
	CPen* pOldPen = pDC->SelectObject( &pen );
	pDC->MoveTo( pt1.X, pt1.Y );
	pDC->LineTo( pt2.X , pt2.Y);
	pDC->SelectObject( pOldPen );
}

GuiItem::Action	GuiLine::GetAction(PointF point)			const	
{
	return none;
}

void	GuiLine::StartAction(PointF point)				
{
	
}

void	GuiLine::DoAction(SizeF size)					
{
	m_ptEnd.X += size.Width;
	m_ptEnd.Y += size.Height;
}

void	GuiLine::EndAction(PointF point)					
{
	m_ptEnd = point;
}


void	GuiLine::CancelAction()									
{

}

std::pair<bool,DWORD>	GuiLine::CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const
{
	return std::make_pair(false, 0);
}

PointF	GuiLine::PointFromSlot(DWORD dwSlot) const throw(SlotException)
{

	throw SlotException("Line slot");
	return  PointF();
}
