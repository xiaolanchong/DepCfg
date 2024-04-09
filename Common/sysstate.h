// SysState.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ELVEES_SYSTEMSTATE_H__INCLUDED_)
#define _ELVEES_SYSTEMSTATE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

///////////////////////////////////////////////////////////////////////////////
// Device states
///////////////////////////////////////////////////////////////////////////////

typedef enum
{
	ECameraUndefined = -1,
	ECameraMaster = 0,
	ECameraSlave = 1
}
ECamType;

typedef enum
{
	ECameraNoError = 0, // ok
	ECameraStopped = 1, // temporarily not work, restart pending
	ECameraInit = 2, // temporarily not work, restart pending
	ECameraError = 3, // camera thread can't start (mostly)
	ECameraNoImage = 4, // no image from video server or no signal from camera
	ECameraBlind = 5  // some kind of violation in environment
}
ECamState;

typedef enum
{
	EPTZNoError = 0, // ok
	EPTZNotConnected = 1, // Not connected to video server
	EPTZFailed = 2, // Last operation failed
	EPTZTimeout = 3  // Last operation failed by timeout
}
EPTZState;

///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////

typedef struct tagCameraState
{
	DWORD csSize;			// = sizeof(CameraState)
	DWORD csCameraID;		// cameras ID in DB
	ECamType  csCamType;	// master or slave or ...
	ECamState csCamState;	// see above
	EPTZState csPTZState;	// see above

} CameraState, * PCameraState;

typedef struct tagComputerState
{
	DWORD sSize;    // = sizeof(ComputerState)
	DWORD sCamera;  // number of cameras
	DWORD sBattery; // 0 - OnLine, 1 - Running on battery
	char  sHostName[16]; // MAX_COMPUTERNAME_LENGTH + 1

} ComputerState, * PComputerState;

///////////////////////////////////////////////////////////////////////////////
// Memory map and example
///////////////////////////////////////////////////////////////////////////////

//	 -----------------------------------------
//	| ComputerState || Camera1 | .. | CameraN |
//	 -----------------------------------------

//	handle input state buffer example
//	BYTE *pBuffer;

//	try
//	{
//		PComputerState pState  = PComputerState(pBuffer);
//		
//		// Computer IP
//		//
//
//		in_addr inAddr;
//		inAddr.s_addr = pState->sMCAddr;
//
//		TCHAR stAddr[16];
//		wsprintf(stAddr, TEXT("%ld.%ld.%ld.%ld"),
//			inAddr.s_net, inAddr.s_host,
//			inAddr.s_lh, inAddr.s_impno);
//
//		_tprintf(TEXT("State message from %s\n")
//					TEXT("\tCamNumber: %ld\n")
//					TEXT("\tPowerStatus: %s\n"),
//				stAddr,
//				pState->sCamera,
//				pState->sBattery ? TEXT("Battery") : TEXT("On-Line") );
//
//		// Camera state
//		//
//		
//		if(pState->sCamera > 0)
//		{
//			PBYTE pCStateStart = &pBuffer[pState->csSize];
//			
//			PCameraState pCState = PCameraState(pCStateStart);
//			long lCamSize        = pCState->csSize;
//		
//			for(int i=0; i<pState->sCamera; i++)
//			{
//				pCState = PCameraState(pCStateStart + lCamSize*i);
//
//				pCState->csCameraID
//				if(pCState->csCamType == ECameraMaster)
//				{
//					_tprintf(TEXT("Master(%ld): %s\n"),
//						pCState->csCameraID,
//						pCState->csCamState == ECameraNoError ? TEXT("OK") :
//						pCState->csCamState == ECameraInitFailed ? TEXT("Init failed") :
//						pCState->csCamState == ECameraError ? TEXT("Error") :
//						pCState->csCamState == ECameraNoImage ? TEXT("No Image") :
//						pCState->csCamState == ECameraBlind ? TEXT("Blind") : TEXT("Unknown state") );
//				}
//				else if(pCState->csCamType == ECameraSlave)
//				{
//					_tprintf(TEXT("Slave(%ld): %s PTZ: %s\n"),
//						pCState->csCameraID,
//						pCState->csCamState == ECameraNoError ? TEXT("OK") :
//						pCState->csCamState == ECameraInitFailed ? TEXT("Init failed") :
//						pCState->csCamState == ECameraError ? TEXT("Error") :
//						pCState->csCamState == ECameraNoImage ? TEXT("No Image") :
//						pCState->csCamState == ECameraBlind ? TEXT("Blind") : TEXT("Unknown state"),
//							
//						pCState->csPTZState == EPTZNoError ? TEXT("OK") :
//						pCState->csPTZState == EPTZNotConnected ? TEXT("Not Connected") :
//						pCState->csPTZState == EPTZFailed ? TEXT("Failed") :
//						pCState->csPTZState == EPTZTimeout	 ? TEXT("Timeout") : TEXT("Unknown state") );
//				}
//				else
//				{
//					_tprintf(TEXT("Unknown camera(%ld) type = %ld\n"),
//						pCState->csCameraID,
//						pCState->csCamType);
//				}
//			}
//		}
//	}
//	catch(...)
//	{
//	}

#endif // !defined(_ELVEES_SYSTEMSTATE_H__INCLUDED_)