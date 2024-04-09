// GuiCamera.h: interface for the GuiCamera class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUICAMERA_H__6CEB9D8D_8B49_4B10_9816_BFDFD4D20A39__INCLUDED_)
#define AFX_GUICAMERA_H__6CEB9D8D_8B49_4B10_9816_BFDFD4D20A39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiRect.h"
#ifndef DEPCFGLITE
#include "CameraProperty.h"
#endif

class GuiSlave :	public GuiRect , 
					private PropertyObserver  
{
	
public:
	GuiSlave(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiRect(db,dwID,rcItem){}

	

//	virtual	Action					GetAction(CPoint point)			const	;	
	

	virtual void SetProperties( PropertyCB* pPropCB ) ;
private:
	virtual void OnPropertyChange(CString Name, const std::any& Value) ;
	virtual void					SetReadOnlyProperty(ReadOnlyProperty* pROP);

	virtual std::set<DWORD>		OnDeleteItem() const;
};

class GuiPhysicSlave : public GuiSlave 
{
	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
	virtual CRectF	GetDragRect() const 
	{
		CRectF rc(GetRect());
		rc.Deflate(rc.Width/5, 0,0 , rc.Height/5);
		return rc;
	}	
public:
	GuiPhysicSlave(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiSlave(db,dwID,rcItem){}

	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
	virtual Action					GetAction(PointF point)			const;	

	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;
};

class GuiLogicSlave : public GuiSlave 
{
	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
	virtual CRectF	GetDragRect() const 
	{
		CRectF rc(GetRect());
		rc.Deflate(rc.Width/5, 0,0 , 0);
		return rc;
	}	
public:
	GuiLogicSlave(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiSlave(db,dwID,rcItem){}

	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;
	
					
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
};


#endif // !defined(AFX_GUICAMERA_H__6CEB9D8D_8B49_4B10_9816_BFDFD4D20A39__INCLUDED_)
