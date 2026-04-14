#-------------------------------------------------
#
# Project created by QtCreator 2026-04-14T13:33:02
#
#-------------------------------------------------

QT       -= gui

TARGET = basedata
TEMPLATE = lib

DEFINES += BASEDATA_LIBRARY

SOURCES += basedata.cpp

HEADERS += basedata.h\
        basedata_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE71DDE9C
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = basedata.dll
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

win32: LIBS += -L$$PWD/../libgit2 -lgit2

INCLUDEPATH += $$PWD/../libgit2/include
