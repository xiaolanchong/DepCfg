#include "stdafx.h"
#include "BridgeWrapper.h"
#include "Helper.cpp"
#include "Registry/Registry.h"


BridgeWrapper::BridgeWrapper(void)
{
}

BridgeWrapper::~BridgeWrapper(void)
{
}
HRESULT BridgeWrapper::CreateDB(LPCTSTR szDefaultConnect)
{
	//Создать строку соединения

	CString csConnect;
	if(szDefaultConnect == NULL)
		CRegistry::LoadString(REGKEY_APP, REGVALUE_DBSTRING, csConnect, szDefaultConnect);
	else
		csConnect = szDefaultConnect;
	
	//Создать интерфейс 
	HRESULT hRes = m_db.OpenFromInitializationString((LPWSTR)(LPCWSTR) csConnect);
	if(SUCCEEDED(hRes))
	{
		hRes = m_session.Open(m_db);
		if(FAILED(hRes))
		{

		}
	}
	else
	{

	}
	return hRes;

}

long BridgeWrapper::LoadValue(LPCTSTR szNameTable, LPCTSTR szNameField, std::multimap<int, unsigned int>& nArrValue )
{
	CProperty<unsigned int> propset;
	std::wstring szSQLQuery = Formats(_T("SELECT %s FROM %s"), szNameField, szNameTable);
	propset.OpenDataSource(m_session);
	HRESULT hRes = propset.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		while(propset.MoveNext() == S_OK)
			if( propset.m_dwPropertyStatus == DBSTATUS_S_OK)
			{
				int_Pair nPair;
				nPair.second = propset.m_Property;
				szSQLQuery = Formats(_T("SELECT ClusterID FROM Camera"));
				CProperty<int> propsetID;
				propsetID.OpenDataSource(m_session);
				if((SUCCEEDED(propsetID.DoCommand(szSQLQuery.c_str(), TRUE))) && (SUCCEEDED(propsetID.MoveNext())) && (propsetID.m_dwPropertyStatus == DBSTATUS_S_OK))
				{
					nPair.first = propsetID.m_Property;
					nArrValue.insert(nPair);
				}
				propsetID.CloseAll();
			}
	}

	propset.CloseAll();
	return hRes;
	
}

long BridgeWrapper::LoadValue(LPCTSTR szNameTable, LPCTSTR szNameField, std::vector<std::wstring>& sArrValue  )
{
	CProperty<TCHAR[255]> propset;
	std::wstring szSQLQuery = Formats(_T("SELECT %s FROM %s"), szNameField, szNameTable);
	propset.OpenDataSource(m_session);
	HRESULT hRes = propset.DoCommand(szSQLQuery.c_str(), TRUE);
	if(SUCCEEDED(hRes))
	{
		while(propset.MoveNext() == S_OK)
			if( propset.m_dwPropertyStatus == DBSTATUS_S_OK)
				sArrValue.push_back(propset.m_Property);
	}

	propset.CloseAll();
	return hRes;
}

long BridgeWrapper::CreateDBInter()
{
	//Сформировать строку соединения
	CString dbString;
	dbString.Format(L"Provider=SQLOLEDB.1;Persist Security Info=True;User ID=depcfg;Initial Catalog=depcfg;Data Source=HAMMER\\SQLEXPRESS;pwd=depcfg");

	//Создать интерфейс 
	long dbbRes = CreateDBBridge2(dbString,  &iDBBridge);

	return dbbRes;
}
LPDWORD BridgeWrapper::GetIDList(IN DBB_SECTION section)
{
	DWORD numOfElements;
	DWORD* pBuffer;
	long dbRes = iDBBridge->GetComponentList( section, &numOfElements, &pBuffer);
	
	return pBuffer;
}
