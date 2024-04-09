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
#ifndef _C_I_P_ADDRESS_EDIT_6779372881941294_
#define _C_I_P_ADDRESS_EDIT_6779372881941294_

#include "UT/OXMaskedEdit.h"

//======================================================================================//
//                                 class CIPAddressEdit                                 //
//======================================================================================//

//! \brief Adapted UT COXMaskEdit class for IP mask
//! \author Eugene Gorbachev (Eugene.Gorbachev@biones.com)
//! \date   23.11.2005
//! \version 1.0
//! \bug 
//! \todo 

class CIPAddressEdit : public COXMaskedEdit
{
public:
	CIPAddressEdit(const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual ~CIPAddressEdit();

	virtual BOOL OnValidate();
};

#endif // _C_I_P_ADDRESS_EDIT_6779372881941294_