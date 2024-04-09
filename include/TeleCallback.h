/********************************************************************
	created:	2004/06/07
	created:	7:6:2004   14:40
	filename: 	E:\Projects\DkClient2\TeleCallback.h
	file path:	E:\Projects\DkClient2
	file base:	TeleCallback
	file ext:	h
	author:		Kirill V. Lyadvinsly
	
	purpose:	
*********************************************************************/

#ifndef _TELECALLBACKH__
#define _TELECALLBACKH__

class ITeleCallback
{
public:
  virtual void OnCamera(LONG id,          // camera id
                        LPCWSTR state     // new state
                        ) =0;
  virtual void OnPTZ(LONG id,             // camera id
                     LPCWSTR state        // new state
                     ) =0;
  virtual void OnComputer(LPCWSTR name,   // computer name
                          LPCWSTR state   // new state
                          ) =0;  
};

#endif //_TELECALLBACKH__
