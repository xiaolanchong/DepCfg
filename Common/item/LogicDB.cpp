// LogicDB.cpp: implementation of the CLogicDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
//#include "depcfg.h"
#include "LogicDB.h"
#include <map>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DWORD DbItem::m_dwCurID = 0;

bool CLogicDB::IsFreeSlot(DWORD dwID, DWORD dwSlot) const
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	for(; itr != m_Connections.end() ; ++itr)
	{
		if ( ( itr->First == dwID && itr->FirstSlot == dwSlot ) ||
			 ( itr->Second == dwID && itr->SecondSlot == dwSlot ) )
			return false;
	}	
	return true;
}

DWORD		CLogicDB::GetCameraID() const
{
	std::set<DWORD> IDSet;
	std::set<SlaveSettings>::const_iterator itr = m_Slaves.begin();
	for(; itr != m_Slaves.end() ; ++itr)
	{
		IDSet.insert( itr->m_dwCameraID.Get() );
	}

	std::set<MasterSettings>::const_iterator itr2 = m_Masters.begin();
	for(; itr2 != m_Masters.end() ; ++itr2)
	{
		IDSet.insert( itr2->m_dwCameraID.Get() );
	}

	for( int i =1 ; ; ++i )
	{
		if( IDSet.find(i) ==  IDSet.end() ) return i;
	}
	return -1;
}

void	CLogicDB::DeleteComputer(DWORD dwID)
{
	m_Computers.erase( ComputerSettings(dwID) );
//	DispatchItem( dwID );
}

void	CLogicDB::DeleteCommutator(DWORD dwID)
{
	m_Commutators.erase( CommutatorSettings(dwID) );
//	DispatchItem( dwID );
}

void	CLogicDB::DeleteMaster(DWORD dwID)
{
	m_Masters.erase( MasterSettings(dwID) );
//	DispatchItem( dwID );
}

void	CLogicDB::DeleteSlave(DWORD dwID)
{
	m_Slaves.erase( SlaveSettings(dwID) );
//	DispatchItem( dwID );
}

void	CLogicDB::DeleteConnection(DWORD dwID)
{
	std::set<Connection>::iterator itr5 = m_Connections.find( dwID );
	if( itr5 != m_Connections.end()) 
	{
		Connection cnt( *itr5 );
		m_Connections.erase( itr5 );

		const CommutatorSettings* cms;
		if( cms = GetCommutator( cnt.First ) )
		{
			ShiftCommSlot( cms->GetID(), cnt.FirstSlot, true );
		}
		else if( cms = GetCommutator( cnt.Second ) )
		{
			ShiftCommSlot( cms->GetID(), cnt.SecondSlot, false );
		}		
	}
}



const ComputerSettings*	CLogicDB::GetComputer(DWORD dwID) const
{
	std::set<ComputerSettings>::const_iterator itr = m_Computers.find( ComputerSettings(dwID) );
	return itr != m_Computers.end() ? &(*itr) : 0;
}

ComputerSettings*		CLogicDB::GetComputer(DWORD dwID)
{
	std::set<ComputerSettings>::iterator itr = m_Computers.find( ComputerSettings(dwID) );
	return itr != m_Computers.end() ? &(*itr) : 0;
}

const CommutatorSettings*	CLogicDB::GetCommutator(DWORD dwID) const
{
	std::set<CommutatorSettings>::const_iterator itr = m_Commutators.find( CommutatorSettings(dwID) );
	return itr != m_Commutators.end() ? &(*itr) : 0;
}

CommutatorSettings*		CLogicDB::GetCommutator(DWORD dwID)
{
	std::set<CommutatorSettings>::iterator itr = m_Commutators.find( CommutatorSettings(dwID) );
	return itr != m_Commutators.end() ? &(*itr) : 0;
}

const SlaveSettings*	CLogicDB::GetSlave(DWORD dwID) const
{
	std::set<SlaveSettings>::const_iterator itr = m_Slaves.find( SlaveSettings(dwID) );
	return itr != m_Slaves.end() ? &(*itr) : 0;
}

SlaveSettings*		CLogicDB::GetSlave(DWORD dwID)
{
	std::set<SlaveSettings>::iterator itr = m_Slaves.find( SlaveSettings(dwID) );
	return itr != m_Slaves.end() ? &(*itr) : 0;
}

const MasterSettings*	CLogicDB::GetMaster(DWORD dwID) const
{
	std::set<MasterSettings>::const_iterator itr = m_Masters.find( MasterSettings(dwID) );
	return itr != m_Masters.end() ? &(*itr) : 0;
}

MasterSettings*		CLogicDB::GetMaster(DWORD dwID)
{
	std::set<MasterSettings>::iterator itr = m_Masters.find( MasterSettings(dwID) );
	return itr != m_Masters.end() ? &(*itr) : 0;
}

const Connection*	CLogicDB::GetConnection(DWORD dwID) const
{
	std::set<Connection>::const_iterator itr = m_Connections.find( Connection(dwID) );
	return itr != m_Connections.end() ? &(*itr) : 0;
}

Connection*		CLogicDB::GetConnection(DWORD dwID)
{
	std::set<Connection>::iterator itr = m_Connections.find( Connection(dwID) );
	return itr != m_Connections.end() ? &(*itr) : 0;
}

void	CLogicDB::AddComputer(const  ComputerSettings& cs)
{
	m_Computers.insert( cs );
}

void	CLogicDB::AddCommutator(const  CommutatorSettings& cs)
{
	m_Commutators.insert( cs );
}


void	CLogicDB::AddSlave(const  SlaveSettings& ss)
{
	m_Slaves.insert( ss );
}

void	CLogicDB::AddMaster(const  MasterSettings& ms)
{
	m_Masters.insert( ms );
}

void	CLogicDB::AddConnection(const Connection& c)
{
	m_Connections.insert( c );
}

std::vector<CString>	CLogicDB::GetAllComputers() const
{
	std::vector<CString> v;
	std::set<ComputerSettings>::const_iterator itr = m_Computers.begin();
	for(; itr != m_Computers.end() ; ++itr)
	{
		if ( itr->m_strIP.IsSet() )
		v.push_back( itr->m_strIP.Get() );
	}	
	return v;
}
#ifndef USE_AUTO_COMP2COMM
void					CLogicDB::AddComp2Comm(DWORD dwCommID, CString Name)
{
	if(Name.IsEmpty())
	{
		std::set<Comp2Comm>::iterator itr = m_Comp2Comm.begin();
		for(; itr != m_Comp2Comm.end() ; ++itr)
		{
			if(itr->m_dwComm == dwCommID)
			{
				m_Comp2Comm.erase(itr);
				return;
			}
		}
	}
	std::set<ComputerSettings>::const_iterator itr = m_Computers.begin();
	for(; itr != m_Computers.end() ; ++itr)
	{
		if ( itr->m_strIP.IsSet() && itr->m_strIP.Get() == Name )
		{
			
			std::set<Comp2Comm>::iterator itr2 = m_Comp2Comm.find( Comp2Comm ( 0, dwCommID ) );
		
			if(  itr2 != m_Comp2Comm.end())
				itr2->m_dwComp = itr->GetID();
			else
				m_Comp2Comm.insert( Comp2Comm ( itr->GetID(), dwCommID ) );
			return;
		}
	}
}

void					CLogicDB::AddComp2Comm(const Comp2Comm& cc)
{
	m_Comp2Comm.insert( cc );
}

CString					CLogicDB::GetComp2Comm(DWORD dwCommID) const
{
	std::set<Comp2Comm>::const_iterator itr = m_Comp2Comm.begin();
	for(; itr != m_Comp2Comm.end() ; ++itr)
	{
		if ( itr->m_dwComm ==  dwCommID )
		{
			const ComputerSettings*  cs = GetComputer( itr->m_dwComp );
			
			return cs->m_strIP.IsSet() ? cs->m_strIP.Get() : CString();
		}
	}
	return CString();
}
#endif

std::set<DWORD>					CLogicDB::GetAllConnections(DWORD dwID) const 
{
	std::set<DWORD>	s;

	std::set<Connection>::const_iterator itr = m_Connections.begin();
	for(; itr != m_Connections.end() ; ++itr)
	{
		if ( itr->First ==  dwID || itr->Second ==  dwID )
		{
/*			s.insert( itr->Second );
		}
		else if (itr->Second ==  dwID )
		{
			s.insert( itr->First );*/
			s.insert( itr->GetID() );
		}
	}
	return s;
}

void	CLogicDB::DeleteItems(const std::set<DWORD> & s)
{
	std::set<DWORD>::const_iterator itr = s.begin();
	for( ; itr != s.end(); ++itr )
	{
		std::set<ComputerSettings>::iterator itr1 = m_Computers.find( ComputerSettings( *itr ));
		if( itr1 != m_Computers.end()) m_Computers.erase( itr1 )  ;

		std::set<SlaveSettings>::iterator itr2 = m_Slaves.find( SlaveSettings( *itr ) );
		if( itr2 != m_Slaves.end()) m_Slaves.erase( itr2 )  ;

		std::set<MasterSettings>::iterator itr3 = m_Masters.find( MasterSettings( *itr ) );
		if( itr3 != m_Masters.end()) m_Masters.erase( itr3 )  ;

		std::set<CommutatorSettings>::iterator itr4 = m_Commutators.find( CommutatorSettings( *itr ) );
		if( itr4 != m_Commutators.end()) m_Commutators.erase( itr4 );

		std::set<Connection>::iterator itr5 = m_Connections.find( Connection( *itr ) );
		if( itr5 != m_Connections.end()) DeleteConnection( itr5->GetID() ) ;//m_Connections.erase( itr5 );
	}
#ifndef USE_AUTO_COMP2COMM
	std::set<Comp2Comm>::iterator itr2 = m_Comp2Comm.begin();
	for(; itr2 != m_Comp2Comm.end();)
	{
		if(  s.find( itr2->m_dwComp ) != s.end()  || s.find( itr2->m_dwComm ) != s.end())
		{
			m_Comp2Comm.erase( itr2++ );
		}
		else ++itr2;
	}
#endif
}

DWORD	CLogicDB::GetConnection(DWORD dwID, DWORD dwSlot) const
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	for( ; itr != m_Connections.end(); ++itr )
	{
		if(itr->First == dwID && dwSlot == itr->FirstSlot) return itr->Second;
		else if(itr->Second == dwID && dwSlot == itr->SecondSlot) return itr->First;
	}
	throw ConnectionException("");
	return 0;
}

DWORD	CLogicDB::AddComputer(CString Name)
{
	std::set<ComputerSettings>::const_iterator itr = m_Computers.begin();
	for(; itr != m_Computers.end() ; ++itr)
	{
		if ( itr->m_strIP.IsSet() && itr->m_strIP.Get() == Name )
		{
			return itr->GetID();
		}
	}

	ComputerSettings cs;
	cs.m_strIP = Name;
	AddComputer( cs );
	return cs.GetID();
}

DWORD		CLogicDB::GetFreeComputerSlot(DWORD dwID) const throw(ConnectionException)
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	std::set<DWORD> Slots;
	for( ; itr != m_Connections.end(); ++itr )
	{
		if(itr->First == dwID) Slots.insert( itr->FirstSlot );
		else if(itr->Second == dwID ) Slots.insert( itr->SecondSlot );
	}

	for(int i=0; i < c_MaxConputerSlots; ++i)
	{
		if( Slots.find( i ) == Slots.end()) return i;
	}

	throw ConnectionException("");
	return 0;	
}

DWORD		CLogicDB::GetFreeCommutatorSlot(DWORD dwID) const throw(ConnectionException)
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	std::set<DWORD> Slots;
	for( ; itr != m_Connections.end(); ++itr )
	{
		if(itr->First == dwID) Slots.insert( itr->FirstSlot );
		else if(itr->Second == dwID ) Slots.insert( itr->SecondSlot );
	}

	DWORD dwAllSlots = GetCommutatorSlotNumber( dwID );
	for(size_t i=0; i < dwAllSlots; ++i)
	{
		if( Slots.find( i ) == Slots.end()) return i;
	}

	throw ConnectionException("");
	return 0;
}

DWORD			CLogicDB::GetCommutatorSlotNumber( DWORD dwID ) const 
{
//	static slots = GetSlot(IDS_REG_SLOT);
//	return 7;
//	const CommutatorSettings* cs = GetCommutator( dwID );
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	DWORD dwSlots = 1; // Free slot
	for( ; itr != m_Connections.end(); ++itr )
	{
		if(itr->First == dwID || itr->Second == dwID) 
		{	
			++dwSlots;
		}		
	}
	return dwSlots;
}

bool			CLogicDB::IsAllSlavePTZ(DWORD dwComp, DWORD dwSlot, DWORD dwPTZ) const
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	bool bSlave = false;
	for( ; itr != m_Connections.end(); ++itr )
	{
		const SlaveSettings* ss = 0;
		if(itr->First == dwComp && itr->FirstSlot == dwSlot) 
		{
			ss = GetSlave( itr->Second );
		}
		else if(itr->Second == dwComp  && itr->SecondSlot == dwSlot) 
		{
			ss = GetSlave( itr->First );	
		}
		else continue;
		if(!ss) return false;
		if(!ss->m_dwPTZDevice.IsSet() ) return false;
		if( ss->m_dwPTZDevice.Get() != dwPTZ) return false;
		bSlave = true; 
	}
	return bSlave;
}

bool			CLogicDB::IsAllSlave(DWORD dwComp, DWORD dwSlot) const
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	bool bSlave = false;
	for( ; itr != m_Connections.end(); ++itr )
	{
		const SlaveSettings* ss = 0;
		if(itr->First == dwComp && itr->FirstSlot == dwSlot && itr->SecondSlot == 1) 
		{
			ss = GetSlave( itr->Second );
		}
		else if(itr->Second == dwComp  && itr->SecondSlot == dwSlot && itr->FirstSlot == 1) 
		{
			ss = GetSlave( itr->First );				
		}
		else continue;
		if(!ss) return false;
		bSlave = true;
	}
	return bSlave;
}

bool			CLogicDB::IsConnectSlave( DWORD dwID) const
{
	std::set<Connection>::const_iterator itr = m_Connections.begin();
	for( ; itr != m_Connections.end(); ++itr )
	{
		const SlaveSettings* ss = 0;
		if(itr->First == dwID) 
		{
			ss = GetSlave( itr->Second );
		}
		else if(itr->Second == dwID )
		{
			ss = GetSlave( itr->First );				
		}
		if(ss) return true;
	}
	return false;
}

void	CLogicDB::DeleteConnection(DWORD dwID, DWORD dwSlot) 
{
	std::set<Connection>::iterator itr = m_Connections.begin();
	for( ; itr != m_Connections.end();  )
	{
		if(itr->First == dwID && dwSlot == itr->FirstSlot) m_Connections.erase(itr++);
		else if(itr->Second == dwID && dwSlot == itr->SecondSlot) m_Connections.erase(itr++);
		else ++itr;
	}
}

void		CLogicDB::ShiftCommSlot(DWORD dwID, DWORD dwSlot, bool bFirstSlot)
{
	const CommutatorSettings* cms = GetCommutator(dwID);
	std::set<Connection>::iterator itr = m_Connections.begin();
	for( ; itr != m_Connections.end(); ++itr )
	{

		if((cms = GetCommutator( itr->First ))	&& 
			itr->First == dwID					&& 
			itr->FirstSlot > dwSlot ) 
		{
			--itr->FirstSlot;
		}
		else if((cms = GetCommutator( itr->Second ))	&& 
				itr->Second == dwID						&& 
				itr->SecondSlot > dwSlot ) 
		{
			--itr->SecondSlot;
		}	
	}
}

void		CLogicDB::SetDefaultSettings()
{
#ifdef USE_OLD_FORMAT
	m_Project.m_dwVideoStandard = GetDefVideoStandatd();
#endif
}

void		CLogicDB::ClearDatabase()
{
	m_Computers.clear();
	m_Commutators.clear();
	m_Slaves.clear();
	m_Masters.clear();
	m_Connections.clear();
#ifndef USE_AUTO_COMP2COMM
	m_Comp2Comm.clear();
#endif	
#ifdef USE_OLD_FORMAT
	m_Project.m_strSQLServer.Reset();
	m_Project.m_strDBName.Reset();
	m_Project.m_strSharedDir.Reset();
	m_Project.m_dwVideoStandard.Reset();
#endif
}

MasterSettings*	CLogicDB::GetMasterFromCameraID(int nID)
{
	std::set<MasterSettings>::iterator itr = m_Masters.begin();
	for( ; itr != m_Masters.end(); ++itr  )
	{
		if( itr->m_dwCameraID.IsSet() &&  nID == itr->m_dwCameraID.Get())
			return &(*itr);
	}	
	return 0;
}

SlaveSettings*	CLogicDB::GetSlaveFromCameraID(int nID)
{
	std::set<SlaveSettings>::iterator itr = m_Slaves.begin();
	for( ; itr != m_Slaves.end(); ++itr  )
	{
		if( itr->m_dwCameraID.IsSet() &&  nID == itr->m_dwCameraID.Get())
			return &(*itr);
	}	
	return 0;	
}

template<typename T1, typename T2>
 struct Compare2nd
 {
	 bool operator ()(	const std::pair<T1, T2> & p1, 
						const std::pair<T1, T2> & p2 )
	 {
		 return p1.second < p2.second;
	 }
 };

std::pair<bool, CString> CLogicDB::GetIPForCommutator(  ) const
{
	std::map< DWORD, DWORD>		CompMasters; 
	std::set<Connection>::const_iterator itCon = m_Connections.begin();
	std::set<ComputerSettings>::const_iterator itComp = m_Computers.begin();
	for( ; itComp != m_Computers.end(); ++ itComp)
	{
		CompMasters[ itComp->GetID() ] = 0;
	}
	for( ; itCon != m_Connections.end(); ++itCon )
	{
		const ComputerSettings* cs = GetComputer(itCon->First);
		const MasterSettings* ms = GetMaster(itCon->Second);
		if( cs && ms )
		{
			++CompMasters[ cs->GetID() ];
		}
		else
		{
			const ComputerSettings* cs = GetComputer(itCon->Second);
			const MasterSettings* ms = GetMaster(itCon->First);
			if( cs && ms )
			{
				++CompMasters[ cs->GetID() ];
			}
		}
	}
	std::map< DWORD, DWORD>::iterator itMax = 
		std::max_element( CompMasters.begin(), CompMasters.end(), Compare2nd<DWORD,DWORD>() );
	if( itMax == CompMasters.end() )
	{
		return std::make_pair( false, CString() );
	}
	else
	{
		DWORD dwID = itMax->first;
		const ComputerSettings* cs = GetComputer(dwID);
		ASSERT(cs);
		return std::make_pair( true, cs->m_strIP.IsSet()? cs->m_strIP.Get() : CString() );
	}
}

void	CLogicDB::CreateReport( std::vector< LogicReport::Cluster>& Clusters ) const
{
	Clusters.clear();
	std::map<DWORD, size_t>	CommIndex;
	std::set<CommutatorSettings>::const_iterator itComm = m_Commutators.begin();
	for( size_t i=0; itComm != m_Commutators.end(); ++ itComm, ++i)
	{
		CString sName = itComm->m_strName.IsSet() ? itComm->m_strName.Get() : CString();
		std::pair< bool, CString > pp = GetIPForCommutator();
		CString sIP = pp.first ? pp.second : CString();

		Clusters.push_back( LogicReport::Cluster( sName, sIP ) );
		CommIndex.insert( std::make_pair( itComm->GetID(), i ) );
	}

	std::set<Connection>::const_iterator itCon = m_Connections.begin();
	for( ; itCon != m_Connections.end(); ++ itCon)
	{
		DWORD dwComm = itCon->First, dwCamera = itCon->Second;
		const CommutatorSettings* cs = 0;
		cs = GetCommutator( dwComm );
		if( !cs )
		{
			dwComm = itCon->Second, dwCamera = itCon->First;
		}
		cs = GetCommutator( dwComm );
		if( cs )
		{
			const SlaveSettings*  ss = GetSlave(dwCamera);
			const MasterSettings* ms = GetMaster(dwCamera);
			if( ss )
			{
				std::map<DWORD, size_t>::const_iterator it = CommIndex.find( cs->GetID() );
				if( it != CommIndex.end() )
				{
					try
					{
					std::pair<bool, CString> pp = GetIPForCamera(  ss->GetID() );
					Clusters[ it->second ].Slaves.push_back( 
						LogicReport::Slave( ss->m_dwCameraID.Get(), pp.first ? pp.second : CString(), 
											ss->m_dwPTZDevice.Get(), ss->m_dwRS485ID.Get()) );
					}
					catch( PropertyException )
					{
						ASSERT(FALSE);
					}
				}
				else
				{
					ASSERT(FALSE);
				}
			}
			else if( ms )
			{
				std::map<DWORD, size_t>::const_iterator it = CommIndex.find( cs->GetID() );
				if( it != CommIndex.end() )
				{
					try
					{
						std::pair<bool, CString> pp = GetIPForCamera(  ms->GetID() );
						Clusters[ it->second ].Masters.push_back( 
							LogicReport::Master( ms->m_dwCameraID.Get(), pp.first ? pp.second : CString() ));
					}
					catch( PropertyException )
					{
						ASSERT(FALSE);
					}
				}
				else
				{
					ASSERT(FALSE);
				}
			}
		}
		cs = cs ? cs : GetCommutator( itCon->Second );

	}
}

std::pair<bool, CString> CLogicDB::GetIPForCamera( DWORD dwID ) const
{
	try
	{
		DWORD dwCompID = GetConnection( dwID, 0 );
		const ComputerSettings* cs = GetComputer( dwCompID );
		if( cs )
		{
			return std::make_pair( true, cs->m_strIP.IsSet() ? cs->m_strIP.Get() : CString() );
		}
		else
		return std::make_pair( false, CString() );
	}
	catch( ConnectionException )
	{
		return std::make_pair( false, CString() );
	}
}
/*
const Connection*	CLogicDB::GetConnectionPtr(DWORD dwID, DWORD dwSlot) const
try
{
	return GetConnection( GetConnection( dwID, dwSlot ) );
}
catch ( ConnectionException ) 
{
	return NULL;
};

Connection*		CLogicDB::GetConnectionPtr(DWORD dwID, DWORD dwSlot)
try
{
	return GetConnection( GetConnection( dwID, dwSlot ) );
}
catch ( ConnectionException ) 
{
	return NULL;
};*/

bool	CLogicDB::IsCameraConnectedWithComputer(DWORD dwCameraID) const
try
{
	const ComputerSettings* cs = GetComputer( GetConnection( dwCameraID, c_CompSignalSlot ) );
	return cs != NULL;
}
catch ( ConnectionException ) 
{
	return false;
}

bool	CLogicDB::IsCameraConnectedWithCommutator(DWORD dwCameraID) const
try
{
	bool bSlave = GetSlave( dwCameraID ) != 0;
	const CommutatorSettings* cs = GetCommutator( GetConnection( dwCameraID, bSlave ? c_CommSlaveSlot : c_CommMasterSlot ) );
	return cs != NULL;
}
catch ( ConnectionException ) 
{
	return false;
}

bool	CLogicDB::IsSlaveConnectedWithComputer(DWORD dwCameraID) const
try
{
	const ComputerSettings* cs = GetComputer( GetConnection( dwCameraID, c_CompControlSlot ) );
	return cs != NULL;
}
catch ( ConnectionException ) 
{
	return false;
}