QT          += core gui
CONFIG      -= debug_and_release
TEMPLATE     = app
DESTDIR = $${BIN_PATH}/

greaterThan(QT_MAJOR_VERSION, 4) {
  QT        += widgets
  DEFINES   += HAVE_QT5
}

win32 {
!exists($$quote($${BIN_PATH}/Qt*.dll)){
    newCopyToDestDir($${LIBS_PATH}/lib/QtLibs,$${BIN_PATH})
}
! exists($$quote($${BIN_PATH}/scripts/*)){
    newCopyToDestDir($${LIBS_PATH}/baseScriptsFiles,  $${BIN_PATH}/scripts)
}
! exists($$quote($${BIN_PATH}/lua/*)){
    newCopyToDestDir($${LIBS_PATH}/baseLuaFiles,  $${BIN_PATH}/lua)
}
! exists($$quote($${BIN_PATH}/localSettings.ini)){
    newCopyToDestDir($${LIBS_PATH}/baseConfigFiles,$${BIN_PATH})
}
}

