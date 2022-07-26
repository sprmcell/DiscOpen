#include <iostream>

#include "server.h"
#include <signal.h>


int main(int argc, const char* argv[])
{
    // TODO: Add cmd args to server
    Server server = Server();

    int exit_code = server.run();

    return exit_code;
}