#ifndef FILESTATUS_H
#define FILESTATUS_H

#include <QString>

enum STATUS_FLAG{
    STATUS_NEW_TO_HEAD     = 1 << 0, //новый
    STATUS_MODFILE_TO_HEAD = 1 << 1, //изменен
    STATUS_DELETE_TO_HEAD  = 1 << 2, //удален
    STATUS_RENAME_TO_HEAD  = 1 << 3, //переименован
    STATUS_MODFILE_TO_DIR  = 1 << 4, //изменен в директории
    STATUS_DELETE_TO_DIR   = 1 << 5, //удален в директории
    STATUS_RENAME_TO_DIR   = 1 << 6, //переименован в директории
    STATUS_NEW_TO_DIR      = 1 << 7  //новый в директории
};

class FileStatus{
    private:
        QString path_new;
        QString path_old;
        int     flags;
    public:
        //конструктор
        FileStatus(const QString &path_new_, const QString &path_old_, int flags_) :
                   path_new(path_new_), path_old(path_old_), flags(flags_) {}
        //геттеры
        const QString& get_path_new(void) const {return path_new;}
        const QString& get_path_old(void) const {return path_old;}
        //проверка флагов
        bool flag_check(STATUS_FLAG flag) const {return flags & flag;}
        bool flag_any_check(int mask) const {return flags & mask;}
        //сеттеры
        void set_flags(int flags_){flags = flags_;}
        void add_flag(STATUS_FLAG flag){flags | flag;}
        void remove_flag(STATUS_FLAG flag){flags &= ~flag;}

        //удобные проверки
        bool is_new_in_head(){return flags & STATUS_NEW_TO_HEAD;}
        bool is_modfile_in_head(){return flags & STATUS_MODFILE_TO_HEAD;}
        bool is_delete_in_head(){return flags & STATUS_DELETE_TO_HEAD;}
        bool is_rename_in_head(){return flags & STATUS_RENAME_TO_HEAD;}
        bool is_new_in_dir(){return flags & STATUS_NEW_TO_DIR;}
        bool is_modfile_in_dir(){return flags & STATUS_MODFILE_TO_DIR;}
        bool is_delete_in_dir(){return flags & STATUS_DELETE_TO_DIR;}
        bool is_rename_in_dir(){return flags & STATUS_RENAME_TO_DIR;}
};

#endif
