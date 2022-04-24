#include "client_server.h"
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "colours.h"

void packet_handler(Server* server, Packet* packet)
{
    if (packet->response != 0 && server->reponeses.count(packet->response))
    {
        server->reponeses.at(packet->response)(server, packet);
        server->reponeses.erase(packet->response);
    }
    else
    {
        switch (packet->opts)
        {
        case Opts::SHUTDOWN:
            server->disconnect(packet->data);
            break;
        case Opts::CLIENT_STDOUT_MESSAGE:
            std::cout << "Server: " << bold(packet->data) << std::endl;
            break;
        case Opts::ERROR:
        {
            std::string errmsg = "Server: ";
            std::cerr << red(errmsg + packet->data) << std::endl;
        }
        default:
            std::cout << red("Unread packet from server") << " -> " << bold(std::to_string(packet->opts)) << std::endl;
            break;
        }
    }
}

Server::Server()
    : server_port{DEFAULT_PORT}, ipv4{DEFAULT_IPV4}, connected{false}
{
    Init();
}

Server::Server(int port, const char* ipv4_c)
    : server_port{port}, connected{false}
{
    strncpy(ipv4, ipv4_c, INET_ADDRSTRLEN);
    Init();
}

Server::~Server()
{
    if (connected)
        disconnect("Client shutting down.");
}


bool Server::Connect()
{
    if (connect(network_socket, (sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        return false;
    }

    std::cout << "Connected to: " << ipv4 << ":" << server_port << std::endl;

    return true;
}

void Server::Init()
{
    srand(time(NULL));
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_port);
    addr.sin_addr.s_addr = INADDR_ANY;

    connected = Connect();
}

bool Server::Send(const Opts opts, const char* data, const uint64_t to_who) const
{
    Packet packet = create_packet(opts, to_who, 0, data);

    int bytes_sent = send(network_socket, &packet, sizeof(Packet), 0);

    if (bytes_sent == -1 || bytes_sent != sizeof(Packet))
    {
        std::cerr << "ERROR: Bytes sent: " << bytes_sent << ". sizeof Packet = " << sizeof(Packet) << std::endl;
        perror("send");
        return false;
    }

    return true;
} 

bool Server::create_account(const char* username, const char* password)
{
    client_create_account_data account_data;
    strncpy(account_data.name, username, sizeof(client_create_account_data::name));
    strncpy(account_data.password, password, sizeof(client_create_account_data::password));

    Packet packet = create_packet(Opts::CLIENT_CREATE_ACCOUNT, &account_data);

    if ((send(network_socket, &packet, sizeof(Packet), 0)) == -1)
    {
        perror("send");
        return false;
    }
    return true;
}

void Server::disconnect(const char* disconnect_message)
{
    this->Send(Opts::SHUTDOWN, disconnect_message);
    close(network_socket);
    connected = false;
}

void Server::reconnect()
{
    if (connected)
        disconnect("Reconnecting...");
    Init();
    connected = Connect();
}

bool Server::get(const Opts opts, const void* data, std::function<void(Server*, Packet*)> callback)
{
    Packet packet = create_packet(opts, data);

    this->reponeses[packet.id] = callback;

    if ((send(network_socket, &packet, sizeof(Packet), 0)) == -1)
    {
        perror("send");
        return false;
    }
    return true;
}

void Server::recv_handler(Server* server)
{
    while (server->is_connected())
    {
        Packet* packet = new Packet();

        if ((recv(server->get_socket(), packet, sizeof(Packet), 0)) == -1)
            perror("recv");
        else
            packet_handler(server, packet);

        delete packet;
    }
}