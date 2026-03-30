#ifndef CTRANSPORT_H
#define CTRANSPORT_H

#include "Libs_global.h"
//#include <qt_windows.h>
#include <vector>
#include <iostream>
#include "visatype.h"
#define IN

class ICTransport
{
public:
   virtual ~ICTransport(){}
   virtual ViStatus InitClass ( int Lib = 0 ) = 0;
   virtual ViStatus UnInitClass ( ) = 0;
   virtual ViStatus Connect ( ViRsrc Address, int bin = 0) = 0;
   virtual ViStatus Disconnect ( void ) = 0;
   virtual ViStatus SetTimeOut ( int TMO) = 0;
//   virtual ViStatus SetBaudRate( int baud) = 0;
   virtual ViStatus SetSuppressMICS ( int Supp) = 0;
   virtual ViStatus SendData ( IN void *btData, IN int *piNumData ) = 0;
   virtual ViStatus RecvData ( IN void *btData, IN int *piNumData ) = 0;
   virtual ViStatus FindInstr ( ViString exp, std::vector<std::string> *vAddresses,
                                  std::vector<std::string> *Data) = 0;
   virtual ViStatus GetErrorDesc(ViStatus status, ViChar _VI_FAR desc[]) = 0;	// Описание ошибки
};
extern "C"{
LIB_DEST ICTransport*  CreateTrClass();
LIB_DEST void DestroyTrClass(ICTransport* pCTransport);
}
#endif // CTRANSPORT_H
