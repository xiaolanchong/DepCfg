//========================= Copyright © 2004, Elvees ==========================
// Author:	Eugene V. Gorbachev
// Purpose: implementation of the CDepCfgDoc class
// Revision:12-09-2004 added database validation - empty parameters
//
//=============================================================================


#include "stdafx.h"
//#include "DepCfg.h"

#include "DepCfgDoc.h"
#include "../item/GuiComputer.h"
#include "../item/GuiCamera.h"
#include "../item/GuiMaster.h"
#include "../item/GuiCommutator.h"
#include "../item/GuiConnection.h"
#include "../utility/Helper.h"

#include "../utility/XMLparser.h"
#include "../utility/iniFile.h"
#ifndef DEPCFGLITE
#include "MainFrm.h"
#include "SettingsDlg.h"
#endif

#ifdef DEPCFGLITE 
#include "../../depcfglite/docview/EditorView.h"
#else
#include "EditorView.h"
#endif

#include "../../DepCfgLite/interface/DepInterface.h"
#include "../../DepCfgLite/interface/IResourceLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using Helper::ConvertA;
using Helper::ConvertW;

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc

IMPLEMENT_DYNCREATE(CDepCfgDoc, CDocument)

BEGIN_MESSAGE_MAP(CDepCfgDoc, CDocument)
	//{{AFX_MSG_MAP(CDepCfgDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc construction/destruction

CDepCfgDoc::CDepCfgDoc():m_PhysicDB(m_DB, this), m_LogicDB(m_DB, this)
{
	// TODO: add one-time construction code here
	::CoInitialize(NULL);
}

CDepCfgDoc::~CDepCfgDoc()
{
	::CoUninitialize();
}

BOOL CDepCfgDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	m_DB.SetDefaultSettings();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc diagnostics

#ifdef _DEBUG
void CDepCfgDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDepCfgDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDepCfgDoc commands

SizeF	CDepCfgDoc::GetPhysicPos() const
{
	POSITION pos = GetFirstViewPosition();
	CEditorView* pView = (CEditorView*)GetNextView(pos);
	SizeF size(pView->GetScrollPos(SB_HORZ) , pView->GetScrollPos(SB_VERT));

	return pView->m_Zoom.GetSize(size);
}

SizeF	CDepCfgDoc::GetLogicPos() const
{
	POSITION pos = GetFirstViewPosition();
	GetNextView(pos);
	CEditorView* pView = (CEditorView*)GetNextView(pos);
	SizeF size(pView->GetScrollPos(SB_HORZ) , pView->GetScrollPos(SB_VERT));

	return pView->m_Zoom.GetSize(size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	CDepCfgDoc::Load(const std::vector<char>& os)	
{
	//NOTENOTE: We set default, all loaded settings rewrite it
	m_DB.SetDefaultSettings();

	CComPtr<IXMLDOMDocument> iXMLDoc;
	iXMLDoc.CoCreateInstance(__uuidof(DOMDocument));
 		 
	VARIANT_BOOL bSuccess=false;
	iXMLDoc->loadXML(CComBSTR(&os[0]),&bSuccess);

	if(!bSuccess) throw XMLException(std::string());
	CComPtr<IXMLDOMElement> iRootElm;
	iXMLDoc->get_documentElement(&iRootElm);
	XMLElem eroot(iRootElm);

	if( eroot.name() != L"base") throw XMLException(std::string());
	std::wstring strVersion = eroot.attr(L"version");
	//NOTENOTE: We can bring it to resource
	if( !wcslen(strVersion.c_str()) ) throw XMLException(std::string("Wrong file version")); 
	double fVersionNumber = wcstod(strVersion.c_str(), 0 );
	if( fVersionNumber < 1.0 ) throw XMLException(std::string("Wrong file version")); 
//	if( eroot.attr() )

	XMLElem e = eroot.begin(), ec, CurElem;

	if( e.name() != L"data" ) throw XMLException(std::string());
	DWORD dwCurID = e.attrInt(L"Counter");
	DbItem::SetCounter(dwCurID);
#ifdef USE_OLD_FORMAT
	ec = e.subnode(L"project");
	LoadProject(&ec, fVersionNumber);
#endif
	ec = e.subnode(L"computers");
	for (  CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"computer") throw XMLException(std::string());
		LoadComputer(&CurElem, fVersionNumber);

	}
	  
	ec = e.subnode(L"masters");
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"master") throw XMLException(std::string());
		LoadMaster(&CurElem, fVersionNumber);

	}

	ec = e.subnode(L"slaves");
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"slave") throw XMLException(std::string());
		LoadSlave(&CurElem, fVersionNumber);

	}

	ec = e.subnode(L"commutators");
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"commutator") throw XMLException(std::string());
		LoadCommutator(&CurElem, fVersionNumber);

	}

	ec = e.subnode(L"connections");
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"connection") throw XMLException(std::string());
		LoadConnection(&CurElem, fVersionNumber);
	}
	// ignore in new version
#ifndef USE_AUTO_COMP2COMM
	ec = e.subnode(L"comp2comms");
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		if( CurElem.name() != L"comp2comm") throw XMLException(std::string());
		LoadComp2Comm(&CurElem, fVersionNumber);
	}
#endif
	e++;
	if( e.name() != L"physic" ) throw XMLException(std::string());

	ec = e;
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		std::wstring str = CurElem.name();
		if( str == L"computer") LoadGuiComputer(&CurElem, fVersionNumber);
		else if( str == L"slave" ) LoadGuiPhySlave(  &CurElem, fVersionNumber);
		else if( str == L"master" ) LoadGuiPhyMaster(  &CurElem, fVersionNumber);
		else if( str == L"connection" ) LoadGuiConnection(  &CurElem, m_PhysicDB, fVersionNumber);
		else throw XMLException(std::string());
	}

	e++;
	if( e.name() != L"logic" ) throw XMLException(std::string());
	ec = e;
	for ( CurElem = ec.begin(); CurElem != ec.end(); CurElem++)
	{
		std::wstring str = CurElem.name();
		if( str == L"slave" ) LoadGuiLogSlave(  &CurElem, fVersionNumber);
		else if( str == L"master" ) LoadGuiLogMaster(  &CurElem, fVersionNumber);
		else if( str == L"connection" ) LoadGuiConnection(  &CurElem, m_LogicDB, fVersionNumber);
		else if( str == L"commutator" ) LoadGuiCommutator(  &CurElem, fVersionNumber);
		else throw XMLException(std::string());
	}
	
}

void	CDepCfgDoc::LoadComputer(XMLElem* e, double fVersionNumber)
{
	DWORD dwID = e->attrInt(L"ID");
	ComputerSettings cs(dwID);
	SetAttr( e->attr(L"IP"), cs.m_strIP );
	try
	{
		std::wstring strDNSName = e->attr(L"DNSName");
		SetAttr( strDNSName, cs.m_strDnsName );
	}
	catch(StringException)
	{
		// ignore if that attribute doesn't exist
	}

	m_DB.AddComputer(cs);
}

void	CDepCfgDoc::LoadSlave(XMLElem* e, double fVersionNumber)
{
	DWORD dwID = e->attrInt(L"ID");
	SlaveSettings ss(dwID);
	SetAttr( e->attr(L"CameraID"), ss.m_dwCameraID );
#ifdef USE_OLD_FORMAT
	SetAttr( e->attr(L"CapturePin"), ss.m_dwCapturePin );
	SetAttr( e->attr(L"CaptureBW"), ss.m_dwCaptureBW );
#endif
	SetAttr( e->attr(L"PTZDevice"), ss.m_dwPTZDevice );
	SetAttr( e->attr(L"RS485ID"), ss.m_dwRS485ID );
	m_DB.AddSlave(ss);
}

void	CDepCfgDoc::LoadMaster(XMLElem* e, double fVersionNumber)
{
	DWORD dwID = e->attrInt(L"ID");
	MasterSettings ms(dwID);
	SetAttr( e->attr(L"CameraID"), ms.m_dwCameraID );
#ifdef USE_OLD_FORMAT
	SetAttr( e->attr(L"CapturePin"), ms.m_dwCapturePin );
	SetAttr( e->attr(L"CaptureBW"), ms.m_dwCaptureBW );
	SetAttr( e->attr(L"EnableArchive"), ms.m_EnableArchive );
	SetAttr( e->attr(L"EnableDataWare"), ms.m_EnableDataWare );
#endif
	m_DB.AddMaster(ms);
}

void	CDepCfgDoc::LoadConnection(XMLElem* e, double fVersionNumber)
{
	DWORD dwID = e->attrInt(L"ID");
	Connection ss(dwID);
	ss.First = e->attrInt( L"First" );
	ss.Second = e->attrInt( L"Second" );
	ss.FirstSlot = e->attrInt( L"FirstSlot" );
	ss.SecondSlot = e->attrInt( L"SecondSlot" );
	m_DB.AddConnection(ss);

/*
	GuiItem* pItem = new GuiConnection(m_PhysicDB,ss.GetID()); 
	m_PhysicDB.AddItem( pItem) ;*/
}

void	CDepCfgDoc::LoadCommutator(XMLElem* e, double fVersionNumber)
{
	DWORD dwID = e->attrInt(L"ID");
	CommutatorSettings ss(dwID);

	SetAttr( e->attr(L"Name"), ss.m_strName);
#ifdef USE_OLD_FORMAT
	SetAttr( e->attr(L"Delay"), ss.m_dwDelay );
	SetAttr( e->attr(L"MasterDelay"), ss.m_dwMasterDelay );
	SetAttr( e->attr(L"EmptyMasterDelay"), ss.m_dwEmptyMasterDelay );
	SetAttr( e->attr(L"PredictionTime"), ss.m_dwPredictionTime);
#endif
	m_DB.AddCommutator(ss);


}
#ifndef USE_AUTO_COMP2COMM
void	CDepCfgDoc::LoadComp2Comm(XMLElem* e, double fVersionNumber)
{	
	DWORD	dwComp = e->attrInt(L"Comp"),
			dwComm = e->attrInt(L"Comm");
	Comp2Comm cc(dwComp, dwComm);
	m_DB.AddComp2Comm(cc);
}
#endif

RectF GetRect(XMLElem* e)
{
	return RectF ( e->attrInt(L"X"), e->attrInt(L"Y") ,  e->attrInt(L"Width"), e->attrInt(L"Height") );
}

void	CDepCfgDoc::LoadGuiComputer(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiComputer(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_PhysicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiPhySlave(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiPhysicSlave(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_PhysicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiPhyMaster(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiPhysicMaster(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_PhysicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiLogSlave(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiLogicSlave(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_LogicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiLogMaster(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiLogicMaster(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_LogicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiCommutator(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"rect" );
	GuiItem* pItem = new GuiCommutator(m_DB, e->attrInt(L"ID"), GetRect(& ec )); 
	m_LogicDB.AddItem( pItem) ;
}

void	CDepCfgDoc::LoadGuiConnection(XMLElem* e, CGuiDB& db, double fVersionNumber)
{
	GuiItem* pItem = new GuiConnection( db, e->attrInt(L"ID"));
	db.AddItem( pItem );
}

#ifdef USE_OLD_FORMAT
void	CDepCfgDoc::LoadProject(XMLElem* e, double fVersionNumber)
{
	XMLElem ec = e->subnode( L"server" );
	m_DB.m_Project.m_strSQLServer = Helper::Convert(ec.val());
	ec = e->subnode( L"database" );
	m_DB.m_Project.m_strDBName = Helper::Convert(ec.val());
	ec = e->subnode( L"shareddir" );
	m_DB.m_Project.m_strSharedDir = Helper::Convert(ec.val());
	ec = e->subnode( L"video_standard" );
	if( ec.elem )
	{
		m_DB.m_Project.m_dwVideoStandard = _wtoi ( ec.val().c_str() );
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////

void CDepCfgDoc::DeleteContents() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	m_DB.ClearDatabase();

	m_LogicDB.m_GuiDB.clear();
	m_PhysicDB.m_GuiDB.clear();
	DbItem::SetCounter(0);
	CDocument::DeleteContents();
}

void	CDepCfgDoc::DeleteItems(const std::set<DWORD> & Items)
{
	DWORD dwID = *Items.begin();
	std::set<DWORD> s ( m_DB.GetAllConnections(  dwID ) );

	s.insert( dwID );
	m_DB.DeleteItems(s);
	m_LogicDB.DeleteItem(s);
	m_PhysicDB.DeleteItem(s);
}

CStringW	CDepCfgDoc::LoadStringInt( UINT nID )
{
	std::shared_ptr< DepCfg::IResourceLoader > p = 
		DepCfg::CreateInternalInterface< DepCfg::IResourceLoader > (
#ifdef DEPCFGLITE
		RESOURCE_DLL_INTERFACE
#else
		RESOURCE_EXE_INTERFACE
#endif
		);
	return p->LoadStringInt(nID);
}