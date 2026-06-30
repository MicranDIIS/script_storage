#ifndef GIT_RAII_H
#define GIT_RAII_H

#include <git2.h>

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
};

typedef GitPtr<git_repository, GitRepositoryDeleter> GitRepositoryPtr;
typedef GitPtr<git_remote, GitRemoteDeleter> GitRemotePtr;
typedef GitPtr<git_object, GitObjectDeleter> GitObjectPtr;
typedef GitPtr<git_commit,      GitCommitDeleter>      GitCommitPtr;
typedef GitPtr<git_tree,        GitTreeDeleter>        GitTreePtr;
typedef GitPtr<git_diff,        GitDiffDeleter>        GitDiffPtr;
typedef GitPtr<git_revwalk,     GitRevwalkDeleter>     GitRevwalkPtr;
typedef GitPtr<git_status_list, GitStatusListDeleter>  GitStatusListPtr;
typedef GitPtr<git_reference,   GitReferenceDeleter>   GitReferencePtr;

#endif 