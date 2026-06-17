#-------------------------------------------------
#
# Project created by QtCreator 2026-03-25T20:06:18
#
#-------------------------------------------------

TARGET = GUI
TEMPLATE = app

include(../../commoncore.pri)

QT       += core gui
DESTDIR = $${BIN_PATH}

# Пути к заголовкам
INCLUDEPATH += $${HEADERS_PATH}/lua \
               $${HEADERS_PATH}

CONFIG_DIR = $$PWD/config
DEFINES += CONFIG_DIR=\\\"$$CONFIG_DIR\\\"

LIBS += -L$${LIB_PATH}
LIBS += -lmgit

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

TRANSLATIONS += $$PWD/app_ru.ts

#<<<<<<< HEAD:src/GUIproto/GUI1_1.pro
#win32 {
#    QM_OUTDIR = $${DESTDIR}/translations
#    QMAKE_POST_LINK += if not exist \"$$QM_OUTDIR\" mkdir \"$$QM_OUTDIR\" $$escape_expand(\\n\\t)
#    QMAKE_POST_LINK += \"$$[QT_INSTALL_BINS]\\lrelease.exe\" \"$$PWD\\translations\\app_en.ts\" -qm \"$$QM_OUTDIR\\app_en.qm\" $$escape_expand(\\n\\t)
#}

#=======
RESOURCES += \
    resources.qrc

