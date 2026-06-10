QT =- gui

QT += core

CONFIG += debug
CONFIG -= release

ROOT_PATH = $${PWD}

CONFIG(debug, debug|release){
     BUILD_FLAG = debug
     DEFINES += DEBUG
}else{
     BUILD_FLAG = release
}

#include
HEADERS_PATH = $${ROOT_PATH}/include
IMPORT_PATH = $${ROOT_PATH}/import

INCLUDEPATH += \
            $${HEADERS_PATH}/ \
            $${IMPORT_PATH}/ 

#dll
LIB_PATH = $${ROOT_PATH}/lib

LIBS += -L$${LIB_PATH}
LIBS += \
     -lgit2

#build
BUILD_PATH = $${ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}

RCC_DIR = $${BUILD_PATH}/rcc
UI_DIR = $${BUILD_PATH}/ui
MOC_DIR = $${BUILD_PATH}/moc
OBJECTS_DIR = $${BUILD_PATH}/obj

#bin
BIN_PATH = $${ROOT_PATH}/bin/$${BUILD_FLAG}

defineTest(copyDirToDestDir) {
    files = $$1
    dir = $$2
    # replace slashes in destination path for Windows

#    DDIR = $$DLLDESTDIR
#    win32:DDIR ~= s,/,\\,g
    for(file, files) {
	# replace slashes in source path for Windows
	win32:file ~= s,/,\\,g
	win32:dir ~= s,/,\\,g
	QMAKE_POST_LINK += $$QMAKE_COPY_DIR $$quote($$file) $$quote($$dir) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}


