#include "client.h"
#include <unistd.h>

Client::Client()
    : addr_size{sizeof(sockaddr_in)}
{

}

Client::Client(Client_data data)
    : addr_size{sizeof(sockaddr_in)}, id{data.id} 
{

}

Client::~Client()
{
    close(network_socket);
}

bool Client::connect(int& server_socket)
{
    if ((network_socket = accept(server_socket, (sockaddr*)&addr, &addr_size)) == -1)
    {
        perror("accept");
        return false;
    }

    in_addr ipAddr = addr.sin_addr;
    inet_ntop(AF_INET, &ipAddr, ipv4_c, INET_ADDRSTRLEN);

    std::cout << "Client (" << ipv4_c << ") accepted." << std::endl;

    connected = true;

    return true;
}

void Client::disconnect(const char* message)
{
    std::cout << "Disconnecting client (" << ipv4_c << "): " << message << std::endl;
    this->Send(Opts::SHUTDOWN, message);
    close(network_socket);
    connected = false;
}

void Client::Send(const Opts opts, const char* data) const
{
    Packet packet = create_packet(opts, data);

    int bytes_sent = send(network_socket, &packet, sizeof(Packet), 0);

    if (bytes_sent == -1 || bytes_sent != sizeof(Packet))
    {
        std::cerr << "Bytes sent: " << bytes_sent << ". sizeof Packet = " << sizeof(Packet) << std::endl;
        perror("send");
    }
}

void Client::respond(const Opts opts, const uint8_t packet_id, const char* data) const
{
    Packet packet = create_packet(opts, 0, packet_id, data);

    int bytes_sent = send(network_socket, &packet, sizeof(Packet), 0);

    if (bytes_sent == -1 || bytes_sent != sizeof(Packet))
    {
        std::cerr << "Bytes sent: " << bytes_sent << ". sizeof Packet = " << sizeof(Packet) << std::endl;
        perror("send");
    }
}