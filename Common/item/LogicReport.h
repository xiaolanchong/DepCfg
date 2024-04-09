//======================================================================================//
//                                       Orwell2K                                       //
//                                                                                      //
//                              Copyright by ElVEES, 2005                               //
//                                                                                      //
//--------------------------------------------------------------------------------------//
// Description:
//	Report from logic database
//
//--------------------------------------------------------------------------------------//
// Author : Eugene Gorbachev (Eugene.Gorbachev@biones.com)
// Date   : 18.04.2005
//                                                                                      //
//======================================================================================//
#ifndef _LOGIC_REPORT_3887452281331489_
#define _LOGIC_REPORT_3887452281331489_
//======================================================================================//
//                                  class LogicReport                                   //
//======================================================================================//

namespace LogicReport
{
	struct Camera
	{
		DWORD		nCameraID;
		CString		sIP;

		Camera( DWORD _id, CString _IP ):nCameraID(_id), sIP(_IP){}
	};

	struct Master : Camera
	{
		Master( DWORD _id, CString _IP ):Camera(_id, _IP){}
	};

	struct Slave: Camera
	{
		DWORD dwPTZDevice, dwRS485ID;

		Slave( DWORD _id, CString _IP, DWORD ptzid, DWORD rsid ):
			Camera(_id, _IP), 
			dwPTZDevice(ptzid), dwRS485ID(rsid){}
	};

	struct Cluster
	{
		CString					sName, sIP;
		std::vector< Master >	Masters;
		std::vector< Slave >	Slaves;

		Cluster( CString _Name, CString _IP ): sName(_Name), sIP(_IP){}
	};
}

#endif // _LOGIC_REPORT_3887452281331489_