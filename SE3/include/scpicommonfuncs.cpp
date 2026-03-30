#include "scpicommonfuncs.h"
#include "QtCore"

void RegTrFuncs(ptr_createTr pCrTr, ptr_deleteTr pDelTr)
{
    spCrTr = pCrTr;
    spDelTr = pDelTr;
}

ScpiCommonFuncs::ScpiCommonFuncs() :
    ErrorManager()
{
    ptrTr = 0;
    pCreateTr = spCrTr;
    pDeleteTr = spDelTr;
    visaType_ = 1;
    timeOut_ = 45000;
    isAddrDbl = false;
    termChar_ = "\n";
}

ScpiCommonFuncs::~ScpiCommonFuncs()
{
    if(ptrTr && pDeleteTr){
        pDeleteTr(ptrTr);
        ptrTr = 0;
    }
}

int ScpiCommonFuncs::CheckErr(int errCode)
{
    qDebug() << "error code " << errCode;
    if(errCode) {
        char errDescr[200];
        if(!ptrTr->GetErrorDesc(errCode,errDescr) && QString(errDescr)!= "") {
            SetError(QString::fromLocal8Bit(errDescr));
        } else {
            SetError(tr("Unknown error ")+QString::number(errCode));
        }
        if(errCode<0) {
            return -1;
        }
    }
    return 0;
}

int ScpiCommonFuncs::Connect(std::string addr)
{
    int bin = 0;
    QString VAddr=QString::fromStdString(addr);
    ptrTr = pCreateTr(VAddr,isAddrDbl);
    if (ptrTr) {
        if (isAddrDbl) {
            return 0;
        }
        int code = ptrTr->InitClass(visaType_);
        if (CheckErr(code)) {
            return -1;
        }
        code = ptrTr->Connect(VAddr.toLocal8Bit().data(),bin);
        if (CheckErr(code)) {
            return -1;
        }
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int ScpiCommonFuncs::Disconnect()
{
    if (isAddrDbl) {
        return 0;
    }
    if (ptrTr) {
        ptrTr->Disconnect();
        pDeleteTr(ptrTr);
        ptrTr = NULL;
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

std::string ScpiCommonFuncs::GetAnsw()
{
    string AnswTemp = Answ;
    Answ = "";
    return AnswTemp;
}

int ScpiCommonFuncs::GetMeasTime()
{
    SetAnsw(QString::number(measTime_).toStdString());
    return 0;
}

int ScpiCommonFuncs::Recv(bool transl)
{

    if (ptrTr) {
//        ptrTr->SetTimeOut(timeOut_);
        int sizeInBuff = 65535;
        int cnt = 65535;
        char *buf = new char[sizeInBuff];
        QTime timer;
        timer.start();
        if (CheckErr(ptrTr->RecvData(buf,&cnt))) {
            return -1;
        }
//        qDebug() << "RecvData: " << timer.elapsed();
        buf[cnt] = '\0';
        Answ = buf;
        delete [] buf;
    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

void ScpiCommonFuncs::SetAnsw(string val)
{
    Answ = val;
}

int ScpiCommonFuncs::SetTermChar(string termChar)
{
    termChar_ = QString::fromStdString(termChar);
    return 0;
}

int ScpiCommonFuncs::SetTimeout(int msec)
{
    timeOut_ = msec;
    return 0;
}

int ScpiCommonFuncs::SetVisaType(int type)
{
    if ((type == 0) || (type == 1)) {
        visaType_ = type;
    } else {
        SetError("You try to set bad visa type");
        return -1;
    }
    return 0;
}

int ScpiCommonFuncs::Send(std::string str_comm, bool ReadAnsw)
{
    QTime timer;
    timer.start();

//    QTime timer2;
//    timer2.start();
    if(ptrTr) {
        QString comm = QString::fromStdString(str_comm);
        if(!comm.contains(termChar_))
            comm+=termChar_;
        int cnt = comm.count();
        if (CheckErr(ptrTr->SendData(comm.toLocal8Bit().data(),&cnt))) {
            return -1;
        }
//        qDebug() << "------------";
//        qDebug() << QString::fromStdString(str_comm);
//        qDebug() << "Send: " << timer2.elapsed();
//        timer2.restart();
//        if((comm.contains("?")&& ReadAnsw && byByte)
//                || (byByte && ReadAnsw))
//        {
//            QString answer = "";
//            while (Answ[0] != termChar_) {
//                if(Recv(false,1)) {
//                    return -1;
//                }
//                answer.append(Answ[0]);
//            }
//            Answ = answer.toStdString();
//        }
        if (comm.contains("?")&& ReadAnsw) {
            if (Recv(true)) {
                return -1;
            }
//            qDebug() << "Recv: " << timer2.elapsed();
        }
        measTime_ = timer.elapsed();
//        qDebug() << "Send + Recv: " << timer.elapsed();
//        qDebug() << "------------";

    } else {
        SetError(tr("Do not create a transport class"));
        return -1;
    }
    return 0;
}

int ScpiCommonFuncs::SleepMS(int mstime)
{
    Sleep(mstime);
    return 0;
}

