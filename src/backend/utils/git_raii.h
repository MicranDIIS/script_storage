#ifndef GIT_RAII_H
#define GIT_RAII_H

#include <git2.h>

struct GitBranchIteratorDeleter{
    static void cleanup(git_branch_iterator* ptr){
        if(ptr) git_branch_iterator_free(ptr);
    }
};

struct GitIndexDeleter{
    static void cleanup(git_index* ptr){
        if(ptr) git_index_free(ptr);
    }
};

struct GitIndexConflictIteratorDeleter{
    static void cleanup(git_index_conflict_iterator* ptr){
        if(ptr) git_index_conflict_iterator_free(ptr);
    }
};

struct GitConfigDeleter{
    static void cleanup(git_config* ptr){
        if(ptr) git_config_free(ptr);
    }
};

struct GitOdbDeleter{
    static void cleanup(git_odb* ptr){
        if(ptr) git_odb_free(ptr);
    }
};

struct GitOdbObjectDeleter{
    static void cleanup(git_odb_object* ptr){
        if(ptr) git_odb_object_free(ptr);
    }
};

struct GitRepositoryDeleter{
    static void cleanup(git_repository* ptr){
        if(ptr) git_repository_free(ptr);
    }
};

struct GitRemoteDeleter{
    static void cleanup(git_remote* ptr){
        if(ptr) git_remote_free(ptr);
    }
};

struct GitObjectDeleter{
    static void cleanup(git_object* ptr){
        if(ptr) git_object_free(ptr);
    }
};

struct GitCommitDeleter{
    static void cleanup(git_commit* ptr){
        if(ptr) git_commit_free(ptr);
    }
};

struct GitTreeDeleter{
    static void cleanup(git_tree* ptr){
        if(ptr) git_tree_free(ptr);
    }
};

struct GitDiffDeleter{
    static void cleanup(git_diff* ptr){
        if(ptr) git_diff_free(ptr);
    }
};

struct GitRevwalkDeleter{
    static void cleanup(git_revwalk* ptr){
        if(ptr) git_revwalk_free(ptr);
    }
};

struct GitStatusListDeleter{
    static void cleanup(git_status_list* ptr){
        if(ptr) git_status_list_free(ptr);
    }
};

struct GitReferenceDeleter{
    static void cleanup(git_reference* ptr){
        if(ptr) git_reference_free(ptr);
    }
};

struct GitAnnotatedCommitDeleter{
    static void cleanup(git_annotated_commit* ptr){
        if(ptr) git_annotated_commit_free(ptr);
    }
};

struct GitBlobDeleter{
    static void cleanup(git_blob* ptr){
        if(ptr) git_blob_free(ptr);
    }
};

struct GitSignatureDeleter{
    static void cleanup(git_signature* ptr){
        if(ptr) git_signature_free(ptr);
    }
};

struct GitBufDeleter{
    static void cleanup(git_buf* ptr){
        if(ptr) git_buf_dispose(ptr);
    }
};

template<typename T, typename Deleter>
class GitPtr{
private:
    T* ptr_;

    GitPtr(const GitPtr&);
    GitPtr& operator=(const GitPtr&);

public:
    explicit GitPtr(T* ptr = NULL) : ptr_(ptr) {}
    ~GitPtr(){
        if(ptr_) Deleter::cleanup(ptr_);
    }

    T* get() const{
        return ptr_;
    }

    T** operator&(){
        return &ptr_;
    }

    bool isNull() const{
        return ptr_ == NULL;
    }

    T* release(){
        T* tmp = ptr_;
        ptr_ = NULL;
        return tmp;
    }

    void reset(T* ptr = NULL){
        if(ptr_ != ptr){
            if(ptr_) Deleter::cleanup(ptr_);
            ptr_ = ptr;
        }
    }

    operator bool() const{
        return ptr_ != NULL;
    }
};

typedef GitPtr<git_branch_iterator,         GitBranchIteratorDeleter>         GitBranchIteratorPtr;
typedef GitPtr<git_index,                   GitIndexDeleter>                   GitIndexPtr;
typedef GitPtr<git_index_conflict_iterator, GitIndexConflictIteratorDeleter>   GitIndexConflictIteratorPtr;
typedef GitPtr<git_config,                  GitConfigDeleter>                  GitConfigPtr;
typedef GitPtr<git_odb,                     GitOdbDeleter>                     GitOdbPtr;
typedef GitPtr<git_odb_object,              GitOdbObjectDeleter>               GitOdbObjectPtr;
typedef GitPtr<git_repository,              GitRepositoryDeleter>              GitRepositoryPtr;
typedef GitPtr<git_remote,                  GitRemoteDeleter>                  GitRemotePtr;
typedef GitPtr<git_object,                  GitObjectDeleter>                  GitObjectPtr;
typedef GitPtr<git_commit,                  GitCommitDeleter>                  GitCommitPtr;
typedef GitPtr<git_tree,                    GitTreeDeleter>                    GitTreePtr;
typedef GitPtr<git_diff,                    GitDiffDeleter>                    GitDiffPtr;
typedef GitPtr<git_revwalk,                 GitRevwalkDeleter>                 GitRevwalkPtr;
typedef GitPtr<git_status_list,             GitStatusListDeleter>              GitStatusListPtr;
typedef GitPtr<git_reference,               GitReferenceDeleter>               GitReferencePtr;
typedef GitPtr<git_annotated_commit,        GitAnnotatedCommitDeleter>         GitAnnotatedCommitPtr;
typedef GitPtr<git_blob,                    GitBlobDeleter>                    GitBlobPtr;
typedef GitPtr<git_signature,               GitSignatureDeleter>               GitSignaturePtr;
typedef GitPtr<git_buf,                     GitBufDeleter>                     GitBufPtr;

#endif
