#-------------------------------------------------
#
# Project created by QtCreator 2026-04-22T15:29:28
#
#-------------------------------------------------

QT       -= gui

TARGET = IRepositoryFactroy
TEMPLATE = lib

DEFINES += IREPOSITORYFACTROY_LIBRARY


HEADERS += irepositoryfactroy.h\
        IRepositoryFactroy_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE26FB637
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = IRepositoryFactroy.dll
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

INCLUDEPATH += $$PWD/../IRepository
