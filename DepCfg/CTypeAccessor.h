// Computers.h : Declaration of the CComputers

#pragma once

#include <atldbcli.h>
#include <atlbase.h>
#include <atlconv.h>
#include <atlstr.h>
#include  <string>
#include <vector>
#include <map>

// code generated on 3 июля 2006 г., 11:04
//extern std::wstring sConection;
template<typename PropType> class CTypeAccessor
{
public:
	/*LONG*/PropType m_Property;

	// The following wizard-generated data members contain status
	// values for the corresponding fields in the column map. You
	// can use these values to hold NULL values that the database
	// returns or to hold error information when the compiler returns
	// errors. See Field Status Data Members in Wizard-Generated
	// Accessors in the Visual C++ documentation for more information
	// on using these fields.
	// NOTE: You must initialize these fields before setting/inserting data!

	DBSTATUS m_dwPropertyStatus;

	// The following wizard-generated data members contain length
	// values for the corresponding fields in the column map.
	// NOTE: For variable-length columns, you must initialize these
	//       fields before setting/inserting data!


	DBLENGTH m_dwPropertyLength;


	void GetRowsetProperties(CDBPropSet* pPropSet)
	{
		pPropSet->AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		pPropSet->AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
		//propSet.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
	}

	void OpenDataSource(CSession cSession)
	{
		m_session = cSession;
	}

	HRESULT OpenDataSource(const std::wstring& sConection)
	{
		CDataSource _db;
		HRESULT hr = S_OK;

		//Конвертировать в Unicode строку
		CT2W psConvertToW( sConection.c_str());

		//#error Security Issue: The connection string may contain a password
		// The connection string below may contain plain text passwords and/or
		// other sensitive information. Please remove the #error after reviewing
		// the connection string for any security related issues. You may want to
		// store the password in some other form or use a different user authentication.
		hr = _db.OpenFromInitializationString(psConvertToW);
		if (FAILED(hr))
		{
		#ifdef _DEBUG
			AtlTraceErrorRecords(hr);
		#endif
		return hr;
		}
		return m_session.Open(_db);
		
	}

	void CloseDataSource()
	{
		m_session.Close();
	}

	operator const CSession&()
	{
		return m_session;
	}

	CSession m_session;

	// In order to fix several issues with some providers, the code below may bind
	// columns in a different order than reported by the provider

	BEGIN_COLUMN_MAP(CTypeAccessor)
		COLUMN_ENTRY_LENGTH_STATUS(1, m_Property, m_dwPropertyLength, m_dwPropertyStatus)
	END_COLUMN_MAP()
};

template<typename T> class CProperty : public CCommand<CAccessor<CTypeAccessor<T> > >
{
public:

	HRESULT DoCommand(LPCTSTR szSQL, bool bUpdate)
	{
		// Запросить настройки
		CDBPropSet propset(DBPROPSET_ROWSET);

		if(!bUpdate)
		{
			propset.AddProperty(DBPROP_IRowsetChange, true);
			propset.AddProperty(DBPROP_CANFETCHBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_CANSCROLLBACKWARDS, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_IRowsetChange, true, DBPROPOPTIONS_OPTIONAL);
			propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE);
		}


		GetRowsetProperties(&propset);

		// Последняя false означает что комманда ничего не возвращает и не надо пытаться сбиндить результат
		HRESULT hRes = Open(m_session, szSQL, &propset, NULL, DBGUID_DEFAULT, bUpdate);

		if(FAILED(hRes))
			AtlTraceErrorRecords(hRes);

		return hRes;
	}

	void CloseAll()
	{
		Close();
		ReleaseCommand();
		CloseDataSource();
	}
};


