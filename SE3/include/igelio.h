#ifndef IGELIO_H
#define IGELIO_H

#include "libsCommon.h"
#include "enums.h"
#include <memory>

struct IGelio
{
    virtual ~IGelio() = default;
    virtual int connectToDevice(char* address) = 0;
    virtual int disconnectDevice() = 0;

    virtual int setFreqRange(double start, double stop, int numPoints) = 0;
    virtual int setFreqList(double* list, int numPoint) = 0;
    virtual int setIfFilter(double iffbw) = 0;
    virtual int setAverageTime(int time) = 0;
    virtual int setShiftFreq(double shift) = 0;
    virtual int setSweepMode(eSweepMode mode) = 0;
    virtual int initBuffer(char* buffer, unsigned long long int size) = 0;

    virtual int run() = 0;
    virtual int stop() = 0;
    virtual int getSweepPackets(char* buff,int buf_size, int numPacket) = 0;
    virtual int getSweepPacketNum() = 0;
    virtual int getSweepTime() = 0;
    virtual int clearPacketBuff() = 0;

    virtual int applyCorrCableFile(char* path) = 0;
    virtual int applyCorrAntennaFile(char* path) = 0;
    virtual int clearAllCorrection() = 0;

    virtual int getStatus() = 0;
    virtual char* getErrorDescr(int code) = 0;
};

using GelioPtr = std::shared_ptr<IGelio>;
//typedef std::shared_ptr<IGelio> GelioPtr;

extern "C" DLL_EXPORT GelioPtr createInstance();
extern "C" DLL_EXPORT IGelio* createInstanceOld();
extern "C" DLL_EXPORT void removeInstanceOld(IGelio*);


#endif // IGELIO_H
