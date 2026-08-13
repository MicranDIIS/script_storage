#include "repository.h"

DiffLine::DiffLine(LineType type_, int oldNum_, int newNum_, const QString &text_) :
    line(type_), oldNum(oldNum_), newNum(newNum_), text(text_) {}

static int hunkCallback(
    const git_diff_delta *delta,
    const git_diff_hunk *hunk,
    void *payload)
{
    DiffResult *result = static_cast<DiffResult*>(payload);

    DiffHunk newHunk;
    newHunk.oldStart = hunk->old_start;
    newHunk.oldLines = hunk->old_lines;
    newHunk.newStart = hunk->new_start;
    newHunk.newLines = hunk->new_lines;
    newHunk.header = QString::fromUtf8(hunk->header, hunk->header_len);
    result->hunks.append(newHunk);

    return 0;
}

static int diffCallback(
    const git_diff_delta *delta,
    const git_diff_hunk *hunk,
    const git_diff_line *line,
    void *payload)
{
    DiffResult *result = static_cast<DiffResult*>(payload);

    if (!line || !line->content) {
        return 0;
    }


    if (line->origin == 'F') {
        return 0;
    }

    QString text = QString::fromUtf8(line->content, line->content_len);
    if (text.endsWith('\n')) text.chop(1);
    if (text.endsWith('\r')) text.chop(1);

    LineType type = CONTEXT;
    int oldNum = line->old_lineno;
    int newNum = line->new_lineno;

    if (line->origin == '+') {
        type = ADD;
        oldNum = -1;
    } else if (line->origin == '-') {
        type = DEL;
        newNum = -1;
    } else if (line->origin != ' ') {
        return 0;
    }

    DiffLine diffLine(type, oldNum, newNum, text);
    if (!result->hunks.isEmpty()) {
        result->hunks.last().lines.append(diffLine);
    }

    return 0;
}

GitError Repository::fillDiff(DiffResult &diffResult, const QString& filePath) const{
    if(repo_ == NULL){
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    GitIndexPtr index;
    QByteArray filePath_ = filePath.toUtf8();
    if(git_repository_index(&index, repo_) != GIT_OK){
        return libgitError();
    }

    const git_index_entry* entry = git_index_get_bypath(index.get(), filePath_.constData(),
                                                        GIT_INDEX_STAGE_NORMAL);
    if(entry == NULL){
        return GitError("not found file", -1);
    }

    GitCommitPtr master_commit;
    GitCommitPtr slave_commit;

    if(git_revparse_single((git_object**)&slave_commit, repo_, HEAD) != GIT_OK){
        return libgitError();
    }

    if(git_commit_parent(&master_commit, slave_commit.get(), 0) != GIT_OK){
        return libgitError();
    }

    const git_signature* master_signature = git_commit_author(master_commit.get());
    const git_signature* slave_signature = git_commit_author(slave_commit.get());

    diffResult.oldCommit.author = QString::fromUtf8(master_signature->name);
    diffResult.newCommit.author = QString::fromUtf8(slave_signature->name);
    qint64 time_ms = static_cast<qint64>(master_signature->when.time) * 1000;
    diffResult.oldCommit.date = QDateTime::fromMSecsSinceEpoch(time_ms);
    time_ms = static_cast<qint64>(slave_signature->when.time) * 1000;
    diffResult.newCommit.date = QDateTime::fromMSecsSinceEpoch(time_ms);
    diffResult.oldCommit.message = QString::fromUtf8(git_commit_message(master_commit.get()));
    diffResult.newCommit.message = QString::fromUtf8(git_commit_message(slave_commit.get()));

    GitTreePtr master_tree;
    GitTreePtr slave_tree;
    if(git_commit_tree(&master_tree, master_commit.get()) != GIT_OK){
        return libgitError();
    }
    if(git_commit_tree(&slave_tree, slave_commit.get()) != GIT_OK){
        return libgitError();
    }

    GitDiffPtr diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;

    const char* pathspec_array[] = { filePath_.constData() };

    opts.pathspec.count = 1;
    opts.pathspec.strings = (char**)pathspec_array;

    if(git_diff_tree_to_tree(&diff, repo_, master_tree.get(), slave_tree.get(), &opts) != GIT_OK){
        return libgitError();
    }

    diffResult.hunks.clear();

    if (git_diff_foreach(diff.get(),
                          NULL,
                          NULL,
                          hunkCallback,
                          diffCallback,
                          &diffResult) != GIT_OK) {
         return libgitError();
     }

    return GitError();
}
