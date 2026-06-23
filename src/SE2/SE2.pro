TARGET = SE2

include(common.pri )
include(app.pri )
include(../../appcore.pri)

TRANSLATIONS = se2.ts
RC_FILE = SE2.rc

QMAKE_TARGET_PRODUCT = SE 2
QMAKE_TARGET_DESCRIPTION = Script Executor 2

SOURCES += main.cpp\
	se2.cpp

HEADERS  += se2.h

FORMS    += se2.ui

RESOURCES += \
    rsrc.qrc
