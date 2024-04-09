// ===================================================================================
// 					Class Implementation : COXSplitterRect
// ===================================================================================

// Header file : OXSplitterRect.cpp

// Copyright � Dundas Software Ltd. 1997 - 1998, All Rights Reserved
// Some portions Copyright (C)1994-5	Micro Focus Inc, 2465 East Bayshore Rd, Palo Alto, CA 94303.
                          
// //////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OXSplitterRect.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// Definition of static members

// Data members -------------------------------------------------------------
// protected:

// private:
	
// Member functions ---------------------------------------------------------
// public:

COXSplitterRect::COXSplitterRect(int type, const RECT& rect)
	{
	ASSERT(type == SPLITTER_VERT || type == SPLITTER_HORZ);
    m_rect = rect;
	m_type = (TCHAR)type;
	}; 

void COXSplitterRect::Draw(CDC* pDC)
	{
    CRect rect = m_rect;
    switch (m_type)
		{
        case SPLITTER_VERT:
			rect.left++;
			pDC->FillSolidRect(rect.left, rect.top, 1, rect.Height(), ::GetSysColor(COLOR_BTNFACE));
			rect.left++;
			pDC->FillSolidRect(rect.left, rect.top, 1, rect.Height(), ::GetSysColor(COLOR_BTNHILIGHT));
			
			rect.right--;
			pDC->FillSolidRect(rect.right, rect.top, 1, rect.Height(), ::GetSysColor(COLOR_WINDOWFRAME));
			rect.right--;
			pDC->FillSolidRect(rect.right, rect.top, 1, rect.Height(), ::GetSysColor(COLOR_BTNSHADOW));
			break;
		
        case SPLITTER_HORZ:
            rect.top++;
            pDC->FillSolidRect(rect.left, rect.top, rect.Width(), 1, ::GetSysColor(COLOR_BTNFACE));
            rect.top++;
            pDC->FillSolidRect(rect.left, rect.top, rect.Width(), 1, ::GetSysColor(COLOR_BTNHILIGHT));
			
            rect.bottom--;
            pDC->FillSolidRect(rect.left, rect.bottom, rect.Width(), 1,::GetSysColor(COLOR_WINDOWFRAME));
            rect.bottom--;
            pDC->FillSolidRect(rect.left, rect.bottom, rect.Width(), 1, ::GetSysColor(COLOR_BTNSHADOW));
            break;
			
		default:
            ASSERT(FALSE);
            break;
		}
	}

#ifdef _DEBUG

void COXSplitterRect::Dump( CDumpContext& dc ) const
	{
	dc << "pos = " << m_nPos;
	dc << (m_type == SPLITTER_HORZ ? " Horz" : " Vert");
	}

#endif

COXSplitterRect::~COXSplitterRect()
	{
	}; 
