#-------------------------------------------------
#
# Project created by QtCreator 2026-04-14T12:54:39
#
#-------------------------------------------------

QT       -= gui

TARGET = core
TEMPLATE = lib

DEFINES += CORE_LIBRARY

SOURCES += core.cpp

HEADERS += core.h\
        core_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE721DE3C
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = core.dll
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
