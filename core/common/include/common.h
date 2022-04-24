#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <pwd.h>

const char* homedir();
const char* cache_dir();

u_int8_t random_8_id();

u_int64_t random_64_id();