// GuiConnection.h: interface for the GuiConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICONNECTION_H__BF61693B_FA0E_4739_B37B_3ED6DA29B56B__INCLUDED_)
#define AFX_GUICONNECTION_H__BF61693B_FA0E_4739_B37B_3ED6DA29B56B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProjectDB.h"

class GuiConnection : public GuiItem
{
protected:
	Action		m_CurAction;
	CGuiDB&		m_GuiDB;
	
public:
	GuiConnection(CGuiDB& db, DWORD dwID) : GuiItem(db.GetDB(),dwID), m_GuiDB(db){}

	virtual void	Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void	DrawReversable(CDC* pDC, const Zoom& zoom)	const;

	virtual	Action	GetAction(PointF point)			const	;
	virtual void	StartAction(PointF point)				;
	virtual void	DoAction(SizeF size)					;
	virtual void	EndAction(PointF point)					;
	virtual void	CancelAction()
		;				;	
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
	virtual PointF	PointFromSlot(DWORD dwSlot) const throw(SlotException);

	virtual std::pair<bool, DWORD>	SlotFromPoint(PointF point) const { return std::make_pair(false, 0);};

	virtual std::set<DWORD>		OnDeleteItem() const ;

};

/////////////////////////////////////////////////////////

class GuiLine : public GuiItem
{
	PointF m_ptStart, m_ptEnd;
	DWORD	m_Slot;
public:
	GuiLine(CLogicDB& db, DWORD dwID, DWORD Slot, PointF ptSlot, PointF ptMouse) : 
		GuiItem(db,dwID), 
		m_ptStart(ptSlot), m_ptEnd(ptMouse), m_Slot(Slot){}

	virtual void	Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void	DrawReversable(CDC* pDC, const Zoom& zoom)	const;

	virtual	Action	GetAction(PointF point)			const	;
	virtual void	StartAction(PointF point)				;
	virtual void	DoAction(SizeF size)					;
	virtual void	EndAction(PointF point)					;
	virtual void	CancelAction()							;				;	
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
	
	virtual PointF	PointFromSlot(DWORD dwSlot) const throw(SlotException);
	virtual std::pair<bool, DWORD>	SlotFromPoint(PointF point) const { return std::make_pair(false, 0);};

	DWORD	GetSlot() const { return m_Slot;}

	virtual std::set<DWORD>		OnDeleteItem() const { return std::set<DWORD>();};
};

#endif // !defined(AFX_GUICONNECTION_H__BF61693B_FA0E_4739_B37B_3ED6DA29B56B__INCLUDED_)
