// GuiComputer.h: interface for the GuiComputer class.
//
//////////////////////////////////////////////////////////////////////




#if !defined(AFX_GUICOMPUTER_H__C30CBD71_3B22_48A1_8766_67B34383EAB1__INCLUDED_)
#define AFX_GUICOMPUTER_H__C30CBD71_3B22_48A1_8766_67B34383EAB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiRect.h"

class GuiComputer : public GuiRect  , private PropertyObserver
{
	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
	virtual CRectF	GetDragRect() const 
	{
		CRectF rc(GetRect());
		rc.Deflate(0, 0, rc.Width/5, 0);
		return rc;
	}
public:
	GuiComputer(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiRect(db,dwID,rcItem){}

	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;

	virtual Action					GetAction(PointF point)			const;
	
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
	virtual void					SetProperties( PropertyCB* pPropCB ) ;
	virtual void					SetReadOnlyProperty(ReadOnlyProperty* pROP);
private:
	virtual void					OnPropertyChange(CString Name, const std::any& Value) ;
	virtual std::set<DWORD>			OnDeleteItem() const ;
};

#endif // !defined(AFX_GUICOMPUTER_H__C30CBD71_3B22_48A1_8766_67B34383EAB1__INCLUDED_)
