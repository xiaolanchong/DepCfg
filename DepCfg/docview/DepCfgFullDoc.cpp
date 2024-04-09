//========================= Copyright © 2004, Elvees ==========================
// Author:	Eugene V. Gorbachev
// Purpose: implementation of the CDepCfgFullDoc class
// Revision:12-09-2004 added database validation - empty parameters
//
//=============================================================================


#include "stdafx.h"
#include "DepCfgFullDoc.h"

#include "../../Common/utility/iniFile.h"

#include "..\..\common\item\GuiComputer.h"
#include "..\..\common\item\GuiCamera.h"
#include "..\..\common\item\GuiMaster.h"
#include "..\..\common\item\GuiCommutator.h"
#include "..\..\common\item\GuiConnection.h"

#include "..\..\common\utility\Helper.h"

#include "MainFrm.h"
#include "SettingsDlg.h"

#include "..\..\common\docview\EditorView.h"

#include "..\..\DepCfgLite\interface\DepInterface.h"
#include "..\..\DepCfgLite\interface\IDBSerializer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using Helper::ConvertA;
using Helper::ConvertW;

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc

IMPLEMENT_DYNCREATE(CDepCfgFullDoc, CDepCfgDoc)

BEGIN_MESSAGE_MAP(CDepCfgFullDoc, CDepCfgDoc)
	//{{AFX_MSG_MAP(CDepCfgFullDoc)
	ON_COMMAND(ID_ITEM_COMMUTATOR, OnItemCommutator)
	ON_COMMAND(ID_ITEM_COMPUTER, OnItemComputer)
	ON_COMMAND(ID_ITEM_MASTER, OnItemMaster)
	ON_COMMAND(ID_ITEM_SLAVE, OnItemSlave)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_COMMAND(ID_FILE_IMPORT, OnFileImport)

	ON_COMMAND( ID_FILE_DB_SAVE, OnFileDBSave)
	ON_COMMAND( ID_FILE_DB_LOAD, OnFileDBLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc construction/destruction

CDepCfgFullDoc::CDepCfgFullDoc()
{
	// TODO: add one-time construction code here
}

CDepCfgFullDoc::~CDepCfgFullDoc()
{
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc serialization

void CDepCfgFullDoc::Serialize(CArchive& ar)
{
//	CFileDialog dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);
//	dlgFile.DoModal();
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		std::ostringstream os;
		Save(os);
		std::string s = os.str();
		ar.Write( s.c_str(), s.length() );
	}
	else
	{
		// TODO: add loading code here
		CFile *pFile = ar.GetFile();
		size_t size = pFile->GetLength();
		std::vector<char> v;
		v.resize( size +1 );
		
		pFile->Read( &v[0], size );
		v.push_back('\0');
		try
		{
			Load(v);
		}
		catch(XMLVersionDBException)
		{
			AfxMessageBox( IDS_XML_VERSION_ERROR, MB_OK|MB_ICONERROR );
			AfxThrowUserException();
		}
		catch(XMLException)
		{
			AfxMessageBox( IDS_XML_ERROR, MB_OK|MB_ICONERROR );
			AfxThrowUserException();
		}
	}
}

void CDepCfgFullDoc::DeleteContents()
{
	CDepCfgDoc::DeleteContents();
	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd) static_cast<CMainFrame*>( pWnd )->ResetProperty();
}

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc diagnostics

#ifdef _DEBUG
void CDepCfgFullDoc::AssertValid() const
{
	CDepCfgDoc::AssertValid();
}

void CDepCfgFullDoc::Dump(CDumpContext& dc) const
{
	CDepCfgDoc::Dump(dc);
}
#endif //_DEBUG

void CDepCfgFullDoc::OnItemCommutator() 
{
	// TODO: Add your command handler code here
	CommutatorSettings cs;

	size_t Num =  m_DB.m_Commutators.size() + 1;
	CString Name;
	Name.Format( _T("Cl%u")/*IDS_FMT_COMMUTATOR_NAME*/, Num );
#ifdef USE_OLD_FORMAT	
	cs.m_dwDelay			= 0;
	cs.m_dwEmptyMasterDelay = 0;
	cs.m_dwMasterDelay		= 0;
	cs.m_dwPredictionTime	= 0;
#endif
	cs.m_strName			= Name;

	m_DB.AddCommutator( cs );

	SizeF sizeScroll = GetLogicPos();

	GuiItem* pItem = new GuiCommutator(m_DB, cs.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_LogicDB.AddItem( pItem) ;
	SetModifiedFlag();
	UpdateAllViews(0);
}

void CDepCfgFullDoc::OnItemComputer() 
{
	// TODO: Add your command handler code here
	ComputerSettings cs;
	m_DB.AddComputer( cs);

	SizeF sizeScroll = GetPhysicPos();

	GuiItem* pItem = new GuiComputer(m_DB, cs.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_PhysicDB.AddItem( pItem) ;
	
	SetModifiedFlag();
	UpdateAllViews(0);	
}

void CDepCfgFullDoc::OnItemMaster() 
{
	// TODO: Add your command handler code here
	MasterSettings ms ;
	ms.m_dwCameraID		= m_DB.GetCameraID();
#ifdef USE_OLD_FORMAT
	ms.m_dwCapturePin	= 0;
	ms.m_dwCaptureBW	= 0;
	ms.m_EnableArchive	= true;
	ms.m_EnableDataWare	= true;
#endif

	m_DB.AddMaster( ms );
	
	SizeF sizeScroll = GetPhysicPos();

	GuiItem* pItem = new GuiPhysicMaster(m_DB, ms.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_PhysicDB.AddItem( pItem) ;
	
	sizeScroll = GetLogicPos();

	pItem = new GuiLogicMaster(m_DB, ms.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_LogicDB.AddItem( pItem) ;

	SetModifiedFlag();
	UpdateAllViews(0);
}

void CDepCfgFullDoc::OnItemSlave() 
{
	// TODO: Add your command handler code here
	SlaveSettings ss ;
	ss.m_dwCameraID = m_DB.GetCameraID();
#ifdef USE_OLD_FORMAT
	ss.m_dwCapturePin	= 0;
	ss.m_dwCaptureBW	= 0;
#endif
	ss.m_dwPTZDevice	= 1;
	m_DB.AddSlave( ss );
	SizeF sizeScroll = GetPhysicPos();
	GuiItem* pItem = new GuiPhysicSlave(m_DB, ss.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_PhysicDB.AddItem( pItem) ;

	sizeScroll = GetLogicPos();
	pItem = new GuiLogicSlave(m_DB, ss.GetID(), RectF(10 + sizeScroll.Width, 10 + sizeScroll.Height, 80, 100)); 
	m_LogicDB.AddItem( pItem) ;
	
	SetModifiedFlag();
	UpdateAllViews(0);		
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CString Convert2XMLString(CString str)
{
	CString s(str)	;
	s.Replace( _T("&"), _T("&amp;") );
	s.Replace( _T("<"), _T("&lt;") );
	s.Replace( _T(">"), _T("&gt;") );
	s.Replace( _T("\""), _T("&quot;") );
	s.Replace( _T("\'"), _T("&apos;") );
	return s;
}

CString GetAttr(CString Name, CString Value)
{
	return Name + _T("=\"") + Convert2XMLString(Value) + _T("\"");
}

CString GetAttr(CString Name, DWORD Value)
{
	CString s;
	s.Format( _T("%d"), Value );
	return GetAttr(Name,s);
}

template <class T>
CString GetAttr(CString Name, const Property<T>& p)
{
	if ( p.IsSet() ) return GetAttr(Name, p.Get() );
	else return GetAttr(Name,"");
}

CString GetVal(const Property<CString>& p)
{
	if ( p.IsSet() ) return Convert2XMLString(p.Get()) ;
	else return CString();
}

/*	

Versions

1.0	- default
1.1 - Virtual
1.2 - 
1.3 - "DNS name" Computer parameter
*/

static LPCSTR szXMLVersionA = "1.4";
static LPCWSTR szXMLVersionW = L"1.4";

void	CDepCfgFullDoc::Save(std::ostream& os)	const
{ 
	os << "<?xml version=\"1.0\"?>\n";// encoding=\"UTF-16\"?>\n";
	os << "<base version=\""<< szXMLVersionA <<"\">\n";

	
	os << "<data " << ConvertA(GetAttr("Counter", DbItem::GetCounter())) << ">" << std::endl;
#ifdef USE_OLD_FORMAT
	SaveProject(os);
#endif
	SaveComputer(os);
	SaveCommutator(os);
	SaveMaster(os);
	SaveSlave(os);
	SaveConnection(os);
#ifndef USE_AUTO_COMP2COMM
	SaveComp2Comm(os);
#endif
	os << "</data>\n";

	os << "<physic>\n";
	SaveGuiBase(os, m_PhysicDB);
	os << "</physic>\n";

	os << "<logic>\n";
	SaveGuiBase(os, m_LogicDB);
	os << "</logic>\n";

	os << "</base>";
}




void	CDepCfgFullDoc::SaveComputer(std::ostream& os) const
{
	os << "<computers>" << std::endl ;
	std::set<ComputerSettings>::const_iterator itr = m_DB.m_Computers.begin();
	for(; itr != m_DB.m_Computers.end() ; ++itr)
	{
		os	<< "<computer "
			<< ConvertA(GetAttr("ID", itr->GetID())) << " "
			<< ConvertA((GetAttr("IP", itr->m_strIP))) << " "
			<< ConvertA((GetAttr("DNSName", itr->m_strDnsName))) << " "
			<< "/>" << std::endl ;
	}
	os << "</computers>" << std::endl ;
}

void	CDepCfgFullDoc::SaveSlave(std::ostream& os) const
{
	os << "<slaves>" << std::endl ;
	std::set<SlaveSettings>::const_iterator itr = m_DB.m_Slaves.begin();
	for(; itr != m_DB.m_Slaves.end() ; ++itr)
	{
		os	<< "<slave "
			<< ConvertA(GetAttr("ID", itr->GetID())) << " "
			<< ConvertA(GetAttr("CameraID", itr->m_dwCameraID)) << " "
#ifdef USE_OLD_FORMAT
			<< ConvertA(GetAttr("CapturePin", itr->m_dwCapturePin)) << " "
			<< ConvertA(GetAttr("CaptureBW", itr->m_dwCaptureBW)) << " "
#endif
			<< ConvertA(GetAttr("PTZDevice", itr->m_dwPTZDevice)) << " "
			<< ConvertA(GetAttr("RS485ID", itr->m_dwRS485ID)) << " "
			<< "/>" << std::endl ;
	}
	os << "</slaves>" << std::endl ;

}

void	CDepCfgFullDoc::SaveMaster(std::ostream& os) const
{
	os << "<masters>" << std::endl ;
	std::set<MasterSettings>::const_iterator itr = m_DB.m_Masters.begin();
	for(; itr != m_DB.m_Masters.end() ; ++itr)
	{
		os	<< "<master "
			<< ConvertA(GetAttr("ID", itr->GetID())) << " "
			<< ConvertA(GetAttr("CameraID", itr->m_dwCameraID)) << " "
#ifdef USE_OLD_FORMAT
			<< ConvertA(GetAttr("CapturePin", itr->m_dwCapturePin)) << " "
			<< ConvertA(GetAttr("CaptureBW", itr->m_dwCaptureBW)) << " "
			<< ConvertA(GetAttr("EnableArchive", itr->m_EnableArchive)) << " "
			<< ConvertA(GetAttr("EnableDataWare", itr->m_EnableDataWare)) << " "
#endif
			<< "/>" << std::endl ;
	}
	os << "</masters>" << std::endl ;
}

void	CDepCfgFullDoc::SaveConnection(std::ostream& os) const
{
	os << "<connections>" << std::endl ;
	std::set<Connection>::const_iterator itr = m_DB.m_Connections.begin();
	for(; itr != m_DB.m_Connections.end() ; ++itr)
	{
		os	<< "<connection "
			<< ConvertA(GetAttr("ID", itr->GetID())) << " "
			<< ConvertA(GetAttr("First", itr->First)) << " "
			<< ConvertA(GetAttr("Second", itr->Second)) << " "
			<< ConvertA(GetAttr("FirstSlot", itr->FirstSlot)) << " "
			<< ConvertA(GetAttr("SecondSlot", itr->SecondSlot)) << " "
			<< "/>" << std::endl ;
	}
	os << "</connections>" << std::endl ;
}

void	CDepCfgFullDoc::SaveCommutator(std::ostream& os) const
{
	os << "<commutators>" << std::endl ;
	std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.begin();
	for(; itr != m_DB.m_Commutators.end() ; ++itr)
	{
		os	<< "<commutator "
			<< ConvertA(GetAttr("ID", itr->GetID())) << " "
			<< ConvertA(GetAttr("Name", itr->m_strName)) << " "
#ifdef USE_OLD_FORMAT
			<< ConvertA(GetAttr("Delay", itr->m_dwDelay)) << " "
			<< ConvertA(GetAttr("MasterDelay", itr->m_dwMasterDelay)) << " "
			<< ConvertA(GetAttr("EmptyMasterDelay", itr->m_dwEmptyMasterDelay)) << " "
			<< ConvertA(GetAttr("PredictionTime", itr->m_dwPredictionTime)) << " "
#endif
			<< "/>" << std::endl ;
	}
	os << "</commutators>" << std::endl ;
}
#ifndef USE_AUTO_COMP2COMM
void	CDepCfgFullDoc::SaveComp2Comm(std::ostream& os) const
{
	os << "<comp2comms>" << std::endl ;
	std::set<Comp2Comm>::const_iterator itr = m_DB.m_Comp2Comm.begin();
	for(; itr != m_DB.m_Comp2Comm.end() ; ++itr)
	{
		os	<< "<comp2comm "
			<< ConvertA(GetAttr("Comp", itr->m_dwComp)) << " "
			<< ConvertA(GetAttr("Comm", itr->m_dwComm)) << " "
			<< "/>" << std::endl ;
	}
	os << "</comp2comms>" << std::endl ;
}
#endif

void	DumpRect(RectF rc, std::ostream& os)
{

		os		<< "<rect "
				<< ConvertA(GetAttr("X", rc.GetLeft())) << " "
				<< ConvertA(GetAttr("Y", rc.GetTop())) << " "
				<< ConvertA(GetAttr("Width", rc.Width)) << " "
				<< ConvertA(GetAttr("Height", rc.Height)) << " "
				<< "/>";
}

void	DumpItem( std::ostream& os, CString Name, GuiRect* pItem )
{
	RectF rc = pItem->GetRect();
			os	<< "<" << ConvertA(Name)<< " "
				<< ConvertA(GetAttr("ID", pItem->GetID())) << " >\n";
			DumpRect(rc, os);
			os << std::endl;
			os	<< "</" <<  ConvertA(Name) << ">\n";
}

//void	DumpConnection()

void	CDepCfgFullDoc::SaveGuiBase(std::ostream& os, const CGuiDB &db) const
{
	CGuiDB::DBList_t::const_iterator itr = db.m_GuiDB.begin();
//	GuiRect*		pRect;
	GuiConnection*	pConnection;
	GuiComputer*	pComputer;
	GuiMaster*		pMaster;
	GuiSlave*		pSlave;
	GuiCommutator*	pCommutator;
	for(; itr != db.m_GuiDB.end(); ++itr)
	{
		
		if( pComputer = dynamic_cast<GuiComputer*>( itr->get() ) )
		{
			DumpItem(os, "computer", pComputer);
		}
		else if( pMaster = dynamic_cast<GuiMaster*>( itr->get() ) )
		{
			DumpItem(os, "master", pMaster);
		}
		else if( pSlave = dynamic_cast<GuiSlave*>( itr->get() ) )
		{
			DumpItem(os, "slave", pSlave);
		}
		else if( pCommutator = dynamic_cast<GuiCommutator*>( itr->get() ) )
		{
			DumpItem(os, "commutator", pCommutator);
		}
		else if( pConnection = dynamic_cast<GuiConnection*>( itr->get() ) )
		{
			os		<< "<connection "
					<< ConvertA(GetAttr("ID", pConnection->GetID())) << " "
					<< "/>" << std::endl;
		}
	}
}

#ifdef USE_OLD_FORMAT
void	CDepCfgFullDoc::SaveProject(std::ostream& os) const
{
	os << "<project>" << std::endl;
	os << "<server>";
	os << ConvertA(GetVal(m_DB.m_Project.m_strSQLServer)) ;
	os << "</server>"  << std::endl;
	os << "<database>"  ;
	os << ConvertA(GetVal( m_DB.m_Project.m_strDBName ));
	os << "</database>"  << std::endl;
	os << "<shareddir>"  ;
	os << ConvertA(GetVal(m_DB.m_Project.m_strSharedDir ));
	os << "</shareddir>"  << std::endl;
	os << "<video_standard>" << m_DB.m_Project.m_dwVideoStandard.Get() 
		<< "</video_standard>" << std::endl;
	os << "</project>" << std::endl;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDepCfgFullDoc::Verify()
{
	// TODO: Add your command handler code here
	VerifyComputer();
#ifdef USE_OLD_FORMAT
	VerifyProject();
#endif
	VerifyCamera();
	VerifyCommutator();

	CString rep = m_Report.GetReport();
	if(!rep.IsEmpty())
	{
		CString strCaption;
		strCaption.LoadString( IDS_ERR_EXPORT_CAPTION );
		MessageBox( AfxGetMainWnd()->GetSafeHwnd(), rep, strCaption, MB_OK|MB_ICONERROR );
		//NOTENOTE: 10/13/2004 bug - report didn't clear
		m_Report.Clear();
		return false;
	}
	return true;
}

void CDepCfgFullDoc::OnFileExport() 
{
#ifdef USE_OLD_FORMAT
	if( !Verify() ) return;

	LPCTSTR szFilter = _T("Ini file(*.ini)|*.ini||");
	CFileDialog dlg( FALSE, _T("ini"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, AfxGetMainWnd() );
	if(dlg.DoModal() == IDOK)
	{
		CIniFile iniFile;
		m_Report.Clear();
#ifdef USE_OLD_FORMAT		
		ExportProject(iniFile);
#endif
		ExportCommutator(iniFile);
		ExportMaster(iniFile);
		ExportSlave(iniFile);

		iniFile.Path( ConvertA(dlg.GetPathName()));
		iniFile.WriteFile();
	}
#endif
}

void CDepCfgFullDoc::OnSettings() 
{
	// TODO: Add your command handler code here
#ifdef USE_OLD_FORMAT
	CSettingsDlg dlg (
		AfxGetMainWnd(),
		m_DB.m_Project.m_strSQLServer.IsSet() ? m_DB.m_Project.m_strSQLServer.Get() : CString(), 
		m_DB.m_Project.m_strDBName.IsSet() ? m_DB.m_Project.m_strDBName.Get() : CString(), 
		m_DB.m_Project.m_strSharedDir.IsSet() ? m_DB.m_Project.m_strSharedDir.Get() : CString(),
		m_DB.m_Project.m_dwVideoStandard.IsSet() ? m_DB.m_Project.m_dwVideoStandard.Get() : 
			CLogicDB::VIDEO_STANDARD_1 
		);
	
	if(  dlg.DoModal() == IDOK)
	{
		m_DB.m_Project.m_strSQLServer		= dlg.m_strServer;
		m_DB.m_Project.m_strDBName			= dlg.m_strDBName;
		m_DB.m_Project.m_strSharedDir		= dlg.m_strSharedDir;
		m_DB.m_Project.m_dwVideoStandard	= dlg.m_dwVideoStandard;
	}
#endif
}

#ifdef USE_OLD_FORMAT
void	CDepCfgFullDoc::ExportProject(CIniFile& iniFile) const
{
	iniFile.SetValue(std::string("Main"), std::string("SQLServerName"), ConvertA(GetProperty(m_DB.m_Project.m_strSQLServer)), true );
	iniFile.SetValue(std::string("Main"), std::string("DataBaseName"), ConvertA(GetProperty(m_DB.m_Project.m_strDBName)), true );
	iniFile.SetValue(std::string("Main"), std::string("SharedDirectory"), ConvertA(GetProperty(m_DB.m_Project.m_strSharedDir)), true );
	iniFile.SetValue(std::string("Main"), std::string("Cameras"), ConvertA(GetProperty(m_DB.m_Slaves.size() + m_DB.m_Masters.size())), true );
	iniFile.SetValue(std::string("Main"), std::string("Clusters"), ConvertA(GetProperty(m_DB.m_Commutators.size())), true );
	iniFile.SetValue(std::string("Main"), std::string("VideoStandard"), ConvertA(GetProperty(m_DB.m_Project.m_dwVideoStandard )), true );
}
#endif

void	CDepCfgFullDoc::ExportCommutator(CIniFile& iniFile) const
{
	int dwCounter = 1;
	std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.begin();
	for(;itr !=m_DB.m_Commutators.end();++itr, ++dwCounter)
	{
		CString Name;
		Name.Format(_T("Commutator%d"), dwCounter);
		std::string Header ( ConvertA(Name) );

		iniFile.SetValue(Header, std::string("Name"), ConvertA(GetProperty(itr->m_strName)), true );
#ifndef USE_AUTO_COMP2COMM
		iniFile.SetValue(Header, std::string("DWServerIP"), ConvertA(m_DB.GetComp2Comm(itr->GetID())), true );
#else
		std::pair<bool, CString> pp = m_DB.GetIPForCommutator();
		CString sIPValue = pp.first ? pp.second : CString();
		iniFile.SetValue(Header, std::string("DWServerIP"), ConvertA(sIPValue), true );
#endif
#ifdef USE_OLD_FORMAT
		iniFile.SetValue(Header, std::string("Delay"), ConvertA(GetProperty(itr->m_dwDelay)), true );
		iniFile.SetValue(Header, std::string("MasterDelay"), ConvertA(GetProperty(itr->m_dwMasterDelay)), true );
		iniFile.SetValue(Header, std::string("EmptyMasterDelay"), ConvertA(GetProperty(itr->m_dwEmptyMasterDelay)), true );
		iniFile.SetValue(Header, std::string("PredictionTime"), ConvertA(GetProperty(itr->m_dwPredictionTime)), true );
#endif
		bool bSlave = m_DB.IsConnectSlave( itr->GetID() );

		iniFile.SetValue(Header, std::string("Virtual") , ConvertA(GetProperty(!bSlave)), true );
	}
}

void	CDepCfgFullDoc::ExportMaster(CIniFile& iniFile) const
{
	int dwCounter = 1;
	std::set<MasterSettings>::const_iterator itr = m_DB.m_Masters.begin();
	for(;itr !=m_DB.m_Masters.end();++itr, ++dwCounter)
	{
		CString Name;
		Name.Format(_T("Camera%d"), dwCounter);
		std::string Header ( ConvertA(Name) );

		iniFile.SetValue(Header, std::string("CameraID"), ConvertA(GetProperty(itr->m_dwCameraID)), true );
#ifdef USE_OLD_FORMAT
		iniFile.SetValue(Header, std::string("CapturePin"), ConvertA(GetProperty(itr->m_dwCapturePin)), true );
		iniFile.SetValue(Header, std::string("CaptureBW"), ConvertA(GetProperty(itr->m_dwCaptureBW)), true );
#endif

		try
		{
			DWORD dwID = m_DB.GetConnection(itr->GetID(), 0);
			const ComputerSettings* cs = m_DB.GetComputer( dwID );
			iniFile.SetValue(Header, std::string("CameraIP"), ConvertA(GetProperty(cs->m_strIP)), true );
		}
		catch(ConnectionException&)
		{
			iniFile.SetValue(Header, std::string("CameraIP"), "", true );
		}
		
		try
		{
			DWORD dwID = m_DB.GetConnection(itr->GetID(), 1);
			std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.find(  dwID );
			ASSERT(itr != m_DB.m_Commutators.end());
			iniFile.SetValue(Header, std::string("Commutator"), ConvertA(GetProperty(std::distance(m_DB.m_Commutators.begin(), itr) + 1)), true );
		}
		catch(ConnectionException&)
		{
			iniFile.SetValue(Header, std::string("Commutator"), "", true );
		}

		iniFile.SetValue(Header, std::string("CameraType"), ConvertA(GetProperty(0)), true );
#ifdef USE_OLD_FORMAT
		iniFile.SetValue(Header, std::string("EnableArchive"), ConvertA(GetProperty(itr->m_EnableArchive)), true );
		iniFile.SetValue(Header, std::string("EnableDataWare"), ConvertA(GetProperty(itr->m_EnableDataWare)), true );
#endif
		iniFile.SetValue(Header, std::string("PTZIP"), "", true );
		iniFile.SetValue(Header, std::string("PTZDevice"), "", true );
		iniFile.SetValue(Header, std::string("RS485ID"), "", true );
	}
}

void	CDepCfgFullDoc::ExportSlave(CIniFile& iniFile) const
{
	int dwCounter = 1 + m_DB.m_Masters.size();
	std::set<SlaveSettings>::const_iterator itr = m_DB.m_Slaves.begin();
	for(;itr !=m_DB.m_Slaves.end();++itr, ++dwCounter)
	{
		CString Name;
		Name.Format(_T("Camera%d"), dwCounter);
		std::string Header ( ConvertA(Name) );

		iniFile.SetValue(Header, std::string("CameraID"), ConvertA(GetProperty(itr->m_dwCameraID)), true );
#ifdef USE_OLD_FORMAT
		iniFile.SetValue(Header, std::string("CapturePin"), ConvertA(GetProperty(itr->m_dwCapturePin)), true );
		iniFile.SetValue(Header, std::string("CaptureBW"), ConvertA(GetProperty(itr->m_dwCaptureBW)), true );
#endif
		try
		{
			DWORD dwID = m_DB.GetConnection(itr->GetID(), 0);
			const ComputerSettings* cs = m_DB.GetComputer( dwID );
			iniFile.SetValue(Header, std::string("CameraIP"), ConvertA(GetProperty(cs->m_strIP)), true );
		}
		catch(ConnectionException&)
		{
			iniFile.SetValue(Header, std::string("CameraIP"), "", true );
		}
		
		try
		{
			DWORD dwID = m_DB.GetConnection(itr->GetID(), 2);
			std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.find(  dwID );
			ASSERT(itr != m_DB.m_Commutators.end());
			iniFile.SetValue(Header, std::string("Commutator"), ConvertA(GetProperty(std::distance(m_DB.m_Commutators.begin(), itr) + 1)), true );
		}
		catch(ConnectionException&)
		{
			iniFile.SetValue(Header, std::string("Commutator"), "", true );
		}

		iniFile.SetValue(Header, std::string("CameraType"), ConvertA(GetProperty(1)), true );
		iniFile.SetValue(Header, std::string("EnableArchive"), "", true );
		iniFile.SetValue(Header, std::string("EnableDataWare"), "", true );

		try
		{
			DWORD dwID = m_DB.GetConnection(itr->GetID(), 1);
			const ComputerSettings* cs = m_DB.GetComputer( dwID );
			iniFile.SetValue(Header, std::string("PTZIP"), ConvertA(GetProperty(cs->m_strIP)), true );
		}
		catch(ConnectionException&)
		{
			iniFile.SetValue(Header, std::string("PTZIP"), "", true );
		}

		iniFile.SetValue(Header, std::string("PTZDevice"), ConvertA(GetProperty(itr->m_dwPTZDevice)), true );
		iniFile.SetValue(Header, std::string("RS485ID"), ConvertA(GetProperty(itr->m_dwRS485ID)), true );

		if( !itr->m_dwRS485ID.IsSet() )
		{
			CString warn;
			warn.Format( _T("Slave%s - RS485ID not assigned"), GetProperty(itr->m_dwCameraID) );
			m_Report.AddString(warn);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string LoadRes(UINT nID)
{
	CString s;
	s.LoadString(nID);
	return ConvertA( s );
}

void CDepCfgFullDoc::OnFileImport() 
{
#ifdef USE_OLD_FORMAT
	BOOL res = SaveModified();
	if(!res) return;	

	// TODO: Add your command handler code here
	LPCTSTR szFilter = _T("Ini file(*.ini)|*.ini||");
	CFileDialog dlg( TRUE, _T("ini"), NULL, OFN_OVERWRITEPROMPT, szFilter, AfxGetMainWnd() );
	if(dlg.DoModal() == IDOK)
	{
		AfxGetMainWnd()->SendMessage( WM_COMMAND, ID_FILE_NEW );

		CIniFile iniFile;
		iniFile.Path( ConvertA(dlg.GetPathName()));
		iniFile.ReadFile();
 
		try
		{
			ProjectImport p = ImportProject(iniFile);
			ImportCommutator(iniFile, p.m_dwCommutatorNumber);
			ImportCamera(iniFile, p.m_dwCameraNumber);

			POSITION pos = GetFirstViewPosition();
			CEditorView* pView = (CEditorView*)GetNextView(pos);
			pView->OnInitialUpdate();
//			pView->AdjustScroll(false);
			SetModifiedFlag();
			UpdateAllViews(0);
			pView = (CEditorView*)GetNextView(pos);
			pView->OnInitialUpdate();
		}
		catch(ImportException& ex)
		{
			DeleteContents();
			AfxMessageBox( Helper::Convert(ex.what()), MB_ICONERROR|MB_OK);
			SetModifiedFlag(FALSE);
			UpdateAllViews(0);
		}
		catch(CommonException& )
		{
			DeleteContents();
			AfxMessageBox(IDS_XML_ERROR, MB_ICONERROR|MB_OK);
			SetModifiedFlag(FALSE);
			UpdateAllViews(0);
		}
	}	
#endif
}

#ifdef USE_OLD_FORMAT
CDepCfgFullDoc::ProjectImport	CDepCfgFullDoc::ImportProject( const CIniFile& iniFile )	throw (ImportException)
{
	ProjectImport p ;
	int res = iniFile.FindKey("Main");
//	std::pair<DWORD,DWORD> p;
	if(res == CIniFile::noID)	throw ImportException(LoadRes(IDS_EXC_IMPORT_MAIN));
	try
	{
		std::string val = iniFile.GetValue( std::string("Main"), std::string("SQLServerName"));
		m_DB.m_Project.m_strSQLServer = CString(val.c_str());
	}
	catch(IniValueException&) {}
	try
	{
		std::string val = iniFile.GetValue( std::string("Main"), std::string("DataBaseName"));
		m_DB.m_Project.m_strDBName = CString(val.c_str());
	}
	catch(IniValueException&) {}
	try
	{
		std::string val = iniFile.GetValue( std::string("Main"), std::string("SharedDirectory"));
		m_DB.m_Project.m_strSharedDir = CString(val.c_str());
	}
	catch(IniValueException&) {}

	try
	{
		p.m_dwCameraNumber = iniFile.GetValueI( std::string("Main"), std::string("Cameras"));
	}
	catch(IniValueException&) 
	{
		throw ImportException(LoadRes(IDS_EXC_IMPORT_CAMNUM));
	}

	try
	{
		p.m_dwCommutatorNumber = iniFile.GetValueI( std::string("Main"), std::string("Clusters"));
	}
	catch(IniValueException&) 
	{
		throw ImportException(LoadRes(IDS_EXC_IMPORT_CLSNUM));
	}
	try
	{
		p.m_dwVideoStandard = iniFile.GetValueI( std::string("Main"), std::string("VideoStandard"));
	}
	catch(IniValueException&)
	{
		p.m_dwVideoStandard = m_DB.GetDefVideoStandatd();
	}

	return p;
}
#endif

void	CDepCfgFullDoc::ImportCommutator( const CIniFile& iniFile, DWORD dwCount )	throw (ImportException)
{
	for(DWORD i=1; i <= dwCount; ++i)
	{
		CString Name;
		Name.Format(_T("Commutator%d"), i);
		std::string Section(ConvertA(Name));
		int res = iniFile.FindKey(Section);
		if(res == CIniFile::noID)	
		{
			CString s;
			s.Format(IDS_EXC_IMPORT_COMSEC, i);
			throw ImportException( ConvertA(s));
		}
		CommutatorSettings cs;
		try
		{
			CString val = iniFile.GetValue( Section, std::string("Name")).c_str();
			cs.m_strName = val;
		}
		catch(IniValueException&) {}
#ifdef USE_OLD_FORMAT
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("Delay"));
			cs.m_dwDelay = val;
		}
		catch(IniValueException&) {}
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("MasterDelay"));
			cs.m_dwMasterDelay = val;
		}
		catch(IniValueException&) {}
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("EmptyMasterDelay"));
			cs.m_dwEmptyMasterDelay = val;
		}
		catch(IniValueException&) {}
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("PredictionTime"));
			cs.m_dwPredictionTime = val;
		}
		catch(IniValueException&) {}
#endif

		try
		{
			CString val = iniFile.GetValue( Section, std::string("DWServerIP")).c_str();
			DWORD dwComp = m_DB.AddComputer(val);
#ifndef USE_AUTO_COMP2COMM
			m_DB.AddComp2Comm( Comp2Comm(dwComp, cs.GetID() ));
#endif
		}
		catch(IniValueException&) {}
	
		m_DB.AddCommutator(cs);
	}

	std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.begin();
	for(DWORD count = 0; itr != m_DB.m_Commutators.end() ; ++itr, ++count)
	{
		GuiItem* pItem = new GuiCommutator(m_DB, itr->GetID(), RectF(10 , 10 + 120 * count, 80, 100)); 
		m_LogicDB.AddItem( pItem) ;
	}	
}

void	CDepCfgFullDoc::ImportCamera( const CIniFile& iniFile, DWORD dwCount )		throw (ImportException)
{
	for(DWORD i=1; i <= dwCount; ++i)
	{
		CString Name;
		Name.Format( _T("Camera%d"), i);
		std::string Section( ConvertA(Name));
		int res = iniFile.FindKey(Section);
		if(res == CIniFile::noID)	
		{
			CString s;
			s.Format(IDS_EXC_IMPORT_CAMSEC, i);
			throw ImportException(ConvertA(s));
		}
		DWORD val = 0;
		try
		{
			val = iniFile.GetValueI( Section, std::string("CameraType"));
		}
		catch(IniValueException&) 
		{
			CString s;
			s.Format(IDS_EXC_IMPORT_CAMTYPE, i);
			throw ImportException(ConvertA(s));
		}
		
		if(val == 0) 
		{
			MasterSettings ms;
			ImportCommonCamera(Section, iniFile, ms, true);
#ifdef USE_OLD_FORMAT
			try
			{
				bool val = iniFile.GetValueB( Section, std::string("EnableArchive"));
				ms.m_EnableArchive = val;
			}
			catch(IniValueException&) {}
			try
			{
				DWORD val = iniFile.GetValueI( Section, std::string("EnableDataWare"));
				ms.m_EnableDataWare = val;
			}
			catch(IniValueException&) {}
#endif
			m_DB.AddMaster(ms);

			GuiItem* pItem = new GuiLogicMaster(m_DB, ms.GetID(), RectF(200 , 10 + 120 * (i-1), 80, 100)); 
			m_LogicDB.AddItem( pItem) ;

			pItem = new GuiPhysicMaster(m_DB, ms.GetID(), RectF(200 , 10 + 120 * (i-1), 80, 100)); 
			m_PhysicDB.AddItem( pItem) ;
			
		}
		else /*if(val == 1)*/
		{
			SlaveSettings ss;
			ImportCommonCamera(Section, iniFile, ss, false);
			try
			{
				DWORD val = iniFile.GetValueI( Section, std::string("PTZDevice"));
				ss.m_dwPTZDevice = val;
			}
			catch(IniValueException&) {}
			try
			{
				DWORD val = iniFile.GetValueI( Section, std::string("RS485ID"));
				ss.m_dwRS485ID = val;
			}
			catch(IniValueException&) {}

			try
			{
				CString val = iniFile.GetValue( Section, std::string("PTZIP")).c_str();
				
				DWORD dwComp = m_DB.AddComputer( val );

				Connection c;
				c.First = ss.GetID();
				c.Second = dwComp;
				c.SecondSlot = m_DB.GetFreeComputerSlot(dwComp);
				c.FirstSlot = 1;

				m_DB.AddConnection( c );

				

				GuiItem* pItem = new GuiConnection(m_PhysicDB, c.GetID()); 
				m_PhysicDB.AddItem( pItem) ;
			}
			catch(IniValueException&) {}

			m_DB.AddSlave(ss);

			GuiItem* pItem = new GuiLogicSlave(m_DB, ss.GetID(), RectF(200 , 10 + 120 * (i-1), 80, 100)); 
			m_LogicDB.AddItem( pItem) ;

			pItem = new GuiPhysicSlave(m_DB, ss.GetID(), RectF(200 , 10 + 120 * (i-1), 80, 100)); 
			m_PhysicDB.AddItem( pItem) ;
		}
	}

	std::set<ComputerSettings>::const_iterator itr = m_DB.m_Computers.begin();
	for(DWORD count = 0; itr != m_DB.m_Computers.end() ; ++itr, ++count)
	{
		GuiItem* pItem = new GuiComputer(m_DB, itr->GetID(), RectF(10 , 10 + 120 * count, 80, 100)); 
		m_PhysicDB.AddItem( pItem) ;
	}

}

void	CDepCfgFullDoc::ImportCommonCamera(const std::string Section,  const CIniFile& iniFile, CameraSettings&cs, bool master )
{
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("CameraID"));
			cs.m_dwCameraID = val;
		}
		catch(IniValueException&) {}
#ifdef USE_OLD_FORMAT
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("CapturePin"));
			cs.m_dwCapturePin = val;
		}
		catch(IniValueException&) {}
		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("CaptureBW"));
			cs.m_dwCaptureBW = val;
		}
		catch(IniValueException&) {}
#endif
		try
		{
			CString val = iniFile.GetValue( Section, std::string("CameraIP")).c_str();
			DWORD dwComp = m_DB.AddComputer(val);

			Connection c;
			c.First = cs.GetID();
			c.Second = dwComp;
			c.SecondSlot = m_DB.GetFreeComputerSlot(dwComp);
			c.FirstSlot = 0;

			m_DB.AddConnection( c );

			

			GuiItem* pItem = new GuiConnection(m_PhysicDB, c.GetID()); 
			m_PhysicDB.AddItem( pItem) ;
		}
		catch(IniValueException&) {}

		try
		{
			DWORD val = iniFile.GetValueI( Section, std::string("Commutator"));
			std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.begin();
			std::advance(itr, val-1);

			Connection c;
			c.First = cs.GetID();
			c.Second = itr->GetID();
			c.SecondSlot = m_DB.GetFreeCommutatorSlot(itr->GetID());
			c.FirstSlot = master ?1 : 2;

			m_DB.AddConnection( c );

			

			GuiItem* pItem = new GuiConnection(m_LogicDB, c.GetID()); 
			m_LogicDB.AddItem( pItem) ;
		}
		catch(IniValueException&) {}

}

CString CDepCfgFullDoc::Report:: GetReport() const
{
	if( m_Strings.empty() ) return CString();
	CString rep = m_Strings[0];
	for(  size_t i=1; i < m_Strings.size(); ++i)
	{
		rep += CString("\n\r") + m_Strings[i]; 
	}
	return rep;
}

/////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Purpose: Verify computer states
//-----------------------------------------------------------------------------
void	CDepCfgFullDoc::VerifyComputer() const
{
//	int dwCounter = 1;
	std::set<ComputerSettings>::const_iterator itr = m_DB.m_Computers.begin();
	for(;itr !=m_DB.m_Computers.end();++itr)
	{
		CString Name = GetProperty( itr->m_strIP );
//		if(!Name.IsEmpty()) Name = CString(" ") + Name;
		if( !itr->m_strIP.IsSet() || itr->m_strIP.Get() .IsEmpty()  )
		{
			CString strError;
			strError.LoadString( IDS_FMT_COMPUTER_REPORT_2 );
			m_Report.AddString( strError );
			continue;
		}
		
		for( int i= 0; i < 5; ++i)
		{
			bool	bSlave	= m_DB.IsAllSlave( itr->GetID(), i );
			bool	bFree	= m_DB.IsFreeSlot( itr->GetID(), i );			// HACK
			//FIXME:  Great hack see ChangeProperty.cpp!!! it's PTZ Device property
			bool	bInt0	= m_DB.IsAllSlavePTZ( itr->GetID(), i, 0 );
			bool	bInt1	= m_DB.IsAllSlavePTZ( itr->GetID(), i, 1 );
			bool	bInt2	= m_DB.IsAllSlavePTZ( itr->GetID(), i, 2 );
			if( bSlave && !bFree && !bInt0 && !bInt1 && !bInt2 )	
			{
				CString strReport;
				strReport.Format( UINT(IDS_FMT_COMPUTER_REPORT_0), (LPCTSTR)Name, i);
				m_Report.AddString(strReport);
			}
		}
	}	
}

#ifdef USE_OLD_FORMAT
//-----------------------------------------------------------------------------
// Purpose: Verify & check project settings
//-----------------------------------------------------------------------------
void	CDepCfgFullDoc::VerifyProject() const
{
	CString strError;
	if( !m_DB.m_Project.m_strSQLServer.IsSet() ) 
	{
		strError.LoadString( IDS_FMT_PROJECT_REPORT_0 );
		m_Report.AddString( strError );
	}
	if( !m_DB.m_Project.m_strDBName.IsSet() ) 
	{
		strError.LoadString( IDS_FMT_PROJECT_REPORT_1 );
		m_Report.AddString( strError );
	}
	if( !m_DB.m_Project.m_strSharedDir.IsSet() ) 
	{
		strError.LoadString( IDS_FMT_PROJECT_REPORT_2 );
		m_Report.AddString( strError );
	}
	if( !m_DB.m_Project.m_dwVideoStandard.IsSet() ) 
	{
		strError.LoadString( IDS_FMT_PROJECT_REPORT_3 );
		m_Report.AddString( strError );
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose: Check camera states
//-----------------------------------------------------------------------------
void	CDepCfgFullDoc::VerifyCamera() const
{
	CString strError;
	std::set<MasterSettings>::const_iterator itrMS = m_DB.m_Masters.begin();
	for( ; itrMS != m_DB.m_Masters.end() ; ++itrMS)
	{	
		const MasterSettings& ms = *itrMS;

		if( !ms.m_dwCameraID.IsSet() )
		{
			strError.LoadString( IDS_FMT_MASTER_REPORT_4 );
			m_Report.AddString( strError );
			continue;
		}

		if ( !m_DB.IsCameraConnectedWithComputer( ms.GetID() ) )
		{
			//FIXME : resources
			strError.Format( /*L"Master %d - не соединен с компьютером"*/IDS_FMT_MASTER_NO_COMP_CONNECTION, ms.m_dwCameraID.Get() );
			m_Report.AddString( strError );
		}

		if ( !m_DB.IsCameraConnectedWithCommutator( ms.GetID() ) )
		{
			//FIXME : resources				
			strError.Format( /*L"Master %d - не соединен с коммутатором"*/IDS_FMT_MASTER_NO_COMM_CONNECTION, ms.m_dwCameraID.Get() );
			m_Report.AddString( strError );
		}

		
#ifdef USE_OLD_FORMAT
		if(!ms.m_dwCapturePin.IsSet())
		{
			strError.Format( IDS_FMT_MASTER_REPORT_0, ms.m_dwCameraID.Get() );
			m_Report.AddString(strError);
		}
		if(!ms.m_dwCaptureBW.IsSet())
		{
			strError.Format( IDS_FMT_MASTER_REPORT_1, ms.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}
		if(!ms.m_EnableArchive.IsSet())
		{
			strError.Format( IDS_FMT_MASTER_REPORT_2, ms.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}
		if(!ms.m_EnableDataWare.IsSet())
		{
			strError.Format( IDS_FMT_MASTER_REPORT_3, ms.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}
#endif
	}

	std::set<SlaveSettings>::const_iterator itrSS = m_DB.m_Slaves.begin();
	for( ; itrSS != m_DB.m_Slaves.end() ; ++itrSS)
	{
		const SlaveSettings& ss = *itrSS;

		if( !ss.m_dwCameraID.IsSet() )
		{
			strError.LoadString( IDS_FMT_SLAVE_REPORT_4 );
			m_Report.AddString( strError );
			continue;
		}

		if ( !m_DB.IsCameraConnectedWithComputer( ss.GetID() ) )
		{
			//FIXME : resources
			strError.Format( /*L"Slave %d - не соединен с компьютером"*/ IDS_FMT_SLAVE_NO_COMP_CONNECTION, ss.m_dwCameraID.Get() );
			m_Report.AddString( strError );
		}

		if ( !m_DB.IsCameraConnectedWithCommutator( ss.GetID() ) )
		{
			//FIXME : resources				
			strError.Format( /*L"Slave %d - не соединен с коммутатором"*/ IDS_FMT_SLAVE_NO_COMM_CONNECTION, ss.m_dwCameraID.Get() );
			m_Report.AddString( strError );
		}

		if ( !m_DB.IsSlaveConnectedWithComputer( ss.GetID() ) )
		{
			//FIXME : resources
			strError.Format(/* L"Slave %d - не соединен с компьютером"*/IDS_FMT_SLAVE_NO_COMP_CONNECTION, ss.m_dwCameraID.Get() );
			m_Report.AddString( strError );
		}

#ifdef USE_OLD_FORMAT
		if(!ss.m_dwCapturePin.IsSet())
		{
			strError.Format( IDS_FMT_SLAVE_REPORT_0, ss.m_dwCameraID.Get() );
			m_Report.AddString(strError);
		}

		if(!ss.m_dwCaptureBW.IsSet())
		{
			strError.Format( IDS_FMT_SLAVE_REPORT_1, ss.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}
#endif
		if(!ss.m_dwPTZDevice.IsSet())
		{
			strError.Format( IDS_FMT_SLAVE_REPORT_2, ss.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}
		if(!ss.m_dwRS485ID.IsSet())
		{
			strError.Format( IDS_FMT_SLAVE_REPORT_3, ss.m_dwCameraID.Get() );
			m_Report.AddString(strError);			
		}		

	}
}

//-----------------------------------------------------------------------------
// Purpose: Check commutator states
//-----------------------------------------------------------------------------
void	CDepCfgFullDoc::VerifyCommutator() const
{
	CString strError;
	std::set<CommutatorSettings>::const_iterator itr = m_DB.m_Commutators.begin();
	std::set<CString> CommutatorNames;
	for( ; itr != m_DB.m_Commutators.end() ; ++itr)
	{
		const CommutatorSettings& cs = *itr;
		//NOTENOTE: when named is undefine we skip validation
		if( !cs.m_strName.IsSet() || cs.m_strName.Get().IsEmpty() )
		{
			strError.LoadString( IDS_FMT_COMMUTATOR_REPORT_5 );
			m_Report.AddString(strError);
			continue;
		}
		
		CString sCmpName = cs.m_strName.Get();
		sCmpName.MakeLower();
		std::set<CString>::const_iterator itName = CommutatorNames.find( sCmpName );
		if( itName != CommutatorNames.end()  )
		{
			//FIXME : resource
			strError.Format(_T("Commutator %s - такое имя уже есть"), (LPCTSTR)cs.m_strName.Get() );
			m_Report.AddString( strError );
			continue;			
		}
		else 
		{
			CommutatorNames.insert( sCmpName );
		}
#ifndef USE_AUTO_COMP2COMM
		CString strCompName = m_DB.GetComp2Comm( cs.GetID() ) ;
#else
		std::pair<bool, CString> p = m_DB.GetIPForCommutator();
		CString strCompName = p.first? p.second : CString() ;
#endif
		if(strCompName.IsEmpty())
		{
			strError.Format( IDS_FMT_COMMUTATOR_REPORT_0, (LPCTSTR)cs.m_strName.Get() );
			m_Report.AddString(strError);
		}
#ifdef USE_OLD_FORMAT
		if(!cs.m_dwDelay.IsSet())
		{
			strError.Format( IDS_FMT_COMMUTATOR_REPORT_1, (LPCTSTR)cs.m_strName.Get() );
			m_Report.AddString(strError);			
		}
		if(!cs.m_dwMasterDelay.IsSet())
		{
			strError.Format( IDS_FMT_COMMUTATOR_REPORT_2, (LPCTSTR)cs.m_strName.Get() );
			m_Report.AddString(strError);			
		}
		if(!cs.m_dwEmptyMasterDelay.IsSet())
		{
			strError.Format( IDS_FMT_COMMUTATOR_REPORT_3, (LPCTSTR)cs.m_strName.Get() );
			m_Report.AddString(strError);			
		}
#endif
	}
}

///////


void CDepCfgFullDoc::OnFileDBSave()
try
{
	if( !Verify() ) return;
	boost::shared_ptr< DepCfg::IDBSerializer > pInt = 
		DepCfg::CreateInternalInterface<DepCfg::IDBSerializer>( DBSERIALIZER_INTERFACE );
	
	std::vector<LogicReport::Cluster> Clusters;
	m_DB.CreateReport( Clusters );
	
	pInt->CreateDatabase(Clusters);

	std::ostringstream os;
	Save(os);
	std::string s = os.str();
	
	pInt->SaveFile( &s[0], s.length() );
	SetModifiedFlag( FALSE );
	AfxMessageBox( IDS_DB_SAVE_OK, MB_OK|MB_ICONINFORMATION );
}
catch( DBConnectionException ) 
{
	AfxMessageBox( IDS_DB_CONNECTION, MB_OK|MB_ICONERROR );
}
catch( DatabaseException )
{
	AfxMessageBox( IDS_DB_SAVE, MB_OK|MB_ICONERROR );
}
catch( CameraConnectionException )
{
	AfxMessageBox( IDS_DB_CAMERAS, MB_OK|MB_ICONERROR );
};

void CDepCfgFullDoc::OnFileDBLoad()
try
{
	BOOL res = SaveModified();
	if(!res) return;	
	
	DeleteContents();

	boost::shared_ptr< DepCfg::IDBSerializer > pInt = 
		DepCfg::CreateInternalInterface<DepCfg::IDBSerializer>( DBSERIALIZER_INTERFACE );

	std::vector<BYTE> BinArr;
	pInt->LoadFile( BinArr );
	std::vector<char> CharArr( BinArr.begin(), BinArr.end() );
	CharArr.push_back( '\0' );
#if 0
	throw DBConnectionException("zzz");
#endif
	try
	{
		Load(CharArr);
	}
	catch(XMLVersionDBException)
	{
		AfxMessageBox( IDS_XML_VERSION_ERROR, MB_OK|MB_ICONERROR );
		return;
	}
	catch(XMLException)
	{
		AfxMessageBox( IDS_XML_ERROR, MB_OK|MB_ICONERROR );
		return;
	}

	POSITION pos = GetFirstViewPosition();
	while( pos )
	{
		CView* pView = GetNextView(pos);
		ASSERT(pView);
		pView->OnInitialUpdate();
	}
}
catch( DBConnectionException ) 
{
	AfxMessageBox( IDS_DB_CONNECTION, MB_ICONERROR|MB_OK );
	DeleteContents();
	SetModifiedFlag(FALSE);
	UpdateAllViews(0);
}
catch( DatabaseException )
{
	AfxMessageBox( IDS_DB_LOAD, MB_ICONERROR|MB_OK );
	DeleteContents();
	SetModifiedFlag(FALSE);
	UpdateAllViews(0);
};