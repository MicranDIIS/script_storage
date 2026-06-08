#-------------------------------------------------
#
# Project created by QtCreator 2026-03-25T20:06:18
#
#-------------------------------------------------

QT       += core gui

TARGET = GUI
TEMPLATE = app

INCLUDEPATH += ../SE2/include

# Пути к заголовкам
INCLUDEPATH += $$PWD/../backend/bin
#INCLUDEPATH += $$PWD/../backend/libgit2/include

# Исходники
#SOURCES += $$PWD/../backend/src/local/local.cpp \
#           $$PWD/../backend/src/network/network.cpp \
#           $$PWD/../backend/src/Repository/repository.cpp

# Заголовки
HEADERS += $$PWD/../backend/include/MGit/mgit.h
#           $$PWD/../backend/src/Repository/repository.h

CONFIG_DIR = $$PWD/config
DEFINES += CONFIG_DIR=\\\"$$CONFIG_DIR\\\"

# Путь к libgit2.dll и линковка
win32: LIBS += -L$$PWD/../backend/bin -lgit2 \
                -L$$PWD/../backend/bin -lmgit

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
