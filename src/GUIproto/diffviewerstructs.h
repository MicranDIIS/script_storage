#ifndef DIFFVIEWERSTRUCTS_H
#define DIFFVIEWERSTRUCTS_H

#include <QString>

struct CommitMeta
{
    QString author;
    QString date; //qtime
};

enum LineType
{
    Context,
    Add,
    Del
};

struct DiffLine
{
    LineType type;
    int oldNum;
    int newNum;
    QString text;

    DiffLine() {}

    DiffLine(LineType t, int oldN, int newN, const QString& txt)
        : type(t), oldNum(oldN), newNum(newN), text(txt)
    {}
};

struct DiffHunk
{
    int oldStart;
    int oldLines;
    int newStart;
    int newLines;

    QString header;
    QList<DiffLine> lines;
};

struct DiffResult
{
   // QString filePath;
    CommitMeta oldCommit;
    CommitMeta newCommit;
    QVector<DiffHunk> hunks; //qlist
};

#endif // DIFFVIEWERSTRUCTS_H
