// CameraProperty.cpp: implementation of the CCameraProperty class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "CameraProperty.h"

#include "../DepCfgLite/interface/DepInterface.h"
#include "../DepCfgLite/interface/IResourceLoader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MACRO_EXCEPTION( PropResolveException, CommonException)

static CString		LoadString( UINT ResID )
{
	boost::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader >(
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);
	return p->LoadStringInt(ResID);
}


struct PTZNumber
{
	std::vector<UINT>	m_ResID;

	PTZNumber()
	{
		m_ResID.reserve( 8 );
		UINT ResID []= 
		{
			IDS_PROP_PTZ_DEVICE_0,
			IDS_PROP_PTZ_DEVICE_1,
			IDS_PROP_PTZ_DEVICE_2,
			IDS_PROP_PTZ_DEVICE_3
		};

		m_ResID.insert( m_ResID.end(), ResID, ResID + sizeof(ResID)/sizeof(UINT) );
	}
	UINT	ToNumber( CString strPKT ) const throw (PropResolveException)
	{
		for(size_t i=0; i < m_ResID.size() ; ++i)
		{
			if( LoadString( m_ResID[i] ) == strPKT ) return i;
		}
		throw PropResolveException("No such PKT property");
		return 0;
	}
	CString	ToString( DWORD dwNumber)
	{
		if(dwNumber >= m_ResID.size()) throw PropResolveException("No such PTZ number");
		return LoadString( m_ResID[dwNumber] );
	}
	std::vector<CString>	LoadPTZDevices() const
	{
		std::vector<CString> z;
		z.reserve( m_ResID.size() );
		for(size_t i=0; i < m_ResID.size() ; ++i)
		{
			z.push_back( LoadString( m_ResID[i] ) );
		}
		return z;
	}
};

//NOTENOTE: they are in resource now
const UINT	prop_camera_id			= IDS_PROP_CAMERA_ID;
const UINT	prop_capture_pin		= IDS_PROP_CAPTURE_PIN;
const UINT  prop_capture_bw			= IDS_PROP_CAPTURE_BW; 		

const UINT  prop_enable_ad			= IDS_PROP_ENABLE_ARCHIVE_DATA;
const UINT  prop_enable_dw			= IDS_PROP_ENABLE_DATAWARE;

const UINT  prop_ptzdevice			= IDS_PROP_PTZ_DEVICE;
const UINT  prop_rs485id			= IDS_PROP_RS485_ID;

const UINT  prop_com_name			= IDS_PROP_COM_NAME;
const UINT  prop_com_delay			= IDS_PROP_COM_DELAY;
const UINT  prop_com_md				= IDS_PROP_COM_MASTER_DELAY;
const UINT  prop_com_emptymd		= IDS_PROP_COM_EMPTY_MASTER_DELAY;
const UINT  prop_com_predictiontime	= IDS_PROP_COM_PREDICTION_TIME;
const UINT  prop_com_ip				= IDS_PROP_COM_IP;
//////////////////////////////////////

void	CCameraProperty::SetProperties( CameraSettings& cs) 
{
	SetNumeric(		LoadString(prop_camera_id), cs.m_dwCameraID);
#ifdef USE_OLD_FORMAT
	SetNumeric(		LoadString(prop_capture_pin), cs.m_dwCapturePin);
	SetBoolNumeric( LoadString(prop_capture_bw),  cs.m_dwCaptureBW);
#endif
}

void CCameraProperty::OnPropertyChange( CameraSettings& cs, CString Name, CString Value) 
{
	if( Name ==		LoadString(prop_camera_id) )
	{		
		ChangeNumeric(cs.m_dwCameraID, Value);
	}
#ifdef USE_OLD_FORMAT
	else if(Name == LoadString(prop_capture_pin) )
	{
		ChangeNumeric(cs.m_dwCapturePin, Value);
	}
	else if (Name == LoadString(prop_capture_bw) ) 
	{
		ChangeNumeric(cs.m_dwCaptureBW, Value);
	}
#endif
}

void CCameraProperty::SetReadOnlyProperties(const CameraSettings& cs, ReadOnlyProperty* pROP)
{
	SetNumericRO(pROP, LoadString(prop_camera_id), cs.m_dwCameraID);
#ifdef USE_OLD_FORMAT
	SetNumericRO(pROP, LoadString(prop_capture_pin), cs.m_dwCapturePin);
	SetNumericRO(pROP, LoadString( prop_capture_bw),  cs.m_dwCaptureBW);	
#endif
}

///////////////////////////////////////////////////////////////////

void	CMasterProperty::SetProperties(MasterSettings& cs) 
{
	CCameraProperty::SetProperties( cs) ;
#ifdef USE_OLD_FORMAT
	SetBool( LoadString(prop_enable_ad), cs.m_EnableArchive );
	SetBool( LoadString(prop_enable_dw), cs.m_EnableDataWare );
#endif
	CString Caption;
	if(cs.m_dwCameraID.IsSet())
	{
		CString strFormat = LoadString(IDS_CAPTION_MASTER_FORMAT);
		Caption.Format(/*_T("Master camera %d")*/ (LPCTSTR)strFormat, cs.m_dwCameraID.Get());
	}
	m_pPropCB->SetHeader( Caption );
}

void CMasterProperty::OnPropertyChange( MasterSettings& cs, CString Name, CString Value) 
{
	CCameraProperty::OnPropertyChange( cs, Name, Value) ;
#ifdef USE_OLD_FORMAT
	if( Name == LoadString(prop_enable_ad))
	{		
		ChangeBool(cs.m_EnableArchive, Value);
	}
	else if(Name == LoadString(prop_enable_dw) )
	{
		ChangeBool(cs.m_EnableDataWare, Value);
	}
#endif
}

void CMasterProperty::SetReadOnlyProperties(const MasterSettings& cs, ReadOnlyProperty* pROP)
{
	CCameraProperty::SetReadOnlyProperties( cs, pROP );
#ifdef USE_OLD_FORMAT
	SetBoolRO( pROP, LoadString(prop_enable_ad), cs.m_EnableArchive );
	SetBoolRO( pROP, LoadString(prop_enable_dw), cs.m_EnableDataWare );		
#endif
}

////////////////////////////////////////////////////

void	CSlaveProperty::SetProperties(  SlaveSettings& cs )
{
	CCameraProperty::SetProperties(  cs) ;
	PTZNumber z;
	std::vector< CString > arrPTZ = z.LoadPTZDevices();
	int DefValue = cs.m_dwPTZDevice.IsSet() ? cs.m_dwPTZDevice.Get() : -1;
	SetStringList( LoadString(prop_ptzdevice), arrPTZ, DefValue);

	SetNumeric( LoadString(prop_rs485id), cs.m_dwRS485ID );	

}

void CSlaveProperty::OnPropertyChange( SlaveSettings& cs, CString Name, CString Value) 
{
	CCameraProperty::OnPropertyChange( cs, Name,  Value) ;
	if( Name ==		LoadString(prop_ptzdevice) )
	{
		PTZNumber z;
		UINT Id = z.ToNumber( Value );
		//NOTENOTE: for compatible we convert ot string and 
		CString s; s.Format( _T("%d") , Id);
		ChangeNumeric(cs.m_dwPTZDevice, /*Value*/s);
	}
	else if(Name == LoadString(prop_rs485id) )
	{
		ChangeNumeric(cs.m_dwRS485ID, Value);
	}	
}

void CSlaveProperty::SetReadOnlyProperties(const SlaveSettings& cs, ReadOnlyProperty* pROP)
{
	CCameraProperty::SetReadOnlyProperties(  cs, pROP) ;
#if 1
	CString sDevice;
	try
	{
		PTZNumber p;
		sDevice = p.ToString( cs.m_dwPTZDevice.Get() );
	}
	catch( CommonException)
	{
		sDevice.Empty();
	}
	SetStringRO( pROP, LoadString(prop_ptzdevice), Property<CString>(sDevice) );
#else
	SetNumericRO( pROP, LoadString(prop_ptzdevice), cs.m_dwPTZDevice );
#endif
	SetNumericRO( pROP, LoadString(prop_rs485id), cs.m_dwRS485ID );
}

/////////////////////////////////////////////////////////

void	CCommutatorProperty::SetProperties( CommutatorSettings& cs ) 
{
	CString Name = cs.m_strName.IsSet() ? cs.m_strName.Get() : CString();
	m_pPropCB->SetEditProperty( LoadString(prop_com_name), Name, m_pPropObs);

	std::vector<CString> v = m_pDB->GetAllComputers();

	v.insert( v.begin(), _T("") );
	int nDefValue = -1;
#ifdef USE_AUTO_COMP2COMM
	CString sEmpty;
	sEmpty.LoadString( IDS_EMPTY );
	std::pair<bool, CString> p = m_pDB->GetIPForCommutator( );
	CString sIPValue =  p.first? p.second : sEmpty;
	m_pPropCB->SetStatic( LoadString(prop_com_ip), sIPValue );
#else
	CString CompName = m_pDB->GetComp2Comm( m_dwID );

	std::vector<CString>::const_iterator itr = std::find( v.begin(), v.end(), CompName );

	if( itr != v.end() ) nDefValue = itr - v.begin();

	m_pPropCB->SetComboProperty( LoadString(prop_com_ip), v, nDefValue, m_pPropObs );
#endif
#ifdef USE_OLD_FORMAT
	SetNumeric( LoadString(prop_com_delay), cs.m_dwDelay);
	SetNumeric( LoadString(prop_com_md), cs.m_dwMasterDelay);
	SetNumeric( LoadString(prop_com_emptymd), cs.m_dwEmptyMasterDelay);
	SetNumeric( LoadString(prop_com_predictiontime), cs.m_dwPredictionTime);
#endif
//	m_pPropCB->SetStatic( CString(_T("Static value")), CString(_T("blah-blah")) );
}

void CCommutatorProperty::SetReadOnlyProperties(const CommutatorSettings& cs, ReadOnlyProperty* pROP)
{
	SetStringRO(pROP, LoadString(prop_com_name), cs.m_strName);
	CString Value;
#ifdef USE_AUTO_COMP2COMM
	CString sEmpty = LoadString(IDS_EMPTY);
	std::pair<bool, CString> p = m_pDB->GetIPForCommutator( );
	Value =  p.first? p.second : sEmpty;
#else
	std::vector<CString> v = m_pDB->GetAllComputers();
	CString CompName = m_pDB->GetComp2Comm( m_dwID );
	std::vector<CString>::const_iterator itr = std::find( v.begin(), v.end(), CompName );
	if( itr != v.end() ) Value = *itr;
#endif
	pROP->SetReadOnlyProperty( LoadString(prop_com_ip), Value);
#ifdef USE_OLD_FORMAT
	SetNumericRO(pROP, LoadString(prop_com_delay), cs.m_dwDelay);
	SetNumericRO(pROP, LoadString(prop_com_md), cs.m_dwMasterDelay);
	SetNumericRO(pROP, LoadString(prop_com_emptymd), cs.m_dwEmptyMasterDelay);
	SetNumericRO(pROP, LoadString(prop_com_predictiontime), cs.m_dwPredictionTime);	
#endif
}

void CCommutatorProperty::OnPropertyChange( CommutatorSettings& cs, CString Name, CString Value) 
{
	if( Name == LoadString(prop_com_name))
	{		
		cs.m_strName =  Value;
	}
#ifdef USE_OLD_FORMAT
	else if(Name == LoadString(prop_com_delay) )
	{
		ChangeNumeric(cs.m_dwDelay, Value);
	}
	else if (Name == LoadString(prop_com_md)) 
	{
		ChangeNumeric(cs.m_dwMasterDelay, Value);
	}
	else if(Name == LoadString(prop_com_emptymd))
	{
		ChangeNumeric(cs.m_dwEmptyMasterDelay, Value);
	}
	else if(Name == LoadString(prop_com_predictiontime))
	{
		ChangeNumeric(cs.m_dwPredictionTime, Value);
	}
#endif
#ifndef USE_AUTO_COMP2COMM
	else if(Name == LoadString(prop_com_ip))
	{
		m_pDB->AddComp2Comm( m_dwID, Value );
	}
#endif
}