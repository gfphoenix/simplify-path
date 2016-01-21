#include "filepath.hpp"
#include <stdio.h>
#include <string.h>

Path::Path(const char *path)
{
    reset(path);
    merge_ = true;
    avoidFirstMove_ = true;
}
void Path::reset(const char *path)
{
    int n = strlen(path);
    char *p = &path_[0];
    len_ = n;
    if(path[0]!='/'){
        *p++ = '/';
        len_++;
    }
    memcpy(p, path, n+1);
    dirty_ = true;
}

const char *Path::path()const
{
    return path_;
}
void Path::trimEndSlash()
{
    while(len_>1){
        if(path_[len_-1] != '/')
            break;
        path_[--len_] = '\0';
    }
}
bool Path::appendSlash()
{
    if(len_+1>=sizeof(path_))
        return false;
    path_[len_++] = '/';
    path_[len_] = '\0';
    dirty_ = true;
    return true;
}

static inline bool isDotDot(const char *p)
{
    return p[0]=='.' && p[1]=='.';
}

// there are two `if` blocks(i.e. #1 and #2) which exist for optimization,
// it is safe to remove them if you feel it unnecessary!
// using both of them can avoid memmove if possible
Path *Path::simplify()
{
    // if 128 path segments are not enough, replace the array(s) with other arrays/vectors
    size_t start[128];
    size_t len[128];
    size_t idx=0;
    size_t i;
    size_t j=0;
    int I=-1; // I+1: the new path segment count after resolves '.' and '..'
    int k=0;
    if(!dirty_)
        return this;

    for(i=0; i<len_ && path_[i]=='/'; i++)
        ;
    if(i==len_){
        goto out;
    }
    // make the last '\0' to '/',
    // so the path is like /+([^/]+/+)*
    path_[len_++] = '/';
    // splite ([^/]+/+)* here
    start[idx] = i;
    for(; i<len_; i++){
        while(path_[i]!='/')
            i++;
        len[idx] = i - start[idx];
        idx++;
        while(i<len_ && path_[i]=='/')
            i++;
        start[idx] = i;
    }

    // handles '.' and '..' correctly here
    for(i=0; i<idx; i++){
        if(len[i]==1 && path_[start[i]]=='.')
            continue;
        if(len[i]==2 && isDotDot(&path_[start[i]])){
            if(I>=0)
                I--;
            continue;
        }
        I++;
        start[I] = start[i];
        len[I] = len[i];
    }
    // merge segments as more as possible to decrease memmove count
    // #1
    if(merge_ && I>=1){
        int mi=0;
        for(int kk=1; kk<=I; kk++){
            if(start[kk] == start[mi]+len[mi]+1){
                len[mi] += len[kk]+1;
            }else{
                mi++;
                start[mi] = start[kk];
                len[mi] = len[kk];
            }
        }
        I = mi;
    }
    // avoid the first memmove if possible
    // #2
    if(avoidFirstMove_ && I>=0){
        if(start[0]==1){
            j += len[0]+1;
        }else{
            memmove(&path_[j], &path_[start[0]-1], len[0]+1);
            j += len[0]+1;
        }
        k = 1;
    }
    // move path segments
    for(; k<=I; k++){
        memmove(&path_[j], &path_[start[k]-1], len[k]+1);
        j += len[k]+1;
    }

out:
    if(j==0)
        j = 1;
    path_[j] = '\0';
    len_ = j;
    dirty_ = false;
    return this;
}

bool Path::check(const char *path)
{
    if(path == NULL)
        return false;
    size_t len = strlen(path);
    if(len<1 || path[0]!='/')
        return false;

    const char *s = strstr(path, "//");
    if(s!=NULL)
        return false;
    s = strstr(path, "/./");
    if(s!=NULL)
        return false;
    s = strstr(path, "/../");
    if(s!=NULL)
        return false;
    // end with '/', '/.', or '/..'
    if(len>=2){
        if(path[len-1]=='/')
            return false;
        if(path[len-1]=='.'){
            if(path[len-2]=='/')
                return false;
            if(len>=3 && path[len-2]=='.' && path[len-3]=='/')
                return false;
        }
    }
    return true;
}

#define B2S(ok) ((ok)?"true":"false")
void Path::test(const char *path)
{
    Path p(path);
    //Path b=p;
    //b.trimEnd();
    //printf("trimed = '%s'\n", b.path());
    printf("old path = '%s'  -> ", p.path());
    p.simplify();
    printf("simplified = '%s' check=%s\n", p.path(), B2S(p.check()));
}

bool Path::join(const char *path)
{
    if(path==NULL)
        return false;
    return join(path, strlen(path));
}
bool Path::join2(const void *p, size_t mostLen)
{
    if(p==NULL)
        return false;
    const char *path = (const char *)p;
    size_t len=0;
    while(len<mostLen && path[len]!='\0'){
        len++;
    }
    return join(p, len);
}

bool Path::join(const void *p, size_t len)
{
    const char *path = (const char *)p;
    if(p == NULL || len==0 || len_+1>=sizeof(path_))
        return false;

    if(path_[len_-1]!='/' && path[0]!='/'){
        path_[len_++] = '/';
        path_[len_] = '\0';
    }
    dirty_ = true;
    if(len_+len>=sizeof(path_))
        return false;
    strcat(path_, path);
    return true;
}
