#ifndef __DYNAMICVISA_HEADER__
#define __DYNAMICVISA_HEADER__
//
// DynamicVisa.h
// This header file contains the DynamicVisa class definition
// and typedefs for all the VISA functions.
//

#define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <QLibrary>
#include "visa.h"

//
// Typedef all the VISA functions for convenience...
//
typedef ViStatus (_VI_FUNC _VI_PTR ViOpenDefaultRM) (ViPSession vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViParseRsrcEx)   (ViSession rmSesn, ViRsrc rsrcName, ViPUInt16 intfType, ViPUInt16 intfNum, ViChar _VI_FAR rsrcClass[], ViChar _VI_FAR expandedUnaliasedName[], ViChar _VI_FAR aliasIfExists[]);
typedef ViStatus (_VI_FUNC _VI_PTR ViOpen)          (ViSession sesn, ViRsrc name, ViAccessMode mode, ViUInt32 timeout, ViPSession vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViFindRsrc)      (ViSession sesn, ViString expr, ViPFindList vi, ViPUInt32 retCnt, ViChar _VI_FAR desc[]);
typedef ViStatus (_VI_FUNC _VI_PTR ViFindNext)      (ViFindList vi, ViChar _VI_FAR desc[]);
typedef ViStatus (_VI_FUNC _VI_PTR ViParseRsrc)     (ViSession rmSesn, ViRsrc rsrcName, ViPUInt16 intfType, ViPUInt16 intfNum);
typedef ViStatus (_VI_FUNC _VI_PTR ViClose)         (ViObject vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViSetAttribute)  (ViObject vi, ViAttr attrName, ViAttrState attrValue);
typedef ViStatus (_VI_FUNC _VI_PTR ViGetAttribute)  (ViObject vi, ViAttr attrName, void _VI_PTR attrValue);
typedef ViStatus (_VI_FUNC _VI_PTR ViStatusDesc)    (ViObject vi, ViStatus status, ViChar _VI_FAR desc[]);
typedef ViStatus (_VI_FUNC _VI_PTR ViTerminate)     (ViObject vi, ViUInt16 degree, ViJobId jobId);
typedef ViStatus (_VI_FUNC _VI_PTR ViLock)          (ViSession vi, ViAccessMode lockType, ViUInt32 timeout, ViKeyId requestedKey, ViChar _VI_FAR accessKey[]);
typedef ViStatus (_VI_FUNC _VI_PTR ViUnlock)        (ViSession vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViEnableEvent)   (ViSession vi, ViEventType eventType, ViUInt16 mechanism, ViEventFilter context);
typedef ViStatus (_VI_FUNC _VI_PTR ViDisableEvent)  (ViSession vi, ViEventType eventType, ViUInt16 mechanism);
typedef ViStatus (_VI_FUNC _VI_PTR ViDiscardEvents) (ViSession vi, ViEventType eventType, ViUInt16 mechanism);
typedef ViStatus (_VI_FUNC _VI_PTR ViWaitOnEvent)   (ViSession vi, ViEventType inEventType, ViUInt32 timeout, ViPEventType outEventType, ViPEvent outContext);
typedef ViStatus (_VI_FUNC _VI_PTR ViInstallHandler)(ViSession vi, ViEventType eventType, ViHndlr handler, ViAddr userHandle);
typedef ViStatus (_VI_FUNC _VI_PTR ViUninstallHandler)(ViSession vi, ViEventType eventType, ViHndlr handler, ViAddr userHandle);
typedef ViStatus (_VI_FUNC _VI_PTR ViRead)          (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViReadAsync)     (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPJobId  jobId);
typedef ViStatus (_VI_FUNC _VI_PTR ViReadToFile)    (ViSession vi, ViConstString filename, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViWrite)         (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViWriteAsync)    (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPJobId  jobId);
typedef ViStatus (_VI_FUNC _VI_PTR ViWriteFromFile)  (ViSession vi, ViConstString filename, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViAssertTrigger) (ViSession vi, ViUInt16 protocol);
typedef ViStatus (_VI_FUNC _VI_PTR ViReadSTB)       (ViSession vi, ViPUInt16 status);
typedef ViStatus (_VI_FUNC _VI_PTR ViClear)         (ViSession vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViSetBuf)        (ViSession vi, ViUInt16 mask, ViUInt32 size);
typedef ViStatus (_VI_FUNC _VI_PTR ViFlush)         (ViSession vi, ViUInt16 mask);
typedef ViStatus (_VI_FUNC _VI_PTR ViBufWrite)      (ViSession vi, ViBuf  buf, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViBufRead)       (ViSession vi, ViPBuf buf, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNCC _VI_PTR ViPrintf)        (ViSession vi, ViString writeFmt, ...);
typedef ViStatus (_VI_FUNC _VI_PTR ViVPrintf)       (ViSession vi, ViString writeFmt, ViVAList params);
typedef ViStatus (_VI_FUNCC _VI_PTR ViSPrintf)       (ViSession vi, ViPBuf buf, ViString writeFmt, ...);
typedef ViStatus (_VI_FUNC _VI_PTR ViVSPrintf)      (ViSession vi, ViPBuf buf, ViString writeFmt, ViVAList parms);
typedef ViStatus (_VI_FUNCC _VI_PTR ViScanf)         (ViSession vi, ViString readFmt, ...);
typedef ViStatus (_VI_FUNC _VI_PTR ViVScanf)        (ViSession vi, ViString readFmt, ViVAList params);
typedef ViStatus (_VI_FUNCC _VI_PTR ViSScanf)        (ViSession vi, ViBuf buf, ViString readFmt, ...);
typedef ViStatus (_VI_FUNC _VI_PTR ViVSScanf)       (ViSession vi, ViBuf buf, ViString readFmt, ViVAList parms);
typedef ViStatus (_VI_FUNCC _VI_PTR ViQueryf)        (ViSession vi, ViString writeFmt, ViString readFmt, ...);
typedef ViStatus (_VI_FUNC _VI_PTR ViVQueryf)       (ViSession vi, ViString writeFmt, ViString readFmt,  ViVAList params);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn8)           (ViSession vi, ViUInt16 space, ViBusAddress offset, ViPUInt8  val8);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut8)          (ViSession vi, ViUInt16 space, ViBusAddress offset, ViUInt8   val8);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn16)          (ViSession vi, ViUInt16 space, ViBusAddress offset, ViPUInt16 val16);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut16)         (ViSession vi, ViUInt16 space, ViBusAddress offset, ViUInt16  val16);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn32)          (ViSession vi, ViUInt16 space, ViBusAddress offset, ViPUInt32 val32);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut32)         (ViSession vi, ViUInt16 space, ViBusAddress offset, ViUInt32  val32);
#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViStatus (_VI_FUNC _VI_PTR ViIn64)          (ViSession vi, ViUInt16 space, ViBusAddress offset, ViPUInt64 val64);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut64)         (ViSession vi, ViUInt16 space, ViBusAddress offset, ViUInt64 val64);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn8Ex)         (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViPUInt8 val8);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut8Ex)        (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViUInt8 val8);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn16Ex)        (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViPUInt16 val16);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut16Ex)       (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViUInt16 val16);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn32Ex)        (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViPUInt32 val32);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut32Ex)       (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViUInt32 val32);
typedef ViStatus (_VI_FUNC _VI_PTR ViIn64Ex)        (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViPUInt64 val64);
typedef ViStatus (_VI_FUNC _VI_PTR ViOut64Ex)       (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViUInt64 val64);
#endif
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn8)       (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt8  buf8);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut8)      (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt8  buf8);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn16)      (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt16 buf16);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut16)     (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt16 buf16);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn32)      (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt32 buf32);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut32)     (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt32 buf32);
#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn64)       (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt64 buf64);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut64)      (ViSession vi, ViUInt16 space, ViBusAddress offset, ViBusSize length, ViAUInt64 buf64);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn8Ex)      (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt8 buf8);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut8Ex)     (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt8 buf8);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn16Ex)     (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt16 buf16);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut16Ex)    (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt16 buf16);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn32Ex)     (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt32 buf32);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut32Ex)    (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt32 buf32);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveIn64Ex)     (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt64 buf64);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveOut64Ex)    (ViSession vi, ViUInt16 space, ViBusAddress64 offset, ViBusSize length, ViAUInt64 buf64);
#endif
typedef ViStatus (_VI_FUNC _VI_PTR ViMove)          (ViSession vi, ViUInt16 srcSpace, ViBusAddress srcOffset, ViUInt16 srcWidth, ViUInt16 destSpace, ViBusAddress destOffset, ViUInt16 destWidth, ViBusSize srcLength); 
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveAsync)     (ViSession vi, ViUInt16 srcSpace, ViBusAddress srcOffset, ViUInt16 srcWidth, ViUInt16 destSpace, ViBusAddress destOffset, ViUInt16 destWidth, ViBusSize srcLength, ViPJobId jobId); 
#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveEx)        (ViSession vi, ViUInt16 srcSpace, ViBusAddress64 srcOffset, ViUInt16 srcWidth, ViUInt16 destSpace, ViBusAddress64 destOffset, ViUInt16 destWidth, ViBusSize srcLength);
typedef ViStatus (_VI_FUNC _VI_PTR ViMoveAsyncEx)   (ViSession vi, ViUInt16 srcSpace, ViBusAddress64 srcOffset, ViUInt16 srcWidth, ViUInt16 destSpace, ViBusAddress64 destOffset, ViUInt16 destWidth, ViBusSize srcLength, ViPJobId jobId);
#endif
typedef ViStatus (_VI_FUNC _VI_PTR ViMapAddress)    (ViSession vi, ViUInt16 mapSpace, ViBusAddress mapOffset, ViBusSize mapSize, ViBoolean access, ViAddr suggested, ViPAddr address);
typedef ViStatus (_VI_FUNC _VI_PTR ViUnmapAddress)  (ViSession vi);
#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViStatus (_VI_FUNC _VI_PTR ViMapAddressEx)  (ViSession vi, ViUInt16 mapSpace, ViBusAddress64 mapOffset, ViBusSize mapSize, ViBoolean access, ViAddr suggested, ViPAddr address);
#endif
typedef void (_VI_FUNC _VI_PTR ViPeek8)         (ViSession vi, ViAddr address, ViPUInt8  val8);
typedef void (_VI_FUNC _VI_PTR ViPoke8)         (ViSession vi, ViAddr address, ViUInt8   val8);
typedef void (_VI_FUNC _VI_PTR ViPeek16)        (ViSession vi, ViAddr address, ViPUInt16 val16);
typedef void (_VI_FUNC _VI_PTR ViPoke16)        (ViSession vi, ViAddr address, ViUInt16  val16);
typedef void (_VI_FUNC _VI_PTR ViPeek32)        (ViSession vi, ViAddr address, ViPUInt32 val32);
typedef void (_VI_FUNC _VI_PTR ViPoke32)        (ViSession vi, ViAddr address, ViUInt32  val32);
#if defined(_VI_INT64_UINT64_DEFINED)
typedef void (_VI_FUNC _VI_PTR ViPeek64)        (ViSession vi, ViAddr address, ViPUInt64 val64);
typedef void (_VI_FUNC _VI_PTR ViPoke64)        (ViSession vi, ViAddr address, ViUInt64 val64);
#endif
typedef ViStatus (_VI_FUNC _VI_PTR ViMemAlloc)      (ViSession vi, ViBusSize size, ViPBusAddress offset);
typedef ViStatus (_VI_FUNC _VI_PTR ViMemFree)       (ViSession vi, ViBusAddress offset);
#if defined(_VI_INT64_UINT64_DEFINED)
typedef ViStatus (_VI_FUNC _VI_PTR ViMemAllocEx)    (ViSession vi, ViBusSize size, ViPBusAddress64 offset);
typedef ViStatus (_VI_FUNC _VI_PTR ViMemFreeEx)     (ViSession vi, ViBusAddress64 offset);
#endif
typedef ViStatus (_VI_FUNC _VI_PTR ViGpibControlREN)  (ViSession vi, ViUInt16 mode);
typedef ViStatus (_VI_FUNC _VI_PTR ViGpibControlATN)  (ViSession vi, ViUInt16 mode);
typedef ViStatus (_VI_FUNC _VI_PTR ViGpibSendIFC)     (ViSession vi);
typedef ViStatus (_VI_FUNC _VI_PTR ViGpibCommand)     (ViSession vi, ViBuf cmd, ViUInt32 cnt, ViPUInt32 retCnt);
typedef ViStatus (_VI_FUNC _VI_PTR ViGpibPassControl) (ViSession vi, ViUInt16 primAddr, ViUInt16 secAddr);
typedef ViStatus (_VI_FUNC _VI_PTR ViVxiCommandQuery) (ViSession vi, ViUInt16 mode, ViUInt32 cmd, ViPUInt32 response);
typedef ViStatus (_VI_FUNC _VI_PTR ViAssertUtilSignal)(ViSession vi, ViUInt16 line);
typedef ViStatus (_VI_FUNC _VI_PTR ViAssertIntrSignal)(ViSession vi, ViInt16 mode, ViUInt32 statusID);
typedef ViStatus (_VI_FUNC _VI_PTR ViMapTrigger)      (ViSession vi, ViInt16 trigSrc, ViInt16 trigDest, ViUInt16 mode);
typedef ViStatus (_VI_FUNC _VI_PTR ViUnmapTrigger)    (ViSession vi, ViInt16 trigSrc, ViInt16 trigDest);
typedef ViStatus (_VI_FUNC _VI_PTR ViUsbControlOut)   (ViSession vi, ViInt16 bmRequestType, ViInt16 bRequest, ViUInt16 wValue, ViUInt16 wIndex, ViUInt16 wLength, ViBuf buf);
typedef ViStatus (_VI_FUNC _VI_PTR ViUsbControlIn)    (ViSession vi, ViInt16 bmRequestType, ViInt16 bRequest, ViUInt16 wValue, ViUInt16 wIndex, ViUInt16 wLength, ViPBuf buf, ViPUInt16 retCnt);

//
// DynamicVisa Class definition
//    This class contains a pointer to each VISA function.
//    Its constructor loads the VISA library specified in the
//    constructor parameter and then gets pointers to each of
//    the VISA functions in the library.
//
class DynamicVisa
{
protected:
	QLibrary *libVisa;
private:
	HMODULE m_hmVisa; // Module handle for VISA library
	int m_nMissing;   // Number of function pointers not found;
public:
	//
	// Constructor
	//       Load the VISA library if parameter 'szFileName' is specified.
	//
	DynamicVisa(const char *szFileName = 0);	

	//
	// Destructor
	//
	~DynamicVisa();
	
	int Initialize(const char * szFileName);
	
	//
	// Initialized:
	//    Returns:
	//       true  if the VISA library was successfuly loaded, but some functions may be not initialized
	//      -1  if the VISA library was not found
	//		
	bool Initialized();
	
	//
	// This function performs unload of the VISA library and
	// reset all the function pointers to 'safe' values.
	//	
	void Uninitialize();
	//
	// MissingFnCount:
	//    Returns:
	//       0  if all VISA functions were successfully found
	//      -1  if the VISA library was not found
	//      <n> the count of functions not found
	//    You can call this function to see if the VISA library of
	//    interest exists and if it contains all the VISA functions.
	//	
	int  MissingFnCount();
	
public:
	//
	// Function pointers to VISA functions:
	//
	ViOpenDefaultRM    viOpenDefaultRM;
	ViParseRsrcEx      viParseRsrcEx;
	ViOpen             viOpen;
	ViFindRsrc         viFindRsrc;
	ViFindNext         viFindNext;
	ViParseRsrc        viParseRsrc;
	ViClose            viClose;
	ViSetAttribute     viSetAttribute;
	ViGetAttribute     viGetAttribute;
	ViStatusDesc       viStatusDesc;
	ViTerminate        viTerminate;
	ViLock             viLock;
	ViUnlock           viUnlock;
	ViEnableEvent      viEnableEvent;
	ViDisableEvent     viDisableEvent;
	ViDiscardEvents    viDiscardEvents;
	ViWaitOnEvent      viWaitOnEvent;
	ViInstallHandler   viInstallHandler;
	ViUninstallHandler viUninstallHandler;
	ViRead             viRead;
	ViReadAsync        viReadAsync;
	ViReadToFile       viReadToFile;
	ViWrite            viWrite;
	ViWriteAsync       viWriteAsync;
	ViWriteFromFile    viWriteFromFile;
	ViAssertTrigger    viAssertTrigger;
	ViReadSTB          viReadSTB;
	ViClear            viClear;
	ViSetBuf           viSetBuf;
	ViFlush            viFlush;
	ViBufWrite         viBufWrite;
	ViBufRead          viBufRead;
	ViPrintf           viPrintf;
	ViVPrintf          viVPrintf;
	ViSPrintf          viSPrintf;
	ViVSPrintf         viVSPrintf;
	ViScanf            viScanf;
	ViVScanf           viVScanf;
	ViSScanf           viSScanf;
	ViVSScanf          viVSScanf;
	ViQueryf           viQueryf;
	ViVQueryf          viVQueryf;
	ViIn8              viIn8;
	ViOut8             viOut8;
	ViIn16             viIn16;
	ViOut16            viOut16;
	ViIn32             viIn32;
	ViOut32            viOut32;
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViIn64             viIn64;
	ViOut64            viOut64;
	ViIn8Ex            viIn8Ex;
	ViOut8Ex           viOut8Ex;
	ViIn16Ex           viIn16Ex;
	ViOut16Ex          viOut16Ex;
	ViIn32Ex           viIn32Ex;
	ViOut32Ex          viOut32Ex;
	ViIn64Ex           viIn64Ex;
	ViOut64Ex          viOut64Ex;
	#endif
	ViMoveIn8          viMoveIn8;
	ViMoveOut8         viMoveOut8;
	ViMoveIn16         viMoveIn16;
	ViMoveOut16        viMoveOut16;
	ViMoveIn32         viMoveIn32;
	ViMoveOut32        viMoveOut32;
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViMoveIn64         viMoveIn64;
	ViMoveOut64        viMoveOut64;
	ViMoveIn8Ex        viMoveIn8Ex;
	ViMoveOut8Ex       viMoveOut8Ex;
	ViMoveIn16Ex       viMoveIn16Ex;
	ViMoveOut16Ex      viMoveOut16Ex;
	ViMoveIn32Ex       viMoveIn32Ex;
	ViMoveOut32Ex      viMoveOut32Ex;
	ViMoveIn64Ex       viMoveIn64Ex;
	ViMoveOut64Ex      viMoveOut64Ex;
	#endif
	ViMove             viMove; 
	ViMoveAsync        viMoveAsync; 
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViMoveEx           viMoveEx;
	ViMoveAsyncEx      viMoveAsyncEx;
	#endif
	ViMapAddress       viMapAddress;
	ViUnmapAddress     viUnmapAddress;
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViMapAddressEx     viMapAddressEx;
	#endif
	ViPeek8            viPeek8;
	ViPoke8            viPoke8;
	ViPeek16           viPeek16;
	ViPoke16           viPoke16;
	ViPeek32           viPeek32;
	ViPoke32           viPoke32;
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViPeek64           viPeek64;
	ViPoke64           viPoke64;
	#endif
	ViMemAlloc         viMemAlloc;
	ViMemFree          viMemFree;
	#if defined(_VI_INT64_UINT64_DEFINED)
	ViMemAllocEx       viMemAllocEx;
	ViMemFreeEx        viMemFreeEx;
	#endif
	ViGpibControlREN   viGpibControlREN;
	ViGpibControlATN   viGpibControlATN;
	ViGpibSendIFC      viGpibSendIFC;
	ViGpibCommand      viGpibCommand;
	ViGpibPassControl  viGpibPassControl;
	ViVxiCommandQuery  viVxiCommandQuery;
	ViAssertUtilSignal viAssertUtilSignal;
	ViAssertIntrSignal viAssertIntrSignal;
	ViMapTrigger       viMapTrigger;
	ViUnmapTrigger     viUnmapTrigger;
	ViUsbControlOut    viUsbControlOut;
	ViUsbControlIn     viUsbControlIn;
};

#endif //#ifndef __DYNAMICVISA_HEADER__
