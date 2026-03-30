#ifndef ITRANSPFUNCS_H
#define ITRANSPFUNCS_H

#include <iostream>

class ITranspFuncs {

public:
    virtual int Connect(std::string addr) = 0;
    virtual int Send(std::string comm,bool ReadAnsw = true) = 0;
    virtual int Disconnect() = 0;
    virtual int SleepMS(int mstime) = 0;

//protected:

    virtual int Recv(bool transl = false) = 0;
    virtual int CheckErr(int errCode) = 0;
    virtual std::string GetAnsw() = 0;
    virtual void SetAnsw(std::string val) = 0;
};

////    R2

////    R4 , G7
//    int MakeAttCalibrFile(string att, string data, string hddFilePath);
//    int GetRegulCurveData(); // запрос данных для регулировочной кривой с калибратора
//    // функции и атрибуты для работы с калибратором
//    int SendToCalibrator(QString str_comm, bool ReadAnsw = true);
//    int ReciveFromCalibrator(bool transl = false, int nbyte = 0);
//    string AnswCalibrator;
//    char RawDataCalibrator[65535];
//    void SetAnswCalibrator(string answ);
//    string GetAnswCalibrator();
//    int GetRegulCurveDataCalibrator();
//    setCalibratorAddress(string calibAddr);

////    Common
//    int GetPower(int channel, bool ReadOnly);
//    int SetRegLVDS (int address, int reg, int wrData, bool Read);
//    int MakeBinFile(string contentString, string dir, bool isWriteToHdd = false, int ftpDir = 0);
//    int OpenDetCh(string path);
//    int SweepTime();
//    int MakeRegulCurves(string freqs, string dir, int calType, bool isWriteToHdd = false, int ftpDir = 0, string calibratorAddr = "");
//    string GetError();

// Unidev
// int SetTerminalChar(string char)


#endif // ITRANSPFUNCS_H







