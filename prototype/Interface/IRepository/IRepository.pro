#-------------------------------------------------
#
# Project created by QtCreator 2026-04-22T13:47:38
#
#-------------------------------------------------

QT       -= gui

TARGET = IRepository
TEMPLATE = lib

DEFINES += IREPOSITORY_LIBRARY


HEADERS += irepository.h\
        IRepository_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEB6CDE92
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = IRepository.dll
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

INCLUDEPATH += $$PWD/../../models/CommitInfo
INCLUDEPATH += $$PWD/../../models/FileStatus
