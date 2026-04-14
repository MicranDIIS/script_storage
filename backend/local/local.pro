#-------------------------------------------------
#
# Project created by QtCreator 2026-04-14T14:09:28
#
#-------------------------------------------------

QT       -= gui

TARGET = local
TEMPLATE = lib

DEFINES += LOCAL_LIBRARY

SOURCES += local.cpp

HEADERS += local.h\
        local_global.h

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE7C90FBE
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = local.dll
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
