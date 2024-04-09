//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Database accessors for the "OrwellDB" database
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 13.04.2005
//                                                                                      //
//======================================================================================//
#ifndef _D_B_ACCESSOR_7983257548527842_
#define _D_B_ACCESSOR_7983257548527842_

#include <atldbcli.h>

//======================================================================================//
//                                   class DBAccessor                                   //
//======================================================================================//
struct DBA_Cluster
{
	int		m_nClusterID;
	LPCTSTR	m_sClusterName;
	LPCTSTR	m_sClusterIP;

	BEGIN_ACCESSOR_MAP( DBA_Cluster, 4 )
		BEGIN_ACCESSOR( 0, false)
			COLUMN_ENTRY( 2, m_sClusterName);
			COLUMN_ENTRY( 3, m_sClusterIP);
		END_ACCESSOR()
		BEGIN_ACCESSOR( 1, false )
			COLUMN_ENTRY( 1, m_nClusterID);
		END_ACCESSOR()
		BEGIN_ACCESSOR( 2, false)
			COLUMN_ENTRY( 1, m_nClusterID);
			COLUMN_ENTRY( 2, m_sClusterIP);
		END_ACCESSOR()
		BEGIN_ACCESSOR( 3, false)
			COLUMN_ENTRY( 2, m_sClusterIP);
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Cluster, _T("SELECT ClusterID, ClusterName, ClusterIP FROM Cluster"))
};

struct DBA_Camera
{
	int		m_nCameraID;
	int		m_nClusterID;
	int		m_nCameraType;
	LPCTSTR	m_sComputerIP;

	BEGIN_ACCESSOR_MAP( DBA_Camera, 2 )
		BEGIN_ACCESSOR( 0, false)
			COLUMN_ENTRY( 1, m_nCameraID);
			COLUMN_ENTRY( 2, m_nClusterID);
			COLUMN_ENTRY( 3, m_nCameraType);
			COLUMN_ENTRY( 4, m_sComputerIP);
		END_ACCESSOR()
		BEGIN_ACCESSOR( 1, false)
			COLUMN_ENTRY( 1, m_nClusterID);
			COLUMN_ENTRY( 2, m_nCameraType);
			COLUMN_ENTRY( 3, m_sComputerIP);
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_Camera, _T("SELECT CameraID, ClusterID, CameraType, CameraComputer FROM Camera"))
};

struct DBA_CameraPTZ
{
	int		m_nCameraID;
	int		m_nPresetMax;
	int		m_nTiltMax;
	int		m_nPanMax;
	LPCTSTR	m_sPlugin;
	int		m_nRS485ID;

	BEGIN_ACCESSOR_MAP( DBA_CameraPTZ, 2 )
		BEGIN_ACCESSOR( 0, false)
			COLUMN_ENTRY( 1, m_nCameraID);
			COLUMN_ENTRY( 2, m_nPresetMax);
			COLUMN_ENTRY( 3, m_nTiltMax);
			COLUMN_ENTRY( 4, m_nPanMax);
			COLUMN_ENTRY( 5, m_sPlugin);
			COLUMN_ENTRY( 6, m_nRS485ID);
		END_ACCESSOR()
		BEGIN_ACCESSOR( 1, false)
			COLUMN_ENTRY( 2, m_nPresetMax);
			COLUMN_ENTRY( 3, m_nTiltMax);
			COLUMN_ENTRY( 4, m_nPanMax);
			COLUMN_ENTRY( 5, m_sPlugin);
			COLUMN_ENTRY( 6, m_nRS485ID);
		END_ACCESSOR()
	END_ACCESSOR_MAP()
	DEFINE_COMMAND_EX(DBA_CameraPTZ, _T("SELECT CameraID, PTZ_PresetMax, PTZ_TiltMax, PTZ_PanMax, PTZ_Plugin, PTZ_RS485ID FROM CameraPTZ"))
};

struct DBA_Globals
{
	ISequentialStream*	m_BLOBDATA;

	ULONG m_BLOBDATA_LENGTH;
	ULONG m_BLOBDATA_STATUS;

	BEGIN_COLUMN_MAP(DBA_Globals)
		BLOB_ENTRY_LENGTH_STATUS(1, IID_ISequentialStream, STGM_READ, m_BLOBDATA, m_BLOBDATA_LENGTH, m_BLOBDATA_STATUS)
	END_COLUMN_MAP()
	DEFINE_COMMAND_EX(DBA_Globals, _T("SELECT DEPData FROM Globals"))
};

#endif // _D_B_ACCESSOR_7983257548527842_