#ifndef ENUMS_H
#define ENUMS_H

#include "time.h"

enum eSweepMode{
    ONCE = 0,
    AUTO
};

enum eState{
    GELSTATE_STAND = 1,
    GELSTATE_RUNNING
};

#pragma pack(push)
#pragma pack(1)
typedef struct{
    time_t timeStamp;
    double numPoints;
    double shiftFreq;
    double ifFilter;
    double averageTime;
    double id;
}GPacketHeader;
#pragma pack(pop)

typedef struct
{
    double f;
    double i;
    double v;
}pointIVData;

#endif // ENUMS_H
