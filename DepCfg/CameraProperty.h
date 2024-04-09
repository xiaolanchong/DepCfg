// CameraProperty.h: interface for the CCameraProperty class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAPROPERTY_H__9B6153D6_2AE5_4B45_B86C_3FEB5BB829F3__INCLUDED_)
#define AFX_CAMERAPROPERTY_H__9B6153D6_2AE5_4B45_B86C_3FEB5BB829F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ChangeProperty.h"

class CCameraProperty  : protected CChangeProperty
{
protected:
	CCameraProperty( PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 CChangeProperty(pPropCB,pPropObs){}

	void SetProperties(   CameraSettings& cs) ;
	void OnPropertyChange( CameraSettings& cs, CString Name, CString Value) ;
	void SetReadOnlyProperties(const CameraSettings& cs, ReadOnlyProperty* pROP);
};

class CSlaveProperty  : protected CCameraProperty
{
public:
	CSlaveProperty( PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 CCameraProperty(pPropCB,pPropObs){}

	void SetProperties(  SlaveSettings& cs ) ;
	void OnPropertyChange( SlaveSettings& cs, CString Name, CString Value) ;
	void SetReadOnlyProperties(const SlaveSettings& cs, ReadOnlyProperty* pROP);
};

class CMasterProperty  : protected CCameraProperty
{
public:
	CMasterProperty( PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 CCameraProperty(pPropCB,pPropObs){}
	void SetProperties( MasterSettings& cs ) ;
	void OnPropertyChange( MasterSettings& cs, CString Name, CString Value) ;
	void SetReadOnlyProperties(const MasterSettings& cs, ReadOnlyProperty* pROP);
};

class CCommutatorProperty  : protected CChangeProperty
{
	CLogicDB*		m_pDB;
	DWORD			m_dwID;
public:
	CCommutatorProperty( CLogicDB* pDB, DWORD dwID , PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 CChangeProperty(pPropCB,pPropObs), m_pDB(pDB), m_dwID(dwID){}
	void SetProperties( CommutatorSettings& cs ) ;
	void OnPropertyChange( CommutatorSettings& cs, CString Name, CString Value) ;
	void SetReadOnlyProperties(const CommutatorSettings& cs, ReadOnlyProperty* pROP);
};
/*
class CComputerProperty  : protected CChangeProperty
{
public:
	CComputerProperty( PropertyCB* pPropCB, PropertyObserver* pPropObs ):
		 CCameraProperty(pPropCB,pPropObs){}
	void SetProperties( MasterSettings& cs ) ;
	void OnPropertyChange( MasterSettings& cs, CString Name, CString Value) ;
};
*/
#endif // !defined(AFX_CAMERAPROPERTY_H__9B6153D6_2AE5_4B45_B86C_3FEB5BB829F3__INCLUDED_)
