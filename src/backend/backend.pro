TARGET = mgit

include(../../common.pri)
include(../../lib.pri)

DEFINES += MGIT_LIBRARY

HEADERS += \
        mgit.h \
        mgit_global.h \
        repository.h 

SOURCES += \
        repository.cpp \
        network.cpp \
        local.cpp

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
