//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                               Copyright© ElVEES, 2005                                //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Функция для создания интерфейсов
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 26.09.2005
//                                                                                      //
//======================================================================================//
#ifndef _DEP_INTERFACE_7165149554842902_
#define _DEP_INTERFACE_7165149554842902_
//======================================================================================//
//                                 struct DepInterface                                  //
//======================================================================================//



namespace DepCfg
{

template<class T> struct ReleaseInterface
{
	void operator () (  T* p) 
	{
		if(p) p->Release();
	}
};

extern "C" bool WINAPI CreateDepInterface(const wchar_t *pName, void **ppInterface);

template< typename T >  boost::shared_ptr< T > CreateInternalInterface( const wchar_t* pIntID )
{
	T* pRawInt;
	bool bCreateRes = DepCfg::CreateDepInterface( pIntID, (void**) &pRawInt);
	if(!bCreateRes) throw std::runtime_error("Interface is not supported");
	return boost::shared_ptr< T> ( pRawInt, DepCfg::ReleaseInterface<T>() );
}


};

#endif // _DEP_INTERFACE_7165149554842902_