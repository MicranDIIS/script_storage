#-------------------------------------------------
#
# Project created by QtCreator 2026-04-22T13:48:10
#
#-------------------------------------------------

QT       -= gui

TARGET = FileStatus
TEMPLATE = lib

DEFINES += FILESTATUS_LIBRARY


HEADERS += filestatus.h\
        FileStatus_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEB008CAD
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = FileStatus.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
