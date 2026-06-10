QT -= gui
TEMPLATE = lib
LIBS += -L$${LIBS_PATH}/extLibs/
LIBS += -L$${LIBS_PATH}/lib/
LIBS += -ltolua++
LIBS += -llua5.1
DESTDIR = $${LIBS_PATH}/lib
win32: DLLDESTDIR = $${BIN_PATH}

!exists($$quote($${BIN_PATH}/lualdap*)){
    copyToDestdir($${LIBS_PATH}/extLibs/,$${BIN_PATH})
}

!exists($$quote($${BIN_PATH}/mime/*)){
    copyDirToDestDir($${LIBS_PATH}/extLibs/mime,$${BIN_PATH}/mime)
}

!exists($$quote($${BIN_PATH}/socket/*)){
    copyDirToDestDir($${LIBS_PATH}/extLibs/socket,$${BIN_PATH}/socket)
}

QMAKE_TARGET_COPYRIGHT = (c)MICRAN
