#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "packet.h"
#include <functional>
#include <map>

#define DEFAULT_PORT 8080
#define DEFAULT_IPV4 "127.0.0.1"

class Server
{
private:
    int network_socket;
    sockaddr_in addr;
    bool connected = false;
    int server_port;
    char ipv4[INET_ADDRSTRLEN];

private:
    void Init();

public:
    Server();
    Server(int port, const char* ipv4_c);
    ~Server();
    Server(const Server&) = delete; // NO COPY

    bool Connect();
    bool is_connected() const { return connected; }
    int get_socket() const { return network_socket; }

    bool Send(const Opts opts, const char* data, const uint64_t to_who=0) const;
    bool create_account(const char* username, const char* password);
    void disconnect(const char* disconnect_message=NULL);

    void reconnect();

    bool get(const Opts opts, const void* data, std::function<void(Server*, Packet* )> func);

    static void recv_handler(Server* server);

public:
    std::map<uint8_t, std::function<void(Server*, Packet* packet)>> reponeses;

};