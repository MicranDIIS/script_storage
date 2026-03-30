#include "script_thread.h"
#include <QApplication>
#include <QTextCodec>
#include <QMutexLocker>

ScriptWrap::ScriptWrap(QObject *parent):QObject(parent)
{
    qDebug()<<"script wrap construct";
    finished = true;

    ModuleFinder mfinder;
    QString mpath;
#ifdef DEBUG
    if(mfinder.FindModule("ScriptCBridged.dll",&mpath)!=SE3_NO_ERROR)
#else
    #ifdef WIN32
        if(mfinder.FindModule("ScriptCBridge.dll",&mpath)!=SE3_NO_ERROR)
    #else
        if(mfinder.FindModule("libScriptCBridge.so",&mpath)!=SE3_NO_ERROR)
    #endif
#endif
    {
        SetError(mfinder.GetQSError());
        return;
    }
    library = new QLibrary(mpath);
    if(!library->isLoaded()) library->load();
    if(!library->isLoaded())
    {
        SetError(library->errorString());
        return;
    }

    pCreateScriptControl = (tCreateScriptControl) library->resolve("CreateScriptControl");
    pRemoveScriptControl = (tRemoveScriptControl) library->resolve("RemoveScriptControl");
    pGetSesManagerObj =    (tGetSesManagerObj)    library->resolve("GetSesManagerObj");
    qDebug()<<"script wrap construct success";
//    init(PythonEngine);
}

ScriptWrap::~ScriptWrap()
{
    qDebug()<<"script wrap destructor"<<m_pScriptControl;
    if(pRemoveScriptControl && m_pScriptControl)
    {
        qApp->processEvents();
        qDebug()<<"script wrap control close"<<QThread::currentThread();
        m_pScriptControl->Close();
//        RemoveScriptControl(m_pScriptControl);
        qDebug()<<"script wrap remove control";
        pRemoveScriptControl(m_pScriptControl);
        m_pScriptControl = NULL;
    }

    if (library)
    {
        qDebug()<<"unload library"<<library->unload();
        delete library;
    }

    qDebug()<<"script wrap destroyed";
    qApp->processEvents();
}

QObject *ScriptWrap::GetSesManagerObj()
{
    qDebug()<<"getSMO"<<pGetSesManagerObj;
    if(pGetSesManagerObj)
        return pGetSesManagerObj();
    SetError("GetSesManager not resolved");
    return NULL;
}

void ScriptWrap::init(ScriptEngine scriptType)
{
    qDebug()<<"init ScriptEngine";
    currOp = opInit;
    m_pScriptControl = pCreateScriptControl(scriptType);
    if(!m_pScriptControl)
    {
        CheckResult(SetError("Unable create ScriptControl"));
        return;
    }
    setFinished(true);
    CheckResult(m_pScriptControl->Open());
}

void ScriptWrap::LoadScript(QString path)
{
    currOp = opLoad;
    if(!m_pScriptControl)
        return CheckResult(SetError("ScriptControl pointer - not valid"));

    currScr = path;

    QByteArray temp = path.toLocal8Bit();
    QTextDecoder *decoder = QTextCodec::codecForName("utf-8")->makeDecoder();
    QTextEncoder *encoder = QTextCodec::codecForName("windows-1251")->makeEncoder();
    path = decoder->toUnicode(temp);//QTextCodec::codecForName("windows-1251")->fromUnicode(QTextCodec::codecForName("utf-8")->toUnicode(temp));
    QByteArray path2ba = encoder->fromUnicode(path);

    qDebug()<<"script thread load file"<<path2ba.data();
    return CheckResult(m_pScriptControl->LoadFile(path2ba.data()));
}

void ScriptWrap::StartScript()
{
    currOp = opStart;

    if(!m_pScriptControl)
        return CheckResult(SetError("ScriptControl pointer - not valid"));

    setFinished(false);
    #ifdef WIN32
    CoInitializeEx(NULL,COINIT_MULTITHREADED);
    #endif
    CheckResult(m_pScriptControl->DoFuncNoArg((char*)"Run"));
    #ifdef WIN32
    CoUninitialize();
    #endif
    if(!getFinished())
    {
        setFinished(true);
        emit operationStatus(opRun, noError);
    }
}

void ScriptWrap::StopScript()
{
    currOp = opStop;

    if(!m_pScriptControl)
        return CheckResult(SetError("ScriptControl pointer - not valid"));

    if(!getFinished())
    {
        //Инициализируем прерывание выполнения скрипта
//        m_pScriptControl->DoString((char*)"mibase.abort = 1");
        m_pScriptControl->Interrupt();
//        WaitForStop();
//        if(!m_pScriptControl->GetGlobal((char*)"cc_exec"))
//            m_pScriptControl->DoString((char*)"base.ClearConnections()");
    }
    emit operationStatus(opStop, noError);
}

void ScriptWrap::StopExitScript()
{
    qDebug()<<"Stop&Exit slot"<<getFinished();
    if(!m_pScriptControl)
        return CheckResult(SetError("ScriptControl pointer - not valid"));

    if(!getFinished())
    {
        //Инициализируем прерывание выполнения скрипта
        m_pScriptControl->DoString((char*)"mibase.abort = 1");
//        m_pScriptControl->DoString((char*)"raise KeyboardInterrupt");
        if(!WaitForStop(3000)) // forced terminate
        {
            qDebug()<<"emit forced ExitReady";
            emit exitReady();
        }
//        if(!m_pScriptControl->GetGlobal((char*)"cc_exec"))
//            m_pScriptControl->DoString((char*)"base.ClearConnections()");
    }

//    if(pRemoveScriptControl && m_pScriptControl)
//    {
//        qApp->processEvents();
//        qDebug()<<"script wrap control close"<<QThread::currentThread();
//        m_pScriptControl->Close();
////        RemoveScriptControl(m_pScriptControl);
//        qDebug()<<"script wrap remove control";
//        pRemoveScriptControl(m_pScriptControl);
//        m_pScriptControl = NULL;
//    }

//    if (library)
//    {
//        qDebug()<<"unload library"<<library->unload();
//        delete library;
//        library = NULL;
//    }
     qApp->processEvents();

    qDebug()<<"emit ExitReady";

}

bool ScriptWrap::WaitForStop(int msec)
{
    qDebug()<<"ScriptWrap"<<"wait for stop";
    QEventLoop el;
    QTimer tmo;
    connect(&tmo,SIGNAL(timeout()),&el,SLOT(quit()));
    connect(this,SIGNAL(Finished()),&el,SLOT(quit()));
    if(!getFinished()){
        tmo.start(msec);
        el.exec();
    }
    return getFinished();
}

void ScriptWrap::CheckResult(int code)
{
    if(code != SE3_NO_ERROR)
    {
        bool ok=false;
        errStruct stLastErr;
        stLastErr.errDescr = GetErrorDescription(code,&ok);
        stLastErr.errTraceBack = "";
        stLastErr.opId = currOp;
        qDebug()<<"check result"<<code<<stLastErr.errDescr<<stLastErr.opId<<ok;
        if(!ok)
        {
            if(m_pScriptControl){
                m_pScriptControl->GetError(&(stLastErr.errDescr),&(stLastErr.errTraceBack));
//            if(currOp == opInit)
                lastES = stLastErr;
//                SetError(stLastErr.errDescr+"\r\n"+stLastErr.errTraceBack);
            }
        }
//        emit SendErrStrc(stLastErr);
        emit operationStatus(currOp, error);
        setFinished(true);
        return;
    }
    emit operationStatus(currOp, noError);
}

void ScriptWrap::setFinished(bool val)
{
    QMutexLocker locker(&m_mutex);
    finished = val;
}

bool ScriptWrap::getFinished()
{
    QMutexLocker locker(&m_mutex);
    return finished;
}
