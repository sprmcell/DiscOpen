#pragma once

#include "client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <thread>
#include "serevr_fm.h"

#include <array>

#define DEFAULT_PORT 8080
#define MAX_CLIENTS 10
#define MSG_SIZE 256


class ServerFileManager;

class Server
{
private:
    int network_socket;
    int port;
    sockaddr_in address;
    bool running = false;
    std::array<Client, MAX_CLIENTS> client_array;
    std::array<std::thread, MAX_CLIENTS> client_threads;
    ServerFileManager* files;

private:
    bool init_server();

    static void handle_client(Server* server, Client* client);
    static void recv_handler(Server* server, Client* client, Packet* packet);

    void shutdown();

public:
    Server();
    Server(int server_port);
    ~Server();

    int run();
};