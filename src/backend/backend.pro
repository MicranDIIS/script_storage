TARGET = mgit

include(../../commoncore.pri)
include(../../libcore.pri)

DEFINES += MGIT_LIBRARY

HEADERS += \
        mgit.h \
        repository.h 

SOURCES += \
        repository.cpp \
        network.cpp \
        local.cpp \
        gitError.cpp 

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
