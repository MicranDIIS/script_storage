TARGET = mgit

include(../../commoncore.pri)
include(../../libcore.pri)

DEFINES += MGIT_LIBRARY

INCLUDEPATH += $$PWD/core \  
            $$PWD/handlers \

HEADERS += \
        mgit.h \
        $$PWD/core/repository.h \
        $$PWD/handlers/update_handler.h


SOURCES += \
        $$PWD/core/repository.cpp \
        $$PWD/core/network.cpp \
        $$PWD/core/local.cpp \
        $$PWD/utils/git_error.cpp \
        $$PWD/handlers/update_handler.cpp


symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE25539B8
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = olmkm.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
