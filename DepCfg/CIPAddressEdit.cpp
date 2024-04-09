//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
/* Description: Adapted UT COXMaskEdit class for IP mask

    Author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
    Date   23.11.2005
*/                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "CIPAddressEdit.h"
//======================================================================================//
//                                 class CIPAddressEdit                                 //
//======================================================================================//

CIPAddressEdit::CIPAddressEdit( const RECT& rect, CWnd* pParentWnd, UINT nID):
	COXMaskedEdit( /*_T("###.###.###.###") */) 
{
	Create( WS_CHILD, rect, pParentWnd, nID ); 
	m_chIntlDecimal = _T('.');
	SetMask(_T("###.###.###.###"));
}

CIPAddressEdit::~CIPAddressEdit()
{
}

BOOL CIPAddressEdit::OnValidate()
{
	SetWindowText( _T("1.2.3.4") );
	return TRUE;
}