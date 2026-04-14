#-------------------------------------------------
#
# Project created by QtCreator 2026-04-14T12:42:55
#
#-------------------------------------------------

QT       -= gui

TARGET = network
TEMPLATE = lib

DEFINES += NETWORK_LIBRARY

SOURCES += network.cpp

HEADERS += network.h\
        network_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE6DE0D94
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = network.dll
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

INCLUDEPATH += $$PWD/../basedata
SOURCES += $$PWD/../basedata/basedata.cpp
