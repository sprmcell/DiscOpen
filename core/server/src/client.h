#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "packet.h"

struct Client_data
{
    uint64_t id;
    char username[USERNAME_SIZE];
    bool online;
};

class Client
{
private:
    int network_socket;
    char ipv4_c[INET_ADDRSTRLEN];
    uint64_t id = 0;

    sockaddr_in addr; 
    socklen_t addr_size;

    bool connected = false;
    bool logged_in = false;

public:
    Client();
    Client(Client_data data);
    ~Client();

    int get_socket() const { return network_socket; }

    bool connect(int& server_socket);
    void disconnect(const char* message);
    void set_id(uint64_t id) { this->id = id; };

    void Send(const Opts opts, const char* data) const;
    void respond(const Opts opts, const uint8_t packet_id, const char* data) const;

    bool is_connected() const { return connected; }

    const char* get_ip_addr() const { return ipv4_c; }
    uint64_t get_id() const { return id; }
};