#include <QtCore/QCoreApplication>
#include <network.h>
#include <local.h>
#include <basedata.h>
#include <stdio.h>
#include <QDebug>
#include <QSettings>
#include <QFile>


struct RepoConfig {
    QString url;
    QString branch;
    QString username;
    QString token;
    QString path;
};

bool loadRepoConfig(RepoConfig &config) {
    if (!QFile::exists("repository.ini")) {
        qWarning() << "Файл repository.ini не найден";
        return false;
    }

    QSettings settings("repository.ini", QSettings::IniFormat);

    config.url      = settings.value("Repository/url").toString();
    config.branch   = settings.value("Repository/branch", "main").toString();
    config.path     = settings.value("Repository/path").toString();
    config.username = settings.value("Credentials/username").toString();
    config.token    = settings.value("Credentials/token").toString();

    if (config.url.isEmpty()) {
        qWarning() << "Отсутствует поле: Repository/url";
        return false;
    }
    if (config.path.isEmpty()) {
        qWarning() << "Отсутствует поле: Repository/path";
        return false;
    }
    if (config.username.isEmpty()) {
        qWarning() << "Отсутствует поле: Credentials/username";
        return false;
    }
    if (config.token.isEmpty()) {
        qWarning() << "Отсутствует поле: Credentials/token";
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{

    setlocale(LC_ALL,"Russian");
    QCoreApplication a(argc, argv);

    RepoConfig config;
    if (!loadRepoConfig(config)) {
        qCritical() << "Не удалось загрузить конфиг репозитория";
        return 1;
    }

    git_libgit2_init();

    if (repo_clone(config.url, config.path, config.username, config.token) != 0) {
        printf("Не удалось подключиться к репозиторию\n");
    } else {
        printf("Подключение прошло успешно!\n");
    }


    const QString path_file = config.path + "\\text.txt";
    FILE *f = fopen(path_file.toUtf8().constData(),"w");

    Repo rep = Repo(config.path);
    QList<FileStatus> list = rep.status();

    if(list.isEmpty()){
        printf("Нет файлов в status");
    }else{
        foreach(FileStatus value, list) {
            qDebug() << value.path_new;
        }
    }

    git_libgit2_shutdown();

    fclose(f);
    return a.exec();
}
