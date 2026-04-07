#-------------------------------------------------
#
# Project created by QtCreator 2026-03-25T20:06:18
#
#-------------------------------------------------

QT       += core gui

TARGET = GUI
TEMPLATE = app

INCLUDEPATH += ../SE2/include

SOURCES += main.cpp\
        mainwindow.cpp \
    scriptwindow.cpp \
    scriptloader.cpp \
    settingreader.cpp

HEADERS  += mainwindow.h \
    scriptwindow.h \
    scriptloader.h \
    settingreader.h

FORMS    += mainwindow.ui \
    scriptwindow.ui
