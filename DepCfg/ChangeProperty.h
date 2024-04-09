// ChangeProperty.h: interface for the CChangeProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANGEPROPERTY_H__63B87929_7A09_4805_A3E7_624973FA543F__INCLUDED_)
#define AFX_CHANGEPROPERTY_H__63B87929_7A09_4805_A3E7_624973FA543F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Common/Callback.h"
#include "../common/item/LogicDB.h"

class CChangeProperty
{
protected:
	PropertyCB*			m_pPropCB;
	PropertyObserver*	m_pPropObs;

	CChangeProperty( PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 m_pPropCB(pPropCB), m_pPropObs(pPropObs){}
	void SetNumeric( CString Name, Property<DWORD> & Prop);
	void SetBool( CString Name, Property<bool> & Prop);
	void SetBoolNumeric( CString Name, Property<DWORD> & Prop);
	void SetStringList( CString Name, const std::vector<CString> Prop, int Value) ;

	void ChangeNumeric(Property<DWORD> & Prop, CString Value);
	void ChangeBool(Property<bool> & Prop, CString Value);
	void ChangeBoolNumeric(Property<DWORD> & Prop, CString Value);

	void SetNumericRO(ReadOnlyProperty* pROP, CString Name, const Property<DWORD>& Prop);
	void SetBoolRO(ReadOnlyProperty* pROP, CString Name, const Property<bool>& Prop);
	void SetStringRO(ReadOnlyProperty* pROP, CString Name,  const Property<CString>& Prop);
};


#endif // !defined(AFX_CHANGEPROPERTY_H__63B87929_7A09_4805_A3E7_624973FA543F__INCLUDED_)
