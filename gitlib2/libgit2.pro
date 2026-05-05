#-------------------------------------------------
#
# Project created by QtCreator 2026-04-11T11:14:05
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = libgit2
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32: LIBS += -L$$PWD/ -llibgit2

INCLUDEPATH += $$PWD/include
