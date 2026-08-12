TARGET = mgit

include(../../commoncore.pri)
include(../../libcore.pri)

CORE_PATH = $$PWD/core/
HANDLERS_PATH = $$PWD/handlers/
UTILS_PATH = $$PWD/utils/

#QMAKE_MOC = C:/QtSDK/Desktop/Qt/4.7.3/mingw/bin/moc.exe

DEFINES += MGIT_LIBRARY

INCLUDEPATH += $${CORE_PATH} \  
            $${HANDLERS_PATH} \
            $${UTILS_PATH}


HEADERS += \
        mgit.h \
        $${CORE_PATH}/repository.h \
        $${HANDLERS_PATH}/update_handler.h \
        $${UTILS_PATH}/git_raii.h \
        $${UTILS_PATH}/git_utils.h 


SOURCES += \
        $${CORE_PATH}/repository.cpp \
        $${CORE_PATH}/network.cpp \
        $${CORE_PATH}/local.cpp \
        $${CORE_PATH}/diff.cpp \
        $${UTILS_PATH}/git_error.cpp \
        $${HANDLERS_PATH}/update_handler.cpp \
        $${UTILS_PATH}/git_utils.cpp

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
