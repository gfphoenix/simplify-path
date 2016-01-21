#include "filepath.hpp"
#include <stdio.h>

int main(int ac, const char *av[])
{
    const char *samples[] = {
        "/a/bb/c/./def/e/../hi",
        "/.",
        "/..",
        "/./a",
        "/../b",
        "//../a/./../bc",
        "abc",
        "",
    };
    int N = sizeof(samples)/sizeof(char*);
    if(ac==1){
        for(int i=0; i<N; i++){
            Path::test(samples[i]);
        }
    }else{
        for(int i=1; i<ac; i++){
            Path::test(av[i]);
        }
    }
    return 0;
}
