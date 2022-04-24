#include "common.h"


const char* homedir()
{
    return getenv("HOME");
}

const char* cache_dir()
{
    if (!getenv("XDG_CACHE_HOME"))
        putenv("XDG_CACHE_HOME=$HOME/.cache");
    return getenv("XDG_CACHE_HOME");
}

u_int8_t random_8_id()
{
    uint8_t num = rand() % 255 + 1;
    return num;
}

u_int64_t random_64_id()
{
    uint64_t num = rand() % 999999999999 + 1;
    return num;
}