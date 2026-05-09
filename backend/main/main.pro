TEMPLATE = app
TARGET = main
QT += core
QT -= gui

CONFIG += console  # ← ЭТО КЛЮЧЕВОЕ

# Пути к заголовкам
INCLUDEPATH += $$PWD/../include/MGit
INCLUDEPATH += $$PWD/../libgit2/include

# Исходники
SOURCES += main.cpp \
            $$PWD/../src/local/local.cpp \
            $$PWD/../src/network/network.cpp \
           $$PWD/../src/Repository/repository.cpp

# Заголовки
HEADERS += $$PWD/../include/MGit/mgit.h \
           $$PWD/../src/Repository/repository.h

# Путь к libgit2.dll и линковка
win32: LIBS += -L$$PWD/../libgit2 -lgit2



