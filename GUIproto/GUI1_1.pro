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
    diffviewerwindow.cpp \
    diffviewhighlighter.cpp \
    scriptloader.cpp \
    settingreader.cpp \
    headerparser.cpp \
    viewmodel.cpp \
    scriptfiltermodel.cpp \
    combofiltermodel.cpp \
    historywindow.cpp


HEADERS  += mainwindow.h \
    scriptwindow.h \
    diffviewerwindow.h \
    diffviewhighlighter.h \
    scriptloader.h \
    settingreader.h \
    headerparser.h \
    viewmodel.h \
    scriptfiltermodel.h \
    combofiltermodel.h \
    historywindow.h \
    commitinfo.h

FORMS    += mainwindow.ui \
    scriptwindow.ui \
    diffviewerwindow.ui \
    historywindow.ui
