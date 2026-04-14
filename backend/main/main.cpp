#include <QtCore/QCoreApplication>
#include <network.h>
#include <local.h>
#include <basedata.h>
#include <stdio.h>
#include <QDebug>

int main(int argc, char *argv[])
{

    setlocale(LC_ALL,"Russian");
    QCoreApplication a(argc, argv);

    git_libgit2_init();
    //клонируетс€ наш репозиторий
    const QString URL = "https://github.com/MicranDIIS/script_storage";
    //path надо задать самому
    const QString path = "";
    //им€ тоже самому
    const QString username = "";
    //токен тоже
    const QString token = "";
    if(repo_clone(URL,path,username,token) != 0){
        printf("Ќе удалось клонировать репозиторий и обработчика ошибок еще нет\n . Ќо советую проверить аргументы ф-ии\n");
    }else{
        printf(" лонирование прошло успешно!\n");
    }

    const QString path_file = path + "\\text.txt";
    //создаем новый файл в директории с гитом
    FILE *f = fopen(path_file.toUtf8().constData(),"w");

    //новый обьект типа Repo который в конструкторе сразу наш репозиторий открывает
    Repo rep = Repo(path);
    QList<FileStatus> list = rep.status();

    if(list.isEmpty()){
        printf("¬се плохо и status не работает");
    }else{
        //имена статуса всех каких-то не таких файлов. ¬ нашем случае должно вывести text.txt
        foreach(FileStatus value,list){
            qDebug() << value.path_new;
        }
    }

    git_libgit2_shutdown();

    fclose(f);
    return a.exec();
}
