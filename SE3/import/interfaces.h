#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtCore>
#include "LuaRef.h"

/*!
 \brief

 \enum
*/
enum
{
    QUER = 0,
    ANSW
};

enum Modes{
    RUN_FROM_CONSOLE = 0,
    EDIT_NOW,
    NETWORK_STORAGE,
    EDIT_NETWORK_STORAGE
};

/*!
 \brief Промежуточный интерфейсный класс для связи плагина с приложением посредством сигналов

 \class INotifyer interfaces.h "interfaces.h"
*/
class INotifyer : public QObject
{
public:
    /*!
     \brief

     \fn ~INotifyer
    */
    virtual ~INotifyer() {}
    /*!
     \brief

     \fn Signal
     \param Param
     \param value
     \param Err
     \param w
    */
    virtual void Signal(QString value, bool Err, QWidget *w ) = 0;
    /*!
     \brief

     \fn instance
    */
    virtual QObject *instance() = 0;

};
/*!
 \brief

 \class PluginInterface interfaces.h "interfaces.h"
*/
class PluginInterface
{
public:
    /*!
     \brief

     \fn ~PluginInterface
    */
    virtual ~PluginInterface() {}
    /*!
     \brief

     \fn GetForm
    */
    virtual QWidget* GetPtrForm() = 0;
    virtual void LoadScript(QString) = 0;
    virtual void setMode(Modes mode) = 0;
};
/*!
 \brief Интерфейс для поиска устройств средствами библиотек visa

 \class FIInterface interfaces.h "interfaces.h"
*/
class FIInterface
{
public:
    /*!
     \brief

     \fn ~FIInterface
    */
    virtual ~FIInterface() {}

    /*!
     \brief Функция запуска поиска приборов

     \fn Execute
     \param TrInterf \li "TCP"
                     \li "USB"
                     \li "COM"
                     \li "GPIB"
     \param Filter \li "R2M"
                    \li "H5M"
                    \li "G7M"
                    \li "R4M"
                    \li "SK4M"
     \param TypeRes \li "0" - Simple
                    \li "1" - Visa
     \param w указатель на виджет отправитель
    */
    virtual void Execute(QString TrInterf, QString Filter, int TypeRes, QWidget* w) = 0;

    virtual INotifyer* notifyer() = 0;
};

/*!
 \brief Интерфейс для взаимодействия с плагинами визуального представления

 \class VisualPluginInterface interfaces.h "interfaces.h"
*/
class VisualPluginInterface{

public:
    /*!
     \brief

     \fn ~VisualPluginInterface
    */
    virtual ~VisualPluginInterface(){}
    /*!
     \brief

     \fn startPlugin
     \param VM
    */
    virtual void startPlugin(void * VM,std::string* UserPtr,QString ScriptName = "") = 0;
    /*!
     \brief

     \fn stopPlugin
    */
    virtual void stopPlugin() = 0;
    virtual QWidget* GetPtrForm() = 0;
};


Q_DECLARE_INTERFACE(PluginInterface, "ru.YY.PluginInterface")
Q_DECLARE_INTERFACE(FIInterface, "ru.YY.FIInterface")
Q_DECLARE_INTERFACE(VisualPluginInterface, "ru.YY.VisualPluginInterface")

#endif // INTERFACES_H
