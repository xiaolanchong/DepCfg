//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Ôóíêöèÿ äëÿ ñîçäàíèÿ èíòåðôåéñîâ
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.09.2005
//                                                                                      //
//======================================================================================//
#include "stdafx.h"
#include "InterfaceEx.h"

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;


InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const wchar_t *pName ) :
m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

namespace DepCfg
{

bool WINAPI CreateDepInterface( const wchar_t *pName, void **ppInterface )
{
	InterfaceReg *pCur;

	if( !ppInterface/* || !*ppInterface*/ ) return false;

	for(pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
	{
		if(wcscmp(pCur->m_pName, pName) == 0)
		{
			try
			{
				// mb throw exceptions
				*ppInterface = pCur->m_CreateFn();
				return true;
			}
			catch(std::exception&)
			{
				break;
			}
		}
	}

	*ppInterface = NULL;
	return false;	
}

}