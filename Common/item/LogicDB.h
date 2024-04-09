// LogicDB.h: interface for the CLogicDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGICDB_H__684E5785_55A6_453D_BC23_1828C2DBCAFC__INCLUDED_)
#define AFX_LOGICDB_H__684E5785_55A6_453D_BC23_1828C2DBCAFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786)

#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>
#include "Exception.h"
#include "LogicReport.h"

const	BYTE	c_MaxConputerSlots	= 9;
const	DWORD	c_CompSignalSlot		= 0;
const	DWORD	c_CompControlSlot		= 1;

const	DWORD	c_CommMasterSlot		= 1;
const	DWORD	c_CommSlaveSlot		= 2;

#define USE_AUTO_COMP2COMM
#undef	USE_OLD_FORMAT

#ifdef DEPCFGLITE


const int sBattery_online = 0;
const int sBattery_battery = 1;
const int sBattery_offline = 2;
#endif

class	DbItem
{
	friend class CDepCfgDoc;
	friend class CDepCfgFullDoc;

	static		DWORD m_dwCurID;
	static void		SetCounter(DWORD dwID) { m_dwCurID = dwID; };
	static DWORD	GetCounter() { return m_dwCurID; };
public:
	DWORD		m_dwID;

public:
	DbItem() : m_dwID(m_dwCurID++){}
	DbItem(DWORD dwID) : m_dwID(dwID) {}
//	void	SetID(DWORD dwID) { m_dwID =  dwID;}
	DWORD	GetID() const { return m_dwID;}
	bool operator <(const DbItem& Item) const { return m_dwID < Item.m_dwID;}
};


MACRO_EXCEPTION(PropertyException, CommonException)
MACRO_EXCEPTION(UndefinedPropertyException, PropertyException)
MACRO_EXCEPTION(AssignPropertyException, PropertyException)


// we assume T is compatible for value passing like POD or CString

template< typename T> class Property
{
	bool		m_bSet;
	typename T	m_t;
public:
	Property(const Property<T>& prop) : m_bSet(prop.m_bSet), m_t(prop.m_t){ /* if(int(prop.m_bSet) > 1 ) throw 0;*/  }
	Property(T prop) : m_bSet(true), m_t(prop){}
	Property() : m_bSet(false){}

	bool		IsSet() const	{ return m_bSet;	};
	void		Reset() { m_bSet = false;}
/*	operator	*/T	Get() const	
	{ 
		if(!m_bSet) throw UndefinedPropertyException("Property undefined"); 
		return m_t;		
	}

	T operator	= (T t)			{ m_bSet = true; return this->m_t = t;}
	Property<T>& operator	= (const Property<T>& prop)			
	{ 
		if(&prop != this) 
		{
			if( !prop.IsSet() ) throw AssignPropertyException("Assign undefined property");
			t.m_t = prop.m_t; 
			t.m_bSet = prop.m_bSet;
		}
		return *this;
	}
};

#ifdef USE_OLD_FORMAT

struct Project
{
	Property<CString>		m_strSQLServer;
	Property<CString>		m_strDBName;
	Property<CString>		m_strSharedDir;
	Property<DWORD>			m_dwVideoStandard;
};

#endif

struct ComputerSettings : DbItem
{	
	Property<CString>		m_strIP;
	Property<CString>		m_strDnsName;
#ifdef DEPCFGLITE
	Property<DWORD>			m_dwNetState;
#endif
	ComputerSettings(){}
	ComputerSettings(DWORD dwID) : DbItem(dwID){}

	ComputerSettings(const ComputerSettings& cs) :DbItem(cs), 
		m_strIP( cs.m_strIP),
		m_strDnsName ( cs.m_strDnsName )
#ifdef DEPCFGLITE
		,m_dwNetState(cs.m_dwNetState)
#endif
		{}
};

struct CommutatorSettings  : DbItem
{
	Property<CString>		m_strName;
#ifdef USE_OLD_FORMAT
	Property<DWORD>			m_dwDelay;
	Property<DWORD>			m_dwMasterDelay;
	Property<DWORD>			m_dwEmptyMasterDelay;
	Property<DWORD>			m_dwPredictionTime;
#endif

	CommutatorSettings(){}
	CommutatorSettings(DWORD dwID) : DbItem(dwID){}

	CommutatorSettings(const CommutatorSettings& cs) :DbItem(cs), 
		m_strName( cs.m_strName)
#ifdef USE_OLD_FORMAT
		,m_dwDelay( cs.m_dwDelay),
		m_dwMasterDelay( cs.m_dwMasterDelay), 
		m_dwEmptyMasterDelay( cs.m_dwEmptyMasterDelay),
		m_dwPredictionTime( cs.m_dwPredictionTime)
#endif
	{}
};

struct CameraSettings : DbItem
{
	Property<DWORD>		m_dwCameraID;
#ifdef USE_OLD_FORMAT
	Property<DWORD>		m_dwCapturePin;
	Property<DWORD>		m_dwCaptureBW;
#endif
#ifdef DEPCFGLITE
	Property<ECamState>	m_NetState;
#endif
	
	CameraSettings(){}
	CameraSettings(DWORD dwID) : DbItem(dwID){}

	CameraSettings(const CameraSettings& cs) :
		DbItem(cs), 
		m_dwCameraID( cs.m_dwCameraID)
#ifdef USE_OLD_FORMAT
		,m_dwCapturePin( cs.m_dwCapturePin)
		,m_dwCaptureBW(cs.m_dwCaptureBW)
#endif
#ifdef DEPCFGLITE
		,m_NetState(cs.m_NetState)
#endif
		{}
};

struct SlaveSettings : CameraSettings
{
	Property<DWORD>		m_dwPTZDevice;
	Property<DWORD>		m_dwRS485ID;
#ifdef DEPCFGLITE
	Property<EPTZState>	m_PTZState;
#endif
	SlaveSettings(){}
	SlaveSettings(DWORD dwID) : CameraSettings(dwID){}

	SlaveSettings(const SlaveSettings& cs) :
	CameraSettings( cs), 
	m_dwPTZDevice( cs.m_dwPTZDevice),
	m_dwRS485ID(cs.m_dwRS485ID)
#ifdef DEPCFGLITE
	,m_PTZState(cs.m_PTZState)
#endif
	{}
};

struct MasterSettings : CameraSettings
{
#ifdef USE_OLD_FORMAT
	Property<bool>		m_EnableArchive;
	Property<bool>		m_EnableDataWare;
#endif
	MasterSettings(){}
	MasterSettings(DWORD dwID) : CameraSettings(dwID){}
	MasterSettings(const MasterSettings& cs) :
	CameraSettings( cs)
#ifdef USE_OLD_FORMAT
	,m_EnableArchive( cs.m_EnableArchive)
	,m_EnableDataWare(cs.m_EnableDataWare)
#endif
	{}

};

struct Connection : DbItem
{
	DWORD	First;
	DWORD	Second;
	DWORD	FirstSlot, SecondSlot;

	Connection(){}
	Connection(DWORD dwID) : DbItem(dwID){}
};

#ifndef USE_AUTO_COMP2COMM

struct Comp2Comm 
{
	DWORD m_dwComp, m_dwComm;
	Comp2Comm(DWORD dwComp, DWORD dwComm) : m_dwComp(dwComp), m_dwComm(dwComm){}
	bool operator <(const Comp2Comm& Item) const 
	{ 
		return m_dwComm < Item.m_dwComm;
	}
};

#endif

////////////// end /////////////////////

MACRO_EXCEPTION(DBException, CommonException)
MACRO_EXCEPTION(ConnectionException, DBException)
MACRO_EXCEPTION(CameraConnectionException, ConnectionException)

class CLogicDB
{
	friend class CDepCfgDoc;
	friend class CDepCfgLiteDoc;
	friend class CDepCfgFullDoc;

	enum 
	{
		VIDEO_STANDARD_0 = 0,
		VIDEO_STANDARD_1 = 1
	};
public:

	DWORD		GetCameraID() const;

	bool		IsFreeSlot(DWORD dwID, DWORD dwSlot) const;
	
	void	DeleteComputer(DWORD dwID);
	void	DeleteCommutator(DWORD dwID);
	void	DeleteMaster(DWORD dwID);
	void	DeleteSlave(DWORD dwID);
	void	DeleteConnection(DWORD dwID);

	void	DeleteItems(const std::set<DWORD> & s);

	const ComputerSettings*	GetComputer(DWORD dwID) const;
	ComputerSettings*		GetComputer(DWORD dwID);

	const CommutatorSettings*	GetCommutator(DWORD dwID) const;
	CommutatorSettings*		GetCommutator(DWORD dwID);

	const SlaveSettings*	GetSlave(DWORD dwID) const;
	SlaveSettings*		GetSlave(DWORD dwID);

	const MasterSettings*	GetMaster(DWORD dwID) const;
	MasterSettings*		GetMaster(DWORD dwID);

	const Connection*	GetConnection(DWORD dwID) const;
	Connection*		GetConnection(DWORD dwID);

	void	AddComputer(const  ComputerSettings& cs);
	DWORD	AddComputer(CString Name);

	void	AddCommutator(const  CommutatorSettings& cs);
	void	AddSlave(const  SlaveSettings& ss);
	void	AddMaster(const  MasterSettings& ms);
	void	AddConnection(const Connection& c);
#ifndef USE_AUTO_COMP2COMM
	void					AddComp2Comm(DWORD dwCommID, CString Name);
	void					AddComp2Comm(const Comp2Comm& cc);
	CString					GetComp2Comm(DWORD dwCommID) const;
#endif
	std::vector<CString>	GetAllComputers() const;
	
	std::set<DWORD>	GetAllConnections(DWORD dwID) const;
	
	DWORD			GetConnection(DWORD dwID, DWORD dwSlot) const;

	DWORD			GetFreeComputerSlot(DWORD dwID) const throw(ConnectionException);
	DWORD			GetFreeCommutatorSlot(DWORD dwID) const throw(ConnectionException);

	DWORD			GetCommutatorSlotNumber(DWORD dwID) const; 
	
	bool			IsAllSlavePTZ(DWORD dwComp, DWORD dwSlot, DWORD dwPTZ) const;
	bool			IsAllSlave(DWORD dwComp, DWORD dwSlot) const;

	bool			IsConnectSlave( DWORD dwID) const;

	void			DeleteConnection(DWORD dwID, DWORD dwSlot) ;
	void			ShiftCommSlot(DWORD dwID, DWORD dwSlot, bool bFirstSlot);

	void			SetDefaultSettings();
	void			ClearDatabase();

	DWORD			GetDefVideoStandatd() const { return VIDEO_STANDARD_1;}

	MasterSettings*	GetMasterFromCameraID(int nID);
	SlaveSettings*	GetSlaveFromCameraID(int nID);

	bool			IsCameraConnectedWithComputer(DWORD dwCameraID) const;
	bool			IsCameraConnectedWithCommutator(DWORD dwCameraID) const;
	bool			IsSlaveConnectedWithComputer(DWORD dwCameraID) const;

	std::pair<bool, CString> GetIPForCommutator() const;
	std::pair<bool, CString> GetIPForCamera( DWORD dwID ) const;
	void					 CreateReport( std::vector< LogicReport::Cluster>& Clusters ) const;

private:
	const Connection*	GetConnectionPtr(DWORD dwID, DWORD dwSlot) const;
	Connection*			GetConnectionPtr(DWORD dwID, DWORD dwSlot);
private:
#ifdef USE_OLD_FORMAT
	Project											m_Project;
#endif
	std::set<ComputerSettings>						m_Computers;
	std::set<CommutatorSettings>					m_Commutators;
	std::set<MasterSettings> 						m_Masters;
	std::set<SlaveSettings> 						m_Slaves;
	std::set<Connection>							m_Connections;
#ifndef USE_AUTO_COMP2COMM
	std::set<Comp2Comm>								m_Comp2Comm;
#endif
};



#endif // !defined(AFX_LOGICDB_H__684E5785_55A6_453D_BC23_1828C2DBCAFC__INCLUDED_)
