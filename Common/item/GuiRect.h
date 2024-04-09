// GuiRect.h: interface for the GuiRect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIRECT_H__AE0D6092_7539_4146_B62C_6722DE6FC206__INCLUDED_)
#define AFX_GUIRECT_H__AE0D6092_7539_4146_B62C_6722DE6FC206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProjectDB.h"
#include <map>

class GuiRect : public GuiItem
{

	CRectF		m_rcItem;
protected:
	Action		m_CurAction;

	virtual std::map<DWORD, PointF>  	GetSlotPoints (  ) const = 0;	
	virtual CRectF	GetDragRect() const = 0;

	void	FillRect( Graphics& gr, const CRectF& rc) const;
	void	DrawItemHeader( Graphics& gr, const CRectF& rc, CString sHeader ) const;
	CString	LoadStringInt( UINT nID );
public:
	GuiRect(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiItem(db,dwID), m_rcItem(rcItem){}

	virtual void	Draw(Graphics& gr)	const	;

	virtual	Action	GetAction(PointF point)			const	;
	virtual void	StartAction(PointF point)				;
	virtual void	DoAction(SizeF size)					;
	virtual void	EndAction(PointF point)					;
	virtual void	CancelAction()							;				;	

	virtual PointF					PointFromSlot	(DWORD dwSlot) const throw(SlotException) ;
	virtual std::pair<bool, DWORD>	SlotFromPoint	(PointF point) const ;		// <-- need GAP!!!

	CRectF GetRect() const { return m_rcItem;}

	void	Offset(SizeF size) { m_rcItem.Offset(size); }

//	virtual std::pair<bool, DWORD>	IsSlot(CPoint point) const { return std::make_pair(false, 0);};
};

#endif // !defined(AFX_GUIRECT_H__AE0D6092_7539_4146_B62C_6722DE6FC206__INCLUDED_)
