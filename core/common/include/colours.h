#pragma once

#include <string>

/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BOLD "\x1B[1m"
#define UNDL "\x1B[4m" 


inline std::string bold(const std::string& str) 
{
    return BOLD + str + RST;
}

inline std::string green(const std::string& str)
{
    return KGRN + str + RST;
}

inline std::string red(const std::string& str)
{
    return KRED + str + RST;
}