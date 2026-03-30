#VPATH = $PWD #Что бы файлы подключались относительно оригинального расположения файла
#INCLUDEPATH += $PWD
#DEFINES += DBG_PROC

PROJECT_ROOT_PATH = $${PWD}
CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
} else {
    BUILD_FLAG = release
}

INCLUDEPATH += $$PWD/import
INCLUDEPATH += $$PWD/jsonAdapt

 CONFIG+=svg

SOURCES     += $$PWD/scriptsmanager.cpp \
    $$PWD/include/errormanager.cpp \
    $$PWD/include/findmodules.cpp \
    $$PWD/se3.cpp \
    $$PWD/include/blockarea.cpp \
    $$PWD/include/updater.cpp \
    $$PWD/include/backuper.cpp \
    $$PWD/include/errors.h \
    $$PWD/scriptitemdelegate.cpp \
    $$PWD/include/metadialog.cpp \
    $$PWD/jsonAdapt/QJsonValueRef.cpp \
    $$PWD/jsonAdapt/QJsonParser.cpp \
    $$PWD/jsonAdapt/QJsonValue.cpp \
    $$PWD/jsonAdapt/QJsonDocument.cpp \
    $$PWD/jsonAdapt/QJsonArray.cpp \
    $$PWD/jsonAdapt/QJsonObject.cpp \
    $$PWD/jsonAdapt/QJsonParseError.cpp

HEADERS     += \
    $$PWD/models.h \
    $$PWD/scriptsmanager.h \
    $$PWD/include/errormanager.h \
    $$PWD/include/findmodules.h \
    $$PWD/se3.h \
    $$PWD/include/createlink.h \
    $$PWD/include/blockarea.h \
    $$PWD/include/updater.h \
    $$PWD/include/backuper.h \
    $$PWD/scriptitemdelegate.h \
    $$PWD/include/metadialog.h \
    $$PWD/include/mount_netw_folder.h \
    $$PWD/jsonAdapt/QJsonValueRef.h \
    $$PWD/jsonAdapt/QJsonParser.h \
    $$PWD/jsonAdapt/QJsonValue.h \
    $$PWD/jsonAdapt/QJsonDocument.h \
    $$PWD/jsonAdapt/QJsonArray.h \
    $$PWD/jsonAdapt/QJsonObject.h \
    $$PWD/jsonAdapt/QJsonParseError.h

FORMS       += \
    $$PWD/se3.ui \
    $$PWD/include/metadialog.ui

RESOURCES += $$PWD/rsrc.qrc

win32:LIBS         += -lole32 -luuid
