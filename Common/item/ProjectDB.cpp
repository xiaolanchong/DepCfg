// ProjectDB.cpp: implementation of the CProjectDB class.
//
//////////////////////////////////////////////////////////////////////



#include "stdafx.h"
#include <algorithm>
#include "ProjectDB.h"

#include "GuiComputer.h"
#include "GuiConnection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGuiDB::CGuiDB(CLogicDB& db, CDocument* pDoc): 
	m_DB(db), 
	m_pCB(0), 
	m_pDoc(pDoc),
#ifdef DEPCFGLITE
	m_bDrawCached(false)
#else
	m_bRedraw(true)
#endif
{

}

#ifdef DEPCFGLITE
void	CGuiDB::Draw(Graphics& gr, const Zoom& zoom)	
{
	DBList_t::reverse_iterator itr = m_GuiDB.rbegin();
	for( ; itr != m_GuiDB.rend(); ++itr)
	{
		DWORD dwID = (*itr)->GetID();
		(*itr)->Draw(gr, zoom );	
	}
}

void		CGuiDB::	Draw(CDC* pDC, const Zoom& zoom)
{
	pDC->BitBlt( 0, 0, m_Cache.size.cx, m_Cache.size.cy, &m_Cache.dc, 0, 0, SRCCOPY );	
}

#else

void	CGuiDB::DrawCached(Graphics& gr, const Zoom& zoom)	const
{
	if(m_bRedraw)
	{	
		PrepareMemDC();	
		RedrawMemDC();
		m_bRedraw = false;
	}
	HDC hDC = gr.GetHDC();
	::BitBlt( hDC, 0, 0, m_Cache.size.cx, m_Cache.size.cy, m_Cache.dc.GetSafeHdc(), 0, 0, SRCCOPY );
}

void	CGuiDB::DrawUncached(Graphics& gr, const Zoom& zoom)	const
{
	DBList_t::const_reverse_iterator itr = m_GuiDB.rbegin();
	
	for( ; itr != m_GuiDB.rend(); ++itr)
	{
		DWORD dwID = (*itr)->GetID();
		(*itr)->Draw(gr, zoom );	
	}
}

void	CGuiDB::Invalidate( CSize size)
{
/*	m_Cache = std::shared_ptr<Bitmap>
		(
		::new Bitmap( size.cx, size.cy, PixelFormat24bppRGB  ), Helper::Deleter<Bitmap>()
		);*/
	if(! IsWindow(AfxGetMainWnd()->GetSafeHwnd()) ) return;
//	size += CSize(200, 200);
	m_Cache.bmp.DeleteObject();
	m_Cache.dc.DeleteDC();
	m_Cache.size = size;
	CDC* pDC = AfxGetMainWnd()->GetDC();
	m_Cache.dc.CreateCompatibleDC(pDC);
	m_Cache.bmp.CreateCompatibleBitmap(pDC, size.cx, size.cy);
	m_Cache.dc.SelectObject( &m_Cache.bmp );
	AfxGetMainWnd()->ReleaseDC(pDC);

	m_bRedraw = true;
}

void	CGuiDB::Invalidate()
{
	m_bRedraw = true;
}

#endif

void CGuiDB::AddItem( GuiItem* pItem )
{
	m_GuiDB.push_back( std::shared_ptr<GuiItem>( pItem ) );
#ifndef DEPCFGLITE
	Invalidate();
#endif
}

void	CGuiDB::PrepareMemDC() const
{
	Graphics BmpBuffer(m_Cache.dc.GetSafeHdc());
	BmpBuffer.Clear( Color::White );

	DBList_t::const_reverse_iterator itr = m_GuiDB.rbegin();
	for( ; itr != m_GuiDB.rend(); ++itr)
	{
		DWORD dwID = (*itr)->GetID();
		if( m_CurItems.find( *itr ) == m_CurItems.end()  )  
			(*itr)->Draw(BmpBuffer, m_Zoom );	
	}
}


#ifndef DEPCFGLITE


void	CGuiDB::RedrawMemDC() const
{
	if( !m_Cache.dc.GetSafeHdc()) return;
	int nROP = m_Cache.dc.SetROP2(R2_NOTXORPEN  );
	CPen pen;
	pen.CreatePen(PS_DOT, 1, Color::Black);
	CBrush * pOldBrush = (CBrush*)m_Cache.dc.SelectStockObject( NULL_BRUSH );
	CPen* pOldPen = (CPen*)m_Cache.dc.SelectObject( &pen );
	
	std::set< std::shared_ptr<GuiItem> >::const_iterator itr = m_CurItems.begin();
	for( ; itr != m_CurItems.end(); ++itr)
	{
		(*itr)->DrawReversable( &m_Cache.dc, m_Zoom );
	}

	m_Cache.dc.SelectObject( pOldBrush );
	m_Cache.dc.SelectObject( pOldPen );
	m_Cache.dc.SetROP2(nROP);	
}

#else

void	CGuiDB::DrawMemDC() const
{
	if( !m_Cache.dc.GetSafeHdc()) return;
	int nROP = m_Cache.dc.SetROP2(R2_NOTXORPEN  );
	CPen pen;
	pen.CreatePen(PS_DOT, 1, Color::Black);
	CBrush * pOldBrush = (CBrush*)m_Cache.dc.SelectStockObject( NULL_BRUSH );
	CPen* pOldPen = (CPen*)m_Cache.dc.SelectObject( &pen );
	
	std::set< std::shared_ptr<GuiItem> >::const_iterator itr = m_CurItems.begin();
	for( ; itr != m_CurItems.end(); ++itr)
	{
		(*itr)->DrawReversable( &m_Cache.dc, m_Zoom );
	}

	m_Cache.dc.SelectObject( pOldBrush );
	m_Cache.dc.SelectObject( pOldPen );
	m_Cache.dc.SetROP2(nROP);	
}

void	CGuiDB::Resize( CSize size)
{
	if(! IsWindow(AfxGetMainWnd()->GetSafeHwnd()) ) return;
	if(!m_bDrawCached) return;
	m_Cache.bmp.DeleteObject();
	m_Cache.dc.DeleteDC();
	m_Cache.size = size;
	CDC* pDC = AfxGetMainWnd()->GetDC();
	m_Cache.dc.CreateCompatibleDC(pDC);
	m_Cache.bmp.CreateCompatibleBitmap(pDC, size.cx, size.cy);
	m_Cache.dc.SelectObject( &m_Cache.bmp );
	AfxGetMainWnd()->ReleaseDC(pDC);

	PrepareMemDC();
	DrawMemDC();
}

#endif

bool	CGuiDB::StartAction(PointF point)			
{
//	InterPoint = point;
	DBList_t::iterator itr = m_GuiDB.begin();
	m_ptPrev = point;
	std::shared_ptr<GuiItem> Current;
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		GuiItem::Action act = (*itr)->GetAction( point  ) ;
		switch(act)
		{
		case GuiItem::drag:
		case GuiItem::resize:
			{
				//! забросим элемент наверх, чтобы отрисовывался поверх всех
			std::shared_ptr<GuiItem> pItem = *itr; 
			m_GuiDB.erase(itr);
			m_GuiDB.push_front( pItem );
			itr = m_GuiDB.begin();
			m_CurItems.insert( *itr );
			Current = (*itr);	
			Current->StartAction( point );
			UnselectAll();
			Current->SetState(GuiItem::selected);
			if(m_pCB) Current->SetProperties( m_pCB );
			{
				std::set<DWORD> s = m_DB.GetAllConnections( Current->GetID() );
				DBList_t::iterator itr = m_GuiDB.begin();
				for( ; itr != m_GuiDB.end(); /*++itr*/)
				{
					if(s.find( (*itr)->GetID() ) != s.end()) 
					{
						//! также забрасываем все соединенные элементы наверх
						std::shared_ptr<GuiItem> pItem = *itr; 
						DBList_t::iterator itAfter = m_GuiDB.erase(itr);
						m_GuiDB.push_front( pItem );
						itr = m_GuiDB.begin();
						m_CurItems.insert( *itr);
						itr = itAfter;
					}
					else ++itr;
				}
			}

#ifdef DEPCFGLITE
			m_bDrawCached = true;
#else
			PrepareMemDC();
			RedrawMemDC();
#endif
			return true;
			}
		case GuiItem::connect : 
			{
			std::pair<bool, DWORD> p = (*itr)->SlotFromPoint(  point );
			PointF ptSlot = (*itr)->PointFromSlot(p.second);
			Current = std::shared_ptr<GuiItem>( new GuiLine(m_DB, (*itr)->GetID(), p.second, ptSlot, point) );
			m_CurItems.insert( Current );
			UnselectAll();
			(*itr)->SetState(GuiItem::highlighted);
#ifdef DEPCFGLITE
			m_bDrawCached = true;
#endif
			return true;
			}
		}
	}
	UnselectAll();
#ifndef DEPCFGLITE
	Invalidate();
#endif
	return false;
}

void	CGuiDB::DoAction(SizeF	size)		
{
	PointF pt = m_ptPrev;
	pt.X += size.Width;
	pt.Y += size.Height;
	DoAction( pt );
}

void	CGuiDB::DoAction(PointF pt)				
{
	SizeF size( pt.X - m_ptPrev.X, pt.Y - m_ptPrev.Y);
	m_ptPrev =  pt;
	if( !m_CurItems.empty() ) 
	{
#ifdef DEPCFGLITE
		DrawMemDC();
#else
		RedrawMemDC();
#endif
		std::for_each( m_CurItems.begin(), m_CurItems.end(), 
			[size](auto item) { item->DoAction(size); });
#ifdef DEPCFGLITE
		DrawMemDC();
#else
		RedrawMemDC();
#endif
		m_pDoc->SetModifiedFlag();
	}
	else return;
	DBList_t::const_iterator itr = m_GuiDB.begin();
	GuiLine * pLine = dynamic_cast<GuiLine*>( m_CurItems.begin()->get() );
	for( ; itr != m_GuiDB.end(); ++itr)
	{

		GuiItem::Action act = (*itr)->GetAction( pt  ) ;
		switch(act)
		{
		case GuiItem::connect : 
			
			if(pLine)
			{
				std::pair<bool, DWORD> p = (*itr)->CanConnect( pLine->GetID(), pLine->GetSlot(), pt );
				if(p.first)(*itr)->SetState(GuiItem::highlighted);
				PrepareMemDC();
#ifdef DEPCFGLITE
		DrawMemDC();
#else
		RedrawMemDC();
#endif
				
			}
			break;
		default:;
			if((*itr)->GetState() == GuiItem::highlighted && itr->get() != pLine) 
			{
				(*itr)->SetState((GuiItem::usual));
				PrepareMemDC();
#ifdef DEPCFGLITE
		DrawMemDC();
#else
		RedrawMemDC();
#endif
			}
		}
	}
}


void	CGuiDB::EndAction(PointF point)							
{
	if( !m_CurItems.empty() ) 
	{
		std::for_each( m_CurItems.begin(), m_CurItems.end(), 
			[point](auto item) { item->EndAction(point); });
	//	m_Cur->EndAction(point);
		GuiLine * pLine = dynamic_cast<GuiLine*>( m_CurItems.begin()->get() );

		if(pLine)
		{
			DBList_t::const_iterator itr = m_GuiDB.begin();
			for( ; itr != m_GuiDB.end(); ++itr)
			{
				GuiItem::Action act = (*itr)->GetAction( point  ) ;
				if(GuiItem::connect == act)
				{
					Connection c;
					c.First  = pLine->GetID();
					c.Second = (*itr)->GetID();
					c.FirstSlot = pLine->GetSlot();

					std::pair<bool, DWORD> p = (*itr)->CanConnect( c.First, c.FirstSlot, point );
					
					c.SecondSlot = p.second;
					if(c.First != c.Second && p.first)
					{
						m_DB.AddConnection( c );
						GuiConnection* p = new GuiConnection( *this, c.GetID());
						m_GuiDB.push_back( std::shared_ptr<GuiItem>(p));
						m_pDoc->SetModifiedFlag();
						
						break;
					}
					
				}
			}
		}
	}
#ifndef DEPCFGLITE
	Invalidate();
#endif
	UnhighliteAll();
	m_CurItems.clear();
}

void	CGuiDB::CancelAction()							
{
	if( !m_CurItems.empty() )
	{
	//	m_Cur->CancelAction();
		std::for_each( m_CurItems.begin(), m_CurItems.end(), 
			[](auto item) { item->CancelAction(); });
	}
#ifndef DEPCFGLITE
	Invalidate();
#endif
	UnhighliteAll();
	m_CurItems.clear();
}

GuiItem* CGuiDB::GetFromID(DWORD dwID)
{
	DBList_t::iterator itr = m_GuiDB.begin();
	
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( dwID ==  (*itr)->GetID()) return itr->get();
	}	
	return 0;
}

void	CGuiDB::UnselectAll()
{
	DBList_t::iterator itr = m_GuiDB.begin();
	
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( GuiItem::selected ==  (*itr)->GetState()) (*itr)->SetState(GuiItem::usual);
	}	
}

void	CGuiDB::UnhighliteAll()
{
	DBList_t::iterator itr = m_GuiDB.begin();
	
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( GuiItem::highlighted ==  (*itr)->GetState()) (*itr)->SetState(GuiItem::usual);
	}	
}

std::set<DWORD>	CGuiDB::GetSelect() const
{
	std::set<DWORD> s;
	DBList_t::const_iterator itr = m_GuiDB.begin();
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if(  (*itr)->GetState() == GuiItem::selected )
		{
			s.insert((*itr)->GetID());
		}
	}
	return s;
}

void	CGuiDB::DeleteItem(const std::set<DWORD>& Items)
{
	DBList_t::iterator itr = m_GuiDB.begin();
	for( ; itr != m_GuiDB.end(); )
	{
		if(  Items.find( (*itr)->GetID() ) != Items.end() )
		{
			m_pDoc->SetModifiedFlag();
			m_GuiDB.erase(itr++);
		}
		else ++itr;
	}
#ifndef DEPCFGLITE
	Invalidate();
#endif
}

CRectF	CGuiDB::GetRect() const
{
	CRectF rc(0,0,0,0);
	
	GuiRect* pRect;
	if(m_GuiDB.empty()) return rc;

	DBList_t::const_iterator itr = m_GuiDB.begin();

//	GuiItem *pi = itr->get();

	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( ( pRect = dynamic_cast<GuiRect*>(itr->get())) )
		{
			rc = pRect->GetRect();
			break;
		}
	}

//	if( ( pRect = dynamic_cast<GuiRect*>(itr->get())) ) rc = pRect->GetRect();

	CRectF rc1(0,0,0,0);
	for( itr = m_GuiDB.begin(); itr != m_GuiDB.end(); ++itr)
	{
		if( ( pRect = dynamic_cast<GuiRect*>(itr->get())) )
		{
			CRectF::Union(rc1, rc, pRect->GetRect());
			rc = rc1;
		}
	}	
	return rc;
}

GuiItem*	CGuiDB::FromPoint(PointF point)
{
	DBList_t::iterator itr = m_GuiDB.begin();
	GuiRect* pRect;
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( ( pRect = dynamic_cast<GuiRect*>(itr->get())) && pRect->GetRect().Contains(point) )
		{
			return itr->get();
		}
	}
	return 0;
}

void		CGuiDB::Offset(SizeF size)
{
	DBList_t::iterator itr = m_GuiDB.begin();
	GuiRect* pRect;
	for( ; itr != m_GuiDB.end(); ++itr)
	{
		if( ( pRect = dynamic_cast<GuiRect*>(itr->get())) )
		{
			pRect->Offset(size);
		}
	}	
}

void		CGuiDB::SetZoom(const Zoom& zoom)
{
	m_Zoom = zoom;
#ifndef DEPCFGLITE
	PrepareMemDC();
	RedrawMemDC();
#endif
}