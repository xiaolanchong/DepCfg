// ProjectDB.h: interface for the CProjectDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTDB_H__916DE89F_08BC_4F9C_82EF_0BA181DE4171__INCLUDED_)
#define AFX_PROJECTDB_H__916DE89F_08BC_4F9C_82EF_0BA181DE4171__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786)

#include <Gdiplus.h>
using namespace Gdiplus;

#include "LogicDB.h"
#include "Exception.h"
#include "Callback.h"

#include <list>

#include "../utility/RectF.h"

MACRO_EXCEPTION(GuiItemException, CommonException)
MACRO_EXCEPTION(SlotException, GuiItemException)

struct Zoom
{
	float	ScaleX ,ScaleY;
	CSize	Offset;
	
	Zoom() : ScaleX(1.0f), ScaleY(1.0f), Offset(0,0){}
	Zoom(float xx, float yy) : ScaleX(xx), ScaleY(yy), Offset(0,0) {}
	PointF	GetPoint(PointF pt) const
	{ 
		return PointF(( pt.X*ScaleX), ( pt.Y*ScaleY)) /*+ Offset*/; 
	}
	PointF	GetInvPoint(PointF pt) const
	{ 
		return PointF(( pt.X/ScaleX), ( pt.Y/ScaleY))/*- Offset*/; 
	}
	SizeF	GetSize(SizeF sz) const
	{ 
		return SizeF((sz.Width*ScaleX), (sz.Height*ScaleY));
	}
	SizeF	GetInvSize(SizeF sz) const
	{ 
		return SizeF((sz.Width/ScaleX), (sz.Height/ScaleY));
	}
	CRectF	GetRect(CRectF rc) const
	{ 
		return CRectF ( rc.X*ScaleX, rc.Y*ScaleY,  rc.Width*ScaleX, rc.Height*ScaleY );
	}
};
#ifndef DEPCFGLITE
#define WM_UPDATE_TABS		WM_USER + 0x7f
#endif

class GuiItem
{
public:
	enum State
	{
		usual,
		selected,
		highlighted
	};
protected:
	DWORD		m_dwID;						// parent ID in logic
	CLogicDB&	m_DB;

	State		m_State;
public:

	enum Action
	{
		none,
		resize,
		drag,
		connect
	};	

	enum DrawAction
	{
		da_drag,
		da_unselect,
		da_normal
	};

	State	GetState() const { return m_State;}
	void	SetState(State st) { m_State = st;}

public:

	GuiItem(CLogicDB& db, DWORD dwID) : m_DB(db), m_dwID(dwID), m_State(usual){}
	virtual ~GuiItem(){}
	

	DWORD			GetID() const		{ return m_dwID;}
	void			SetID(DWORD dwID)	{ m_dwID = dwID;}
	
	virtual void	SetProperties(PropertyCB* pPropCB)  {};



	virtual void	Draw(Graphics& gr, const Zoom& zoom)	const	= 0;
	virtual void	DrawReversable(CDC* pDC, const Zoom& zoom)	const	= 0;

	virtual	Action	GetAction(PointF point)			const	= 0;

	virtual void	StartAction(PointF point)				= 0;
	virtual void	DoAction(SizeF	size)					= 0;
	virtual void	EndAction(PointF point)					= 0;
	virtual void	CancelAction()							= 0;

	virtual std::pair<bool, DWORD>	CanConnect		(DWORD dwID, DWORD dwSlot, PointF point) const	= 0;

	// throw(SlotException)
	virtual PointF					PointFromSlot	(DWORD dwSlot) const = 0;
	virtual std::pair<bool, DWORD>	SlotFromPoint	(PointF point) const = 0;		// <-- need GAP!!!

	virtual std::set<DWORD>		OnDeleteItem() const = 0;

	virtual void SetReadOnlyProperty(ReadOnlyProperty* pROP) {};
};

//////////////////////////////////////////////////////////////

class CGuiDB 
{	
	friend class CDepCfgDoc;
	friend class CDepCfgFullDoc;

	typedef std::list< std::shared_ptr<GuiItem> >  DBList_t ;

	Zoom						m_Zoom;
	mutable struct Cache
	{
		CDC						dc;
		CBitmap					bmp;
		CSize					size;
	} m_Cache;

	PointF						m_ptPrev;

#ifdef DEPCFGLITE
	bool						m_bDrawCached;
#else
	mutable bool				m_bRedraw;
	void	RedrawMemDC() const;
#endif
	DBList_t					m_GuiDB;
	CDocument*					m_pDoc;
	CLogicDB&					m_DB;

	std::set< std::shared_ptr<GuiItem> > m_CurItems;

	PropertyCB*		m_pCB;				

	void	UnselectAll();
	void	UnhighliteAll();
	
	void	DeleteItem(const std::set<DWORD>& Items);


	void	PrepareMemDC() const;
public:
	CGuiDB(CLogicDB& db, CDocument* pDoc);

#ifdef DEPCFGLITE
	bool	IsCached() const { return m_bDrawCached;}

	void	DrawMemDC() const;
	void			Draw( Graphics& gr, const Zoom& zoom)	;
	void			Draw(CDC* pDC, const Zoom& zoom);
	void			Resize( CSize size);
#else
	void	DrawCached(Graphics& gr, const Zoom& zoom)	const;
	void	DrawUncached(Graphics& gr, const Zoom& zoom) const;
	void	Invalidate(CSize size);		
	void	Invalidate();
#endif

	void	AddItem( GuiItem* pItem );

	bool	StartAction(PointF point)			;
	void	DoAction(PointF pt)				;
	void	DoAction(SizeF	size)		;
	void	EndAction(PointF point)							;
	void	CancelAction()							;
	GuiItem* GetFromID(DWORD dwID);
	CLogicDB& GetDB()  { return m_DB;}

	void	SetCB(PropertyCB* pCB ) { m_pCB = pCB;};

	std::set<DWORD>	GetSelect() const;
	CRectF			GetRect() const;
	GuiItem*		FromPoint(PointF point);
	void			Offset(SizeF size);

	void			SetZoom(const Zoom& zoom);
};

#endif // !defined(AFX_PROJECTDB_H__916DE89F_08BC_4F9C_82EF_0BA181DE4171__INCLUDED_)
