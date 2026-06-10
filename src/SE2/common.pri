PROJECT_ROOT_PATH = $${PWD}

CONFIG(debug, debug|release) {
    BUILD_FLAG = debug
#    LIB_SUFFIX = d
    DEFINES   += DEBUG
} else {
    BUILD_FLAG = release
}

LIBS_PATH = $${PROJECT_ROOT_PATH}/lib
INC_PATH = $${PROJECT_ROOT_PATH}/include
INT_INC_PATH = $${PROJECT_ROOT_PATH}/src/include
IMPORT_PATH = $${PROJECT_ROOT_PATH}/import/
BIN_PATH = $${PROJECT_ROOT_PATH}/bin/$${BUILD_FLAG}
BUILD_PATH = $${PROJECT_ROOT_PATH}/build/$${BUILD_FLAG}/$${TARGET}

RCC_DIR = $${BUILD_PATH}/rcc/
UI_DIR = $${BUILD_PATH}/ui/
MOC_DIR = $${BUILD_PATH}/moc/
OBJECTS_DIR = $${BUILD_PATH}/obj/
LIBS += -L$${LIBS_PATH}/
INCLUDEPATH += $${INC_PATH}/
INCLUDEPATH += $${INT_INC_PATH}/
INCLUDEPATH += $${IMPORT_PATH}/

#VERSION = 0.0.0.9
CONFIG -= debug_and_release
CONFIG += skip_target_version_ext

QMAKE_TARGET_COPYRIGHT = Micran

defineTest(newCopyToDestDir) {
    _SRC = $$1
    _DST = $$2

    win32 {
	# Превращаем пути в формат Windows (с обратными слешами)
	_SRC_WIN = $$replace(_SRC, /, \\)
	_DST_WIN = $$replace(_DST, /, \\)

	# /E - копирует все подпапки (включая пустые)
	# /I - если папка назначения не существует, считает её папкой
	# /Y - перезаписывать файлы без подтверждения
	# /Q - "тихий" режим
	QMAKE_POST_LINK += xcopy "$$_SRC_WIN" "$$_DST_WIN" /E /I /Y /Q $$escape_expand(\\n\\t)
    } else {
	# Для Linux/macOS
	QMAKE_POST_LINK += cp -r $$quote($$_SRC) $$quote($$_DST) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

 #Copies the given files to the destination directory
defineTest(copyToDestdir) {
    files = $$1
    DDIR = $$2

    for(FILE, files) {
	# Replace slashes in paths with backslashes for Windows
	win32:FILE ~= s,/,\\,g
	win32:DDIR ~= s,/,\\,g
	QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
    }
    export(QMAKE_POST_LINK)
}

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

#QMAKE_POST_LINK  = translate_subdirs.bat
