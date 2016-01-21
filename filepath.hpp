#ifndef PATH_HPP
#define PATH_HPP
#include <stddef.h>

class Path
{
    size_t len_;
    char path_[2048];
    bool dirty_;
    bool merge_;
    bool avoidFirstMove_;

    public:
    Path(const char *path="/");
    const char *path()const;
    Path *simplify();
    bool join(const char *path);
    // the P block must not contains '\0',
    // if not sure, use join2
    bool join(const void *p, size_t len);
    // join P block with len=min(strlen(p), mostlen)
    bool join2(const void *p, size_t mostLen);

    void reset(const char *path="/");
    void trimEndSlash();
    bool appendSlash();
    inline void enableMerge(bool enable){merge_=enable;}
    inline void enableAvoidFirstMove(bool enable){avoidFirstMove_=enable;}
    inline bool isEnabledMerge()const{return merge_;}
    inline bool isEnabledAvoidFirstMove()const{return merge_;}
    inline void dirtyPath(){dirty_=true;}

    inline const char *cleanPath(){return simplify()->path();}
    inline bool check()const{return check(path_);}

    static bool check(const char *path);
    static void test(const char *path);
};

#endif // PATH_HPP
