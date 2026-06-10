TEMPLATE = lib

DESTDIR = $${LIB_PATH}

win32: DLLDESTDIR = $${BIN_PATH}

!exists($$quote($${BIN_PATH}/git2*)){
    copyDirToDestDir($${LIB_PATH}, $${BIN_PATH})
}
