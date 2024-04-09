// GuiMaster.h: interface for the GuiMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIMASTER_H__4662CBA2_ACF1_41AC_AD57_8166CF98A911__INCLUDED_)
#define AFX_GUIMASTER_H__4662CBA2_ACF1_41AC_AD57_8166CF98A911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GuiRect.h"
#ifndef DEPCFGLITE
#include "CameraProperty.h"
#endif

class GuiMaster :	public GuiRect  , 
					private PropertyObserver
{
protected:
	virtual CRectF	GetDragRect() const 
	{
		CRectF rc(GetRect());
		rc.Deflate(rc.Width/5, 0, 0, 0);
		return rc;
	}

	
public:
	GuiMaster(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiRect(db,dwID,rcItem){}


	virtual void					SetProperties( PropertyCB* pPropCB ) ;
private:
	virtual void					OnPropertyChange(CString Name, const std::any& Value) ;
	virtual void					SetReadOnlyProperty(ReadOnlyProperty* pROP);
	virtual std::set<DWORD>			OnDeleteItem() const ;
};

class GuiPhysicMaster : public GuiMaster
{
public:
	GuiPhysicMaster(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiMaster(db,dwID,rcItem){}

	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;

//	virtual Action					GetAction(PointF point)			const;
	
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
};

class GuiLogicMaster : public GuiMaster
{
protected:
	virtual std::map<DWORD, PointF>	GetSlotPoints() const ;
public:

	virtual void					Draw(Graphics& gr, const Zoom& zoom)	const	;
	virtual void					DrawReversable(CDC* pDC, const Zoom& zoom)	const;
	GuiLogicMaster(CLogicDB& db, DWORD dwID, RectF rcItem) : GuiMaster(db,dwID,rcItem){}
	virtual std::pair<bool, DWORD>	CanConnect(DWORD dwID, DWORD dwSlot, PointF point) const;
};

#endif // !defined(AFX_GUIMASTER_H__4662CBA2_ACF1_41AC_AD57_8166CF98A911__INCLUDED_)
