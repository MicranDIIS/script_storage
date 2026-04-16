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


    Repo repo = Repo::clone("https://github.com/MicranDIIS/script_storage","","","");
    if(!repo.valid()){
        printf("error\n");
    }else{
        printf("yes\n");
    }

    if(!repo.valid()){
        printf("no valid");
    }else{
        printf("yra");
    }
    FILE *f = fopen("\\test.txt","w");
    fprintf(f,"%s","asd");


    QList<FileStatus> list = repo.status();


    if(list.isEmpty()){
        printf("pysto");
    }
    foreach(FileStatus value,list){
        if(value.flag_check(STATUS_NEW_TO_DIR)){
            qDebug() << value.get_path_new() + " " + "NEW_TO_DIR";
        }
    }

    git_libgit2_shutdown();

    fclose(f);
    return a.exec();
}
