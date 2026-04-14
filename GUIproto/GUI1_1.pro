#-------------------------------------------------
#
# Project created by QtCreator 2026-03-25T20:06:18
#
#-------------------------------------------------

QT       += core gui

TARGET = GUI1_1
TEMPLATE = app

INCLUDEPATH += ../SE2/include

SOURCES += main.cpp\
        mainwindow.cpp \
    scriptwindow.cpp \
    diffviewerwindow.cpp \
    diffviewhighlighter.cpp \
    historywindow.cpp

HEADERS  += mainwindow.h \
    scriptwindow.h \
    diffviewerwindow.h \
    diffviewhighlighter.h \
    historywindow.h \
    commitinfo.h

FORMS    += mainwindow.ui \
    scriptwindow.ui \
    diffviewerwindow.ui \
    historywindow.ui
