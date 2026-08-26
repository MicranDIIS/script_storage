#include "repository.h"

DiffLine::DiffLine(LineType type_, int oldNum_, int newNum_, const QString &text_) :
    type(type_), oldNum(oldNum_), newNum(newNum_), text(text_) {}

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
static int get_two_last_commits_for_file(GitCommitPtr& latest_commit,
                                         GitCommitPtr& previous_commit,
                                         const GitRevwalkPtr& walker,
                                         git_repository* repo,
                                         const char* filePath) {
    bool found_first = false;

    while (true) {
        git_oid walk_oid;
        int walk_code = git_revwalk_next(&walk_oid, walker.get());
        if (walk_code == GIT_ITEROVER) {
            return found_first ? 0 : -2;
        } else if (walk_code != GIT_OK) {
            return -1;
        }

        GitCommitPtr curr_commit;
        if (git_commit_lookup(&curr_commit, repo, &walk_oid) != GIT_OK) {
            return -1;
        }

        unsigned int parent_count = git_commit_parentcount(curr_commit.get());
        if (parent_count == 0) {
            if (!found_first) {
                latest_commit.reset(curr_commit.release());
                return 0;
            }
            continue;
        }

        GitCommitPtr parent_commit;
        if (git_commit_parent(&parent_commit, curr_commit.get(), 0) != GIT_OK) {
            return -1;
        }

        GitTreePtr curr_tree, parent_tree;
        if (git_commit_tree(&curr_tree, curr_commit.get()) != GIT_OK ||
            git_commit_tree(&parent_tree, parent_commit.get()) != GIT_OK) {
            return -1;
        }

        git_tree_entry* curr_entry = NULL;
        git_tree_entry* parent_entry = NULL;

        int curr_err = git_tree_entry_bypath(&curr_entry, curr_tree.get(), filePath);
        int parent_err = git_tree_entry_bypath(&parent_entry, parent_tree.get(), filePath);

        bool file_changed = false;

        if (curr_err == GIT_OK && parent_err == GIT_OK) {
            if (!git_oid_equal(git_tree_entry_id(parent_entry), git_tree_entry_id(curr_entry))) {
                file_changed = true;
            }
        } else if (curr_err == GIT_OK && parent_err != GIT_OK) {
            file_changed = true;
        }

        if (curr_entry) git_tree_entry_free(curr_entry);
        if (parent_entry) git_tree_entry_free(parent_entry);

        if (file_changed) {
            if (!found_first) {
                latest_commit.reset(curr_commit.release());
                found_first = true;
            } else {
                previous_commit.reset(curr_commit.release());
                return 0;
            }
        }
    }
}


GitError Repository::fillDiff(DiffResult &diffResult, const QString& filePath) const {
    if (repo_ == NULL) {
        return GitError("repo is NULL", REPO_IS_NULL);
    }

    QByteArray filePath_ = filePath.toUtf8();
    if (filePath_.startsWith('/')) {
        filePath_ = filePath_.mid(1);
    }

    GitRevwalkPtr walker;
    GitError err = GitRevwalkInit(walker);
    if (!err.success) {
        return libgitError();
    }

    GitCommitPtr latest_commit;
    GitCommitPtr previous_commit;

    int check = get_two_last_commits_for_file(latest_commit, previous_commit,
                                              walker, repo_, filePath_.constData());
    if (check == -1) {
        return libgitError();
    } else if (check == -2 || !latest_commit.get()) {
        return GitError("No commits found for this file", -101);
    }

    GitTreePtr latest_tree;
    GitTreePtr previous_tree;

    if (git_commit_tree(&latest_tree, latest_commit.get()) != GIT_OK) {
        return libgitError();
    }

    const git_signature* latest_sig = git_commit_author(latest_commit.get());
    diffResult.newCommit.author = QString::fromUtf8(latest_sig->name);
    diffResult.newCommit.date = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(latest_sig->when.time) * 1000);
    diffResult.newCommit.message = QString::fromUtf8(git_commit_message(latest_commit.get()));

    if (previous_commit.get()) {
        const git_signature* prev_sig = git_commit_author(previous_commit.get());
        diffResult.oldCommit.author = QString::fromUtf8(prev_sig->name);
        diffResult.oldCommit.date = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(prev_sig->when.time) * 1000);
        diffResult.oldCommit.message = QString::fromUtf8(git_commit_message(previous_commit.get()));

        if (git_commit_tree(&previous_tree, previous_commit.get()) != GIT_OK) {
            return libgitError();
        }
    } else {
        diffResult.oldCommit.author = "-";
        diffResult.oldCommit.message = "Initial commit for this file";
    }

    GitDiffPtr diff;
    git_diff_options opts = GIT_DIFF_OPTIONS_INIT;
    const char* pathStr = filePath_.constData();
    char* pathArray[] = {const_cast<char*>(pathStr)};

    git_strarray paths;
    paths.count = 1;
    paths.strings = pathArray;
    opts.pathspec = paths;

    if (git_diff_tree_to_tree(&diff, repo_, previous_tree.get(), latest_tree.get(), &opts) != GIT_OK) {
        return libgitError();
    }

    diffResult.hunks.clear();

    if (git_diff_foreach(diff.get(), NULL, NULL, hunkCallback, diffCallback, &diffResult) != GIT_OK) {
         return libgitError();
    }

    return GitError();
}
