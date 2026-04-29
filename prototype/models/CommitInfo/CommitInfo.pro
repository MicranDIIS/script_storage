#-------------------------------------------------
#
# Project created by QtCreator 2026-04-22T13:48:40
#
#-------------------------------------------------

QT       -= gui

TARGET = CommitInfo
TEMPLATE = lib

DEFINES += COMMITINFO_LIBRARY


HEADERS += commitinfo.h\
        CommitInfo_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE92F7247
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = CommitInfo.dll
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
