#include "repository.h"

bool FileStatus::isNewToHead() const {return flags & STATUS_NEW_TO_HEAD;}
bool FileStatus::isModFileToHead() const {return flags & STATUS_MODFILE_TO_HEAD;}
bool FileStatus::isDeleteToHead() const {return flags & STATUS_DELETE_TO_HEAD;}
bool FileStatus::isRenameToHead() const {return flags & STATUS_RENAME_TO_HEAD;}
bool FileStatus::isNewToDir() const {return flags & STATUS_NEW_TO_DIR;}
bool FileStatus::isModFileToDir() const {return flags & STATUS_MODFILE_TO_DIR;}
bool FileStatus::isDeleteToDir() const {return flags & STATUS_DELETE_TO_DIR;}
bool FileStatus::isRenameToDir() const {return flags & STATUS_RENAME_TO_DIR;}
