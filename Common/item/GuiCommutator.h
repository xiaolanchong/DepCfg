// GuiCommutator.h: interface for the GuiCommutator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICOMMUTATOR_H__D5BF215E_A3B8_4512_A6F5_6ABCC3BA90D7__INCLUDED_)
#define AFX_GUICOMMUTATOR_H__D5BF215E_A3B8_4512_A6F5_6ABCC3BA90D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiRect.h"

class GuiCommutator : public GuiRect  , private PropertyObserver  
{
	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
	virtual CRectF	GetDragRect() const 
	{
		CRectF rc(GetRect());
		rc.Deflate(0, 0, rc.Width/5, 0);
		return rc;
	}	

	void	DrawSlots( Graphics& gr, const Zoom& zoom ) const;
	void	DrawSlots( CDC* pDC, const Zoom& zoom ) const;
public:
	GuiCommutator(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiRect(db,dwID,rcItem){}

	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
	virtual void					SetProperties( PropertyCB* pPropCB ) ;
	virtual void					SetReadOnlyProperty(ReadOnlyProperty* pROP);
private:
	virtual void					OnPropertyChange(CString Name, const boost::any& Value) ;
	virtual std::set<DWORD>			OnDeleteItem() const;
public:
	virtual std::pair<bool, DWORD>	SlotFromPoint	(PointF point) const;
};

#endif // !defined(AFX_GUICOMMUTATOR_H__D5BF215E_A3B8_4512_A6F5_6ABCC3BA90D7__INCLUDED_)
