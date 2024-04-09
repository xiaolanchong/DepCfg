#pragma once
#include "CTypeAccessor.h"


class CIDBBridge;

typedef std::pair<int, unsigned int> int_Pair;
#define REGKEY_APP	_T("HKLM\\SOFTWARE\\ElVEES\\Common")
#define REGVALUE_DBSTRING _T("DbInitString") 

class BridgeWrapper
{
public:
	BridgeWrapper(void);

	//Создать соединение с бд
	HRESULT CreateDB(LPCTSTR szDefaultConnect);

	//Загрузаить поле unsigned int
	long LoadValue(LPCTSTR szNameTable, LPCTSTR szNameField, std::multimap<int, unsigned int>& nArrValue  );
	
	//Загрузит поле типа string
	long LoadValue(LPCTSTR szNameTable, LPCTSTR szNameField, std::vector<std::wstring>& sValue );
	

	//////////////////////////////////////////////////////////////////////////
	long CreateDBInter();
	LPDWORD GetIDList(IN DBB_SECTION section);

public:
	~BridgeWrapper(void);
public:
	//
	CDataSource m_db;
	//
	CSession m_session;
	//
	CIDBBridge* iDBBridge;

};
