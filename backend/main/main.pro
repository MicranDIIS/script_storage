#-------------------------------------------------
#
# Project created by QtCreator 2026-04-13T16:11:35
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = main
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32: LIBS += -L$$PWD/ -lgit2

INCLUDEPATH += $$PWD/include