#ifndef TRANSPFUNCSMICS_H
#define TRANSPFUNCSMICS_H

#include "transpfuncsbase.h"

struct stCommandHead
{	unsigned short  Signature; ///< Признак начала команды = 0x55AA
    unsigned short  Command;   ///< Код команды
    short           Status;    ///< Статус команды
    unsigned short  BodyLen;   ///< Длина тела команды
    unsigned char   FlagA;     ///< Флаг A
    unsigned char   FlagB;     ///< Флаг B
    unsigned short	dummy;     ///< Зарезервированное поле
    unsigned long	CommandID; ///< Идентификатор команды
};

typedef union
{
        struct
        {
            unsigned short Data;			///<	Данные записываемые в регистр.
            unsigned char RegAddress;		///<	Адрес регистра "в блоке".
            unsigned char Address     :5;	///<	Адрес блока в LVDS кольце.
            unsigned char Error       :1;	///<	Флаг ошибки.
            unsigned char Done        :1;	///<	Флаг статуса "выполнения".
            unsigned char RW          :1;	///<	Флан статуса чтение/запись.
        } Scheme;
        unsigned long Frame;				///<	Тело регистра.
        unsigned char Buff[4];				///<	Байтовый буфер.
} uniTunRegister;

class CTranspFuncsMics : public CTranspFuncsBase
{
    Q_OBJECT
public:
    CTranspFuncsMics();

    Q_INVOKABLE int Send(char* sComm,bool bReadAnsw = true, bool bReadByByte = false);
    int Recv(bool bTransl = false, int nByte = 0);
    QString CorrectAddr(QString sAddr);
    void CheckSpecificErr(int nErrCode);
    int SetRegLVDS (int nAddress, int nReg, int nWrData, bool bRead);
    int InitMiTransport(bool bState);
};

#endif // TRANSPFUNCSMICS_H
