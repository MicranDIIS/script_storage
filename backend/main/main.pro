#-------------------------------------------------
#
# Project created by QtCreator 2026-04-14T14:16:41
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = main
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32: LIBS += -L$$PWD/../libgit2 -lgit2
INCLUDEPATH += $$PWD/../libgit2/include

INCLUDEPATH += $$PWD/../basedata
SOURCES += $$PWD/../basedata/basedata.cpp

INCLUDEPATH += $$PWD/../network
SOURCES += $$PWD/../network/network.cpp

INCLUDEPATH += $$PWD/../local
SOURCES += $$PWD/../local/local.cpp


