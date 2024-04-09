// ChangeProperty.cpp: implementation of the CChangeProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "depcfg.h"
#include "ChangeProperty.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CChangeProperty::SetNumeric( CString Name, Property<DWORD> & Prop)
{
	CString Value;
	if( Prop.IsSet())
		Value.Format(_T("%d"), Prop.Get());
	else
		Value.Empty();
	m_pPropCB->SetNumEditProperty( Name, Value, m_pPropObs );
}

void CChangeProperty::SetBool( CString Name, Property<bool> & Prop)
{
	int DefValue = -1;
	if( Prop.IsSet())
	{
		DefValue = int(Prop.Get());
	}

	m_pPropCB->SetBoolProperty( Name, DefValue, m_pPropObs );
}

void CChangeProperty::SetBoolNumeric( CString Name, Property<DWORD> & Prop )
{
	std::vector<CString> Values;
	Values.push_back(CString());
	Values.push_back(CString("0"));
	Values.push_back(CString("1"));

	int DefValue = 0;
	if( Prop.IsSet())
	{
		DefValue = Prop.Get() + 1;
	}
	m_pPropCB->SetComboProperty( Name, Values, DefValue, m_pPropObs );
}


void CChangeProperty:: SetStringList( CString Name, const std::vector<CString> Prop, int Value) 
{
	m_pPropCB->SetComboProperty( Name, Prop, Value, m_pPropObs );
}

void CChangeProperty::ChangeNumeric(Property<DWORD> & Prop, CString Value)
{
	if( Value.IsEmpty() )
	{
		Prop.Reset();
	}
	else
	{
		DWORD dw =  _ttoi( Value );
		Prop = dw;
	}
}

void CChangeProperty::ChangeBool(Property<bool> & Prop, CString Value)
{
	if( Value.IsEmpty() )
	{
		Prop.Reset();
	}
	else
	{
		Prop = ( Value == _T("true") ) ? true : false ;
	}
}

void CChangeProperty::ChangeBoolNumeric(Property<DWORD> & Prop, CString Value)
{

}

void CChangeProperty::SetNumericRO(ReadOnlyProperty* pROP, CString Name,const  Property<DWORD>& Prop)
{
	CString Value;
	if( Prop.IsSet())
		Value.Format(_T("%d"), Prop.Get());
	else
		Value.Empty();
	pROP->SetReadOnlyProperty( Name, Value );	
}

void CChangeProperty::SetBoolRO(ReadOnlyProperty* pROP, CString Name, const Property<bool>& Prop)
{
	CString Value;
	if( Prop.IsSet())
		Value = Prop.Get()? _T("true") : _T("false");
	else
		Value.Empty();
	pROP->SetReadOnlyProperty( Name, Value );	
}

void CChangeProperty::SetStringRO(ReadOnlyProperty* pROP, CString Name, const Property<CString>& Prop)
{
	CString Value;
	if( Prop.IsSet())
		Value = Prop.Get();
	else
		Value.Empty();
	pROP->SetReadOnlyProperty( Name, Value );	
}