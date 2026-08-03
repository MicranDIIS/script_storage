#include "repository.h"

DiffLine::DiffLine(LineType type_, int oldNum_, int newNum_, const QString &text_) :
    line(type_), oldNum(oldNum_), newNum(newNum_), text(text_) {}

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

    if (line->origin == 'H') {
        DiffHunk newHunk;
        newHunk.oldStart = hunk->old_start;
        newHunk.oldLines = hunk->old_lines;
        newHunk.newStart = hunk->new_start;
        newHunk.newLines = hunk->new_lines;
        newHunk.header = QString::fromUtf8(hunk->header, hunk->header_len);
        result->hunks.append(newHunk);
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

static int hunkCallback(
    const git_diff_delta *delta,
    const git_diff_hunk *hunk,
    void *payload)
{
    (void)delta;
    (void)hunk;
    (void)payload;
    return 0;
}

GitError Repository::fillDiff(DiffResult &diffResult) const {
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    opts.flags = GIT_DIFF_IGNORE_WHITESPACE;

    GitCommitPtr head_commit;
    if (git_revparse_single((git_object**)&head_commit, repo_, "HEAD") != GIT_OK) {
        return libgitError();
    }

    GitCommitPtr parent_commit;
    int parent_result = git_commit_parent(&parent_commit, head_commit.get(), 0);

    GitTreePtr head_tree;
    if (git_commit_tree(&head_tree, head_commit.get()) != GIT_OK) {
        return libgitError();
    }

    GitTreePtr parent_tree;

    if (parent_result == GIT_ENOTFOUND) {
        git_oid empty_oid;
        git_oid_fromstr(&empty_oid, "4b825dc642cb6eb9a060e54bf8d69288fbee4904");
        if (git_tree_lookup(&parent_tree, repo_, &empty_oid) != GIT_OK) {
            return libgitError();
        }
    } else if (parent_result != GIT_OK) {
        return libgitError();
    } else {
        if (git_commit_tree(&parent_tree, parent_commit.get()) != GIT_OK) {
            return libgitError();
        }
    }

    GitDiffPtr diff;
    if (git_diff_tree_to_tree(&diff, repo_, parent_tree.get(), head_tree.get(), &opts) != GIT_OK) {
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

    const git_signature *head_author = git_commit_author(head_commit.get());
    diffResult.newCommit.author = QString::fromUtf8(head_author->name);
    qint64 time_ms = static_cast<qint64>(head_author->when.time) * 1000;
    diffResult.newCommit.date = QDateTime::fromMSecsSinceEpoch(time_ms);

    if (parent_result == GIT_ENOTFOUND) {
        diffResult.oldCommit.author = "Initial commit";
        diffResult.oldCommit.date = QDateTime::fromMSecsSinceEpoch(0);
    } else {
        const git_signature *parent_author = git_commit_author(parent_commit.get());
        diffResult.oldCommit.author = QString::fromUtf8(parent_author->name);
        qint64 time_ms = static_cast<qint64>(parent_author->when.time) * 1000;
        diffResult.oldCommit.date = QDateTime::fromMSecsSinceEpoch(time_ms);
    }

    return GitError();
}
