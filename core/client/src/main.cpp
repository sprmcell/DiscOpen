#include <iostream>

#include "client_server.h"
#include <unistd.h>
#include <string>
#include "cli.h"
#include <thread>
#include "common.h"

int main(int argc, const char* argv[])
{
    // TODO: Add args
    Server server;
    Cli cli = Cli(&server);

    if (server.is_connected())
    {
        std::thread recv_handler_thread = std::thread(Server::recv_handler, &server);

        cli.run();
        recv_handler_thread.join();
    }
    else
        return EXIT_FAILURE;
    
    return EXIT_SUCCESS;
}