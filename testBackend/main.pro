include(../commoncore.pri)

QT       += core

QT       -= gui

TARGET = main
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += ../include/

TEMPLATE = app

HEADERS += \
        ../include/mgit.h \

SOURCES += main.cpp

LIBS += -L$$PWD/BUILD_PATH -lmgit
