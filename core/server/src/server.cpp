#include "server.h"
#include <iostream>
#include <thread>
#include "packet.h"
#include "unistd.h"
#include <map>
#include <functional>
#include "common.h"
#include <colours.h>


Server::Server()
    : port{DEFAULT_PORT}, files{ServerFileManager::create()}
{
    running = init_server();
}

Server::Server(int server_port)
    : port{server_port}, files{ServerFileManager::create()} 
{
    running = init_server();
}

Server::~Server()
{
    // shutdown();
    delete files;
}

int Server::run()
{
    if (!running)
        return EXIT_FAILURE;

    size_t client_idx = 0;

    while (running)
    {
        client_array.at(client_idx) = Client();
        Client* client = &client_array.at(client_idx);

        if (client->connect(network_socket))
        {
            client_threads.at(client_idx) = std::thread(handle_client, this, client);
            client_idx++;
        }
        else
        {
            std::cerr << "Client failed to connect." << std::endl;
            client->disconnect("Failed to connect");
        }
    }

    return EXIT_SUCCESS;
}

void Server::handle_client(Server* server, Client* client)
{
    while (server->running && client->is_connected())
    {
        Packet* client_packet = new Packet();

        if ((recv(client->get_socket(), client_packet, sizeof(Packet), 0)) == -1)
        {
            char errmsg[64];
            sprintf(errmsg, "ERROR:recv: Client (%s)", client->get_ip_addr());
            perror(errmsg);
            // client->disconnect("");
        }
        else
        {
            Server::recv_handler(server, client, client_packet);
        }
        delete client_packet;
    }
}


bool Server::init_server()
{
    if ((network_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        perror("socket");
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;

    if (setsockopt(network_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
        perror("setsockopt");

    if (bind(network_socket, (sockaddr*)&address, sizeof(address)) == -1)
    {
        perror("bind error");
        return false;
    }

    if (listen(network_socket, MAX_CLIENTS) == -1)
    {
        perror("listen error");
        return false;
    }

    std::cout << "Listening on port: " << port << std::endl;

    return true;
}

void Server::shutdown()
{
    for (Client& client : client_array)
        if (client.is_connected())
            client.disconnect("Server is shutting down...");

    for (std::thread& thread : client_threads)
        if (thread.joinable())
            thread.join();

    close(network_socket);
    running = false;
}

void Server::recv_handler(Server* server, Client* client, Packet* packet)
{
    if (client->get_id() == 0)
    {
        if (packet->opts != Opts::CLIENT_CREATE_ACCOUNT &&
            packet->opts != Opts::CLIENT_LOGIN &&
            packet->opts != Opts::SHUTDOWN )
        {
            client->Send(Opts::ERROR, "Not singed in.");
            return;
        }
    }

    switch (packet->opts)
    {
    case Opts::CLIENT_MESSAGE:
    {
        if (server->files->client_exists(packet->to_who))
        {
            for (const Client& o_client : server->client_array)
            {
                std::cout << "ID: " << o_client.get_id() << std::endl;
                if (o_client.get_id() == packet->to_who)
                {
                    if (o_client.is_connected())
                    {
                        o_client.Send(Opts::CLIENT_STDOUT_MESSAGE, packet->data);
                    }
                    else
                    {
                        char errmsg[64] = {0};
                        sprintf(errmsg, "User: %d is offline.", packet->to_who);
                        client->respond(Opts::ERROR, packet->id, errmsg);
                    }
                    break;
                }
            }
        }
        else
        {
            char errmsg[64] = {0};
            sprintf(errmsg, "User: %d doesn't exists.", packet->to_who);
            client->respond(Opts::ERROR, packet->id, errmsg);
        }
        break;
    }
    case Opts::SHUTDOWN:
    {
        client->disconnect(packet->data);
        
        break;
    }
    case Opts::CLIENT_CREATE_ACCOUNT:
    {
        client_create_account_data new_account_data;
        memcpy(&new_account_data, packet->data, sizeof(client_create_account_data));

        uint64_t new_user_id = random_64_id();
        std::cout << "Creating new user: '" << bold(new_account_data.name) << "' [" << bold(std::to_string(new_user_id)) << "]" << std::endl;

        server->files->server_create_new_user(new_user_id, new_account_data.name, new_account_data.password);

        break;
    }
    case Opts::CLIENT_DELETE_ACCOUNT:
    {
        break;
    }
    case Opts::CLIENT_COMMAND:
    {
        std::string cmd = packet->data;
        std::cout << "Command: " << cmd << std::endl;

        FILE* cmdoutput = popen(cmd.c_str(), "r");
        char buff[256];
        fread(buff, 256, 1, cmdoutput);
        client->Send(Opts::CLIENT_MESSAGE, buff);

        std::map<std::string, std::function<void(std::string&)>> commands = {
            {"ls", [&](std::string& cmd) {
            }}
        };

        if (commands.count(cmd))
            commands.at(cmd)(cmd);
        else
            std::cout << "Wrong command\n";

        break;
    }
    case Opts::GET_CLIENT_FRIEND_FROM_ID:
    {
        uint64_t id = 0; 
        memcpy(&id, packet->data, sizeof(uint64_t));

        // TODO: Get friend info
        char username[32] = {0};
        sprintf(username, "%s@%d", "Gentaiii", id); // TODO: Remove

        fuser_info friend_data = {
            .id = id,
        };
        strncpy(friend_data.username, username, sizeof(friend_data.username));

        Packet response_packet = create_packet(packet->opts, 0, packet->id, &friend_data);
        std::cout << "frends_from_id: " << (uint)response_packet.response << std::endl;
        
        if ((send(client->get_socket(), &response_packet, sizeof(Packet), 0)) == -1)
        {
            perror("send");
        }

        break;
    }
    case Opts::GET_CLIENT_FRIENDS_ID:
    {
        size_t friends_count = 5; // TODO: Count client friends
        uint64_t* friends_id = new uint64_t[friends_count]; 

        for (size_t i = 0; i < friends_count; i++)
            friends_id[i] = random_64_id(); // ID

        Packet response_packet = create_packet(packet->opts, 0, packet->id, &friends_id);

        if ((send(client->get_socket(), &response_packet, sizeof(Packet), 0)) == -1)
        {
            perror("send");
        }

        delete friends_id;

        break;
    }
    case Opts::CLIENT_LOGIN:
    {
        User_login_data login_data;
        memcpy(&login_data, packet->data, sizeof(User_login_data));

        uint64_t id = 0;
        
        // TODO: Create a new method to search users. Disk reading not good.
        if (server->files->client_exists(login_data.username, id))
        {
            if (server->files->compare_password(id, login_data.password))
            {
                client->set_id(id);

                user_data client_info = {
                    .id = id,
                    .online = true
                };
                strncpy(client_info.name, login_data.username, USERNAME_SIZE);

                client->respond(Opts::OK, packet->id, reinterpret_cast<char*>(&client_info));
            }
            else
                client->respond(Opts::ERROR, packet->id, "Incorrect username or password.");
        }
        else
            client->respond(Opts::ERROR, packet->id, "Incorrect username or password.");

        break;
    }
    default:
        std::cerr << "Client (" << client->get_ip_addr() << ") unknown Packet.opts: " << packet->opts << "?" << std::endl;
        break;
    }
}