isEmpty(DESTDIR): DESTDIR = $${BIN_PATH}/

greaterThan(QT_MAJOR_VERSION, 4) {
  QT        += widgets
  DEFINES   += HAVE_QT5
}

win32 {
!exists($$quote($${BIN_PATH}/Qt*.dll)){
    newCopyToDestDir($${SE2_LIBDATA_PATH}/lib/QtLibs,$${BIN_PATH})
}
! exists($$quote($${BIN_PATH}/scripts/*)){
    newCopyToDestDir($${SE2_LIBDATA_PATH}/baseScriptsFiles,  $${BIN_PATH}/scripts)
}
! exists($$quote($${BIN_PATH}/lua/*)){
    newCopyToDestDir($${SE2_LIBDATA_PATH}/baseLuaFiles,  $${BIN_PATH}/lua)
}
! exists($$quote($${BIN_PATH}/localSettings.ini)){
    newCopyToDestDir($${SE2_LIBDATA_PATH}/baseConfigFiles,$${BIN_PATH})
}
}

