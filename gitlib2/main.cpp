#include <QtCore/QCoreApplication>
#include <QLibrary>
#include <QDebug>
#include <git2/repository.h>
#include <git2.h>



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    git_repository *repo = NULL;
    int error;

    git_libgit2_init();
    error = git_repository_init(&repo, "test_repo", 0);
    if (error < 0) {
        const git_error *e = giterr_last();
        printf("Error: %s\n", e->message);
    } else {
        printf("Success\n");
        git_repository_free(repo);
    }
    git_libgit2_shutdown();

    return a.exec();
}
