#include "repository.h"
#include <QSet>
#include <QMap>
#include <QVector>

static bool hasUncommittedChanges(git_repository* repo) {
    GitStatusListPtr statusList;
    git_status_options statusOpts = GIT_STATUS_OPTIONS_INIT;
    statusOpts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;

    if (git_status_list_new(&statusList, repo, &statusOpts) != GIT_OK) {
        return true;
    }

    return git_status_list_entrycount(statusList.get()) > 0;
}

static GitError checkoutToBranch(git_repository* repo, const char* branchRef, unsigned int checkoutStrategy) {
    GitObjectPtr treeish;
    if (git_revparse_single(&treeish, repo, branchRef) != GIT_OK) {
        return libgitError();
    }

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = checkoutStrategy | GIT_CHECKOUT_RECREATE_MISSING;

    if (git_checkout_tree(repo, treeish.get(), &opts) != GIT_OK) {
        return libgitError();
    }

    if (git_repository_set_head(repo, branchRef) != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

static GitError checkoutToBranchSafe(git_repository* repo, const char* branchRef) {
    return checkoutToBranch(repo, branchRef, GIT_CHECKOUT_SAFE);
}

static void addAllFilesFromTree(git_repository* repo, const git_tree_entry* treeEntry,
                                const QString& prefix, QSet<QString>& files) {
    GitTreePtr tree;
    if (git_tree_lookup(&tree, repo, git_tree_entry_id(treeEntry)) != GIT_OK) {
        return;
    }

    size_t count = git_tree_entrycount(tree.get());
    for (size_t i = 0; i < count; ++i) {
        const git_tree_entry* entry = git_tree_entry_byindex(tree.get(), i);
        QString name = QString::fromUtf8(git_tree_entry_name(entry));
        QString path = prefix + "/" + name;

        if (git_tree_entry_type(entry) == GIT_OBJECT_BLOB) {
            files.insert(path);
        } else if (git_tree_entry_type(entry) == GIT_OBJECT_TREE) {
            addAllFilesFromTree(repo, entry, path, files);
        }
    }
}

static QSet<QString> getAllFilesFromCommit(git_repository* repo, git_commit* commit) {
    QSet<QString> files;
    GitTreePtr tree;
    if (git_commit_tree(&tree, commit) != GIT_OK) {
        return files;
    }

    size_t count = git_tree_entrycount(tree.get());
    for (size_t i = 0; i < count; ++i) {
        const git_tree_entry* entry = git_tree_entry_byindex(tree.get(), i);
        QString name = QString::fromUtf8(git_tree_entry_name(entry));

        if (git_tree_entry_type(entry) == GIT_OBJECT_BLOB) {
            files.insert(name);
        } else if (git_tree_entry_type(entry) == GIT_OBJECT_TREE) {
            addAllFilesFromTree(repo, entry, name, files);
        }
    }

    return files;
}

static void collectProtectedFilesRecursive(git_repository* repo,
                                           git_tree* baseTree,
                                           git_tree* debugTree,
                                           const QString& prefix,
                                           QSet<QString>& protectedFiles) {
    size_t baseCount = git_tree_entrycount(baseTree);
    size_t debugCount = git_tree_entrycount(debugTree);

    QMap<QString, const git_tree_entry*> baseEntries;
    for (size_t i = 0; i < baseCount; ++i) {
        const git_tree_entry* entry = git_tree_entry_byindex(baseTree, i);
        baseEntries[QString::fromUtf8(git_tree_entry_name(entry))] = entry;
    }

    for (size_t i = 0; i < debugCount; ++i) {
        const git_tree_entry* debugEntry = git_tree_entry_byindex(debugTree, i);
        QString name = QString::fromUtf8(git_tree_entry_name(debugEntry));
        QString path = prefix.isEmpty() ? name : prefix + "/" + name;

        QMap<QString, const git_tree_entry*>::const_iterator it = baseEntries.find(name);

        if (it == baseEntries.end()) {
            if (git_tree_entry_type(debugEntry) == GIT_OBJECT_BLOB) {
                protectedFiles.insert(path);
            } else if (git_tree_entry_type(debugEntry) == GIT_OBJECT_TREE) {
                addAllFilesFromTree(repo, debugEntry, path, protectedFiles);
            }
        } else {
            const git_tree_entry* baseEntry = it.value();

            if (git_tree_entry_type(debugEntry) != git_tree_entry_type(baseEntry)) {
                if (git_tree_entry_type(debugEntry) == GIT_OBJECT_BLOB) {
                    protectedFiles.insert(path);
                } else if (git_tree_entry_type(debugEntry) == GIT_OBJECT_TREE) {
                    addAllFilesFromTree(repo, debugEntry, path, protectedFiles);
                }
            } else if (git_tree_entry_type(debugEntry) == GIT_OBJECT_TREE) {
                GitTreePtr baseSubTree;
                GitTreePtr debugSubTree;

                if (git_tree_lookup(&baseSubTree, repo, git_tree_entry_id(baseEntry)) == GIT_OK) {
                    if (git_tree_lookup(&debugSubTree, repo, git_tree_entry_id(debugEntry)) == GIT_OK) {
                        collectProtectedFilesRecursive(repo, baseSubTree.get(), debugSubTree.get(), path, protectedFiles);
                    }
                }
            } else if (!git_oid_equal(git_tree_entry_id(debugEntry), git_tree_entry_id(baseEntry))) {
                protectedFiles.insert(path);
            }
        }
    }
}

static QSet<QString> collectProtectedFiles(git_repository* repo,
                                           git_commit* debugCommit,
                                           const git_oid* mergeBaseOid) {
    GitCommitPtr mergeBaseCommit;
    if (git_commit_lookup(&mergeBaseCommit, repo, mergeBaseOid) != GIT_OK) {
        return getAllFilesFromCommit(repo, debugCommit);
    }

    GitTreePtr baseTree;
    GitTreePtr debugTree;

    if (git_commit_tree(&baseTree, mergeBaseCommit.get()) != GIT_OK ||
        git_commit_tree(&debugTree, debugCommit) != GIT_OK) {
        return QSet<QString>();
    }

    QSet<QString> protectedFiles;
    collectProtectedFilesRecursive(repo, baseTree.get(), debugTree.get(), "", protectedFiles);

    return protectedFiles;
}

static GitError restoreProtectedFiles(git_repository* repo,
                                      const QSet<QString>& files,
                                      git_commit* commit) {
    if (files.isEmpty()) {
        return GitError();
    }

    GitTreePtr tree;
    if (git_commit_tree(&tree, commit) != GIT_OK) {
        return libgitError();
    }

    QVector<QByteArray> pathArrays;
    QVector<char*> pathPtrs;
    pathArrays.reserve(files.size());
    pathPtrs.reserve(files.size());

    for (QSet<QString>::const_iterator it = files.begin(); it != files.end(); ++it) {
        pathArrays.append(it->toUtf8());
    }
    for (int i = 0; i < pathArrays.size(); ++i) {
        pathPtrs.append(pathArrays[i].data());
    }

    git_strarray paths;
    paths.strings = pathPtrs.data();
    paths.count = pathPtrs.size();

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_FORCE;
    opts.paths = paths;

    if (git_checkout_tree(repo, (git_object*)tree.get(), &opts) != GIT_OK) {
        return libgitError();
    }

    GitIndexPtr index;
    if (git_repository_index(&index, repo) != GIT_OK) {
        return libgitError();
    }

    for (QSet<QString>::const_iterator it = files.begin(); it != files.end(); ++it) {
        QByteArray filePath = it->toUtf8();
        git_index_remove_bypath(index.get(), filePath.constData());
        git_index_add_bypath(index.get(), filePath.constData());
    }

    if (git_index_write(index.get()) != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

static GitError resolveConflicts(git_repository* repo, const QSet<QString>& /* protectedFiles */) {
    GitIndexPtr index;
    if (git_repository_index(&index, repo) != GIT_OK) {
        return libgitError();
    }

    if (!git_index_has_conflicts(index.get())) {
        return GitError();
    }

    GitIndexConflictIteratorPtr conflictIter;
    if (git_index_conflict_iterator_new(&conflictIter, index.get()) != GIT_OK) {
        return libgitError();
    }

    const git_index_entry* ancestorEntry = NULL;
    const git_index_entry* ourEntry = NULL;
    const git_index_entry* theirEntry = NULL;

    while (git_index_conflict_next(&ancestorEntry, &ourEntry, &theirEntry, conflictIter.get()) == GIT_OK) {
        QString path = QString::fromUtf8(ourEntry->path);
        QByteArray pathBytes = path.toUtf8();

        git_index_remove_bypath(index.get(), pathBytes.constData());
        git_index_add_bypath(index.get(), pathBytes.constData());
    }

    if (git_index_write(index.get()) != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

static GitError createMergeCommit(git_repository* repo, git_commit* basicCommit) {
    GitIndexPtr index;
    if (git_repository_index(&index, repo) != GIT_OK) {
        return libgitError();
    }

    git_oid treeOid;
    if (git_index_write_tree(&treeOid, index.get()) != GIT_OK) {
        return libgitError();
    }

    GitTreePtr tree;
    if (git_tree_lookup(&tree, repo, &treeOid) != GIT_OK) {
        return libgitError();
    }

    git_oid headOid;
    if (git_reference_name_to_id(&headOid, repo, "HEAD") != GIT_OK) {
        return libgitError();
    }

    GitCommitPtr headCommit;
    if (git_commit_lookup(&headCommit, repo, &headOid) != GIT_OK) {
        return libgitError();
    }

    GitSignaturePtr signature;
    if (git_signature_default(&signature, repo) != GIT_OK) {
        return libgitError();
    }

    const git_commit* parents[2] = { headCommit.get(), basicCommit };

    git_oid commitOid;
    int error = git_commit_create(
        &commitOid,
        repo,
        "HEAD",
        signature.get(),
        signature.get(),
        "UTF-8",
        "Auto-merge from main branch to debug",
        tree.get(),
        2,
        parents
    );

    if (error == GIT_OK) {
        git_repository_state_cleanup(repo);
        return GitError();
    }

    return libgitError();
}

static GitError abortMergeAndRestore(git_repository* repo, const git_oid& originalDebugOid) {
    git_repository_state_cleanup(repo);

    GitCommitPtr commit;
    if (git_commit_lookup(&commit, repo, &originalDebugOid) != GIT_OK) {
        return libgitError();
    }

    GitTreePtr tree;
    if (git_commit_tree(&tree, commit.get()) != GIT_OK) {
        return libgitError();
    }

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_FORCE;
    if (git_checkout_tree(repo, (git_object*)tree.get(), &opts) != GIT_OK) {
        return libgitError();
    }

    git_repository_set_head_detached(repo, &originalDebugOid);

    GitIndexPtr index;
    if (git_repository_index(&index, repo) == GIT_OK) {
        git_index_read(index.get(), true);
        git_index_write(index.get());
    }

    return GitError();
}

static GitError createDebugBranch(git_repository* repo,
                                  const QByteArray& basicBranchRef,
                                  const QByteArray& debugBranchName) {
    GitObjectPtr obj;
    if (git_revparse_single(&obj, repo, basicBranchRef.constData()) != GIT_OK) {
        return libgitError();
    }

    if (git_object_type(obj.get()) != GIT_OBJECT_COMMIT) {
        return GitError(QString("Target is not a commit object"), -1);
    }

    GitReferencePtr newDebugRef;
    int error = git_branch_create(&newDebugRef, repo,
                                  debugBranchName.constData(),
                                  (git_commit*)obj.get(), 0);
    if (error != GIT_OK) {
        return libgitError();
    }

    return checkoutToBranchSafe(repo, git_reference_name(newDebugRef.get()));
}

static GitError updateDebugBranch(git_repository* repo,
                                  const QByteArray& basicBranchRef,
                                  const QByteArray& debugBranchRef,
                                  const QByteArray& /* debugBranchName */) {
    GitObjectPtr debugObj;
    GitObjectPtr basicObj;

    if (git_revparse_single(&debugObj, repo, debugBranchRef.constData()) != GIT_OK ||
        git_revparse_single(&basicObj, repo, basicBranchRef.constData()) != GIT_OK) {
        return libgitError();
    }

    if (git_object_type(debugObj.get()) != GIT_OBJECT_COMMIT ||
        git_object_type(basicObj.get()) != GIT_OBJECT_COMMIT) {
        return GitError(QString("Branch does not point to a commit"), -1);
    }

    git_commit* debugCommit = (git_commit*)debugObj.get();
    git_commit* basicCommit = (git_commit*)basicObj.get();

    git_oid mergeBaseOid;
    if (git_merge_base(&mergeBaseOid, repo,
                       git_commit_id(debugCommit),
                       git_commit_id(basicCommit)) != GIT_OK) {
        return libgitError();
    }

    if (git_oid_equal(&mergeBaseOid, git_commit_id(basicCommit))) {
        return checkoutToBranchSafe(repo, debugBranchRef.constData());
    }

    if (hasUncommittedChanges(repo)) {
        return GitError(QString("Cannot update debug branch: there are uncommitted changes"), -1);
    }

    QSet<QString> protectedFiles = collectProtectedFiles(repo, debugCommit, &mergeBaseOid);
    git_oid debugOidBeforeMerge = *git_commit_id(debugCommit);

    GitError checkoutError = checkoutToBranchSafe(repo, debugBranchRef.constData());
    if (!checkoutError.success) {
        return checkoutError;
    }

    git_merge_options mergeOpts = GIT_MERGE_OPTIONS_INIT;
    mergeOpts.file_favor = GIT_MERGE_FILE_FAVOR_NORMAL;

    git_checkout_options checkoutOpts = GIT_CHECKOUT_OPTIONS_INIT;
    checkoutOpts.checkout_strategy = GIT_CHECKOUT_SAFE | GIT_CHECKOUT_ALLOW_CONFLICTS;

    GitAnnotatedCommitPtr annotated;
    if (git_annotated_commit_lookup(&annotated, repo, git_commit_id(basicCommit)) != GIT_OK) {
        return libgitError();
    }

    const git_annotated_commit* refs[] = { annotated.get() };
    int error = git_merge(repo, refs, 1, &mergeOpts, &checkoutOpts);

    if (error != GIT_OK && error != GIT_EMERGECONFLICT) {
        abortMergeAndRestore(repo, debugOidBeforeMerge);
        return libgitError();
    }

    if (!protectedFiles.isEmpty()) {
        GitError restoreError = restoreProtectedFiles(repo, protectedFiles, debugCommit);
        if (!restoreError.success) {
            abortMergeAndRestore(repo, debugOidBeforeMerge);
            return restoreError;
        }
    }

    GitError resolveError = resolveConflicts(repo, protectedFiles);
    if (!resolveError.success) {
        abortMergeAndRestore(repo, debugOidBeforeMerge);
        return resolveError;
    }

    GitIndexPtr index;
    bool hasConflicts = false;
    if (git_repository_index(&index, repo) == GIT_OK) {
        hasConflicts = git_index_has_conflicts(index.get());
    }

    if (!hasConflicts) {
        GitError commitError = createMergeCommit(repo, basicCommit);
        if (!commitError.success) {
            abortMergeAndRestore(repo, debugOidBeforeMerge);
            return commitError;
        }
        return GitError();
    }

    return GitError(QString("Merge conflicts require manual resolution"), 1);
}

GitError Repository::startDebugMode() {
    if (!repo_) {
        return GitError(QString("Repository is not open"), -1);
    }

    int isDetached = git_repository_head_detached(repo_);
    if (isDetached < 0) {
        return libgitError();
    }
    if (isDetached) {
        return GitError(QString("Cannot start debug mode in detached HEAD state"), -1);
    }

    QByteArray debugBranchName("debug");
    QByteArray debugBranchRef = QByteArray("refs/heads/") + debugBranchName;
    QByteArray basicBranchRef = QByteArray("refs/heads/") + cfg_.branch;

    GitReferencePtr head;
    if (git_repository_head(&head, repo_) != GIT_OK) {
        return libgitError();
    }

    GitReferencePtr debugRef;
    int error = git_reference_lookup(&debugRef, repo_, debugBranchRef.constData());

    if (error == GIT_ENOTFOUND) {
        return createDebugBranch(repo_, basicBranchRef, debugBranchName);
    }

    if (error != GIT_OK) {
        return libgitError();
    }

    return updateDebugBranch(repo_, basicBranchRef, debugBranchRef, debugBranchName);
}


GitError Repository::saveDebugFiles(const QString &commitMsg) {
    if (!repo_) {
        return GitError(QString("Repository is not open"), -1);
    }

    int isDetached = git_repository_head_detached(repo_);
    if (isDetached < 0) {
        return libgitError();
    }
    if (isDetached) {
        return GitError(QString("Cannot commit in detached HEAD state"), -1);
    }

    GitReferencePtr head;
    if (git_repository_head(&head, repo_) != GIT_OK) {
        return libgitError();
    }

    const char* branchName;
    if (git_branch_name(&branchName, head.get()) != GIT_OK) {
        return libgitError();
    }

    if (QString::fromUtf8(branchName) != QString("debug")) {
        return GitError(QString("Can only save debug files on debug branch"), -1);
    }

    GitIndexPtr index;
    if (git_repository_index(&index, repo_) != GIT_OK) {
        return libgitError();
    }

    git_strarray paths = {NULL, 0};
    if (git_index_add_all(index.get(), &paths, GIT_INDEX_ADD_DEFAULT, NULL, NULL) != GIT_OK) {
        return libgitError();
    }

    if (git_index_update_all(index.get(), &paths, NULL, NULL) != GIT_OK) {
        return libgitError();
    }

    if (git_index_write(index.get()) != GIT_OK) {
        return libgitError();
    }

    git_oid treeOid;
    if (git_index_write_tree(&treeOid, index.get()) != GIT_OK) {
        return libgitError();
    }

    GitTreePtr newTree;
    if (git_tree_lookup(&newTree, repo_, &treeOid) != GIT_OK) {
        return libgitError();
    }

    git_oid headOid;
    if (git_reference_name_to_id(&headOid, repo_, "HEAD") != GIT_OK) {
        return libgitError();
    }

    GitCommitPtr headCommit;
    if (git_commit_lookup(&headCommit, repo_, &headOid) != GIT_OK) {
        return libgitError();
    }

    GitTreePtr headTree;
    if (git_commit_tree(&headTree, headCommit.get()) != GIT_OK) {
        return libgitError();
    }

    if (git_oid_equal(git_tree_id(newTree.get()), git_tree_id(headTree.get()))) {
        return GitError(QString("Nothing to commit"), 0);
    }

    git_oid parentOid;
    if (git_reference_name_to_id(&parentOid, repo_, "HEAD") != GIT_OK) {
        return libgitError();
    }

    GitCommitPtr parentCommit;
    if (git_commit_lookup(&parentCommit, repo_, &parentOid) != GIT_OK) {
        return libgitError();
    }

    GitSignaturePtr signature;
    if (git_signature_default(&signature, repo_) != GIT_OK) {
        return libgitError();
    }

    const git_commit* parents[1] = { parentCommit.get() };
    QByteArray msgUtf8 = commitMsg.toUtf8();
    QByteArray encoding("UTF-8");

    git_oid commitOid;
    int error = git_commit_create(
        &commitOid,
        repo_,
        "HEAD",
        signature.get(),
        signature.get(),
        encoding.constData(),
        msgUtf8.constData(),
        newTree.get(),
        1,
        parents
    );

    if (error != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

GitError Repository::closeDebugMode() {
    if (!repo_) {
        return GitError(QString("Repository is not open"), -1);
    }

    int isDetached = git_repository_head_detached(repo_);
    if (isDetached < 0) {
        return libgitError();
    }
    if (isDetached) {
        return GitError(QString("Cannot close debug mode in detached HEAD state"), -1);
    }

    GitReferencePtr head;
    if (git_repository_head(&head, repo_) != GIT_OK) {
        return libgitError();
    }

    const char* branchName;
    if (git_branch_name(&branchName, head.get()) != GIT_OK) {
        return libgitError();
    }

    if (QString::fromUtf8(branchName) != QString("debug")) {
        return GitError(QString("Can only close debug mode from debug branch"), -1);
    }

    if (hasUncommittedChanges(repo_)) {
        return GitError(QString("Cannot close debug mode: there are uncommitted changes. Commit or stash them first"), -1);
    }

    QByteArray basicBranchRef = QByteArray("refs/heads/") + cfg_.branch;

    GitObjectPtr basicObj;
    if (git_revparse_single(&basicObj, repo_, basicBranchRef.constData()) != GIT_OK) {
        return libgitError();
    }

    if (git_object_type(basicObj.get()) != GIT_OBJECT_COMMIT) {
        return GitError(QString("Target branch does not point to a commit"), -1);
    }

    git_checkout_options opts = GIT_CHECKOUT_OPTIONS_INIT;
    opts.checkout_strategy = GIT_CHECKOUT_SAFE | GIT_CHECKOUT_RECREATE_MISSING;

    if (git_checkout_tree(repo_, basicObj.get(), &opts) != GIT_OK) {
        return libgitError();
    }

    if (git_repository_set_head(repo_, basicBranchRef.constData()) != GIT_OK) {
        return libgitError();
    }

    return GitError();
}

