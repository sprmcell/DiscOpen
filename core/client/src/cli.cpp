#include "cli.h"
#include <unistd.h>
#include "colours.h"
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <sstream>

#define MAX_LOGIN_TRIES 3


Cli::Cli(Server* server_p)
    : server{server_p},
    commands{
        {"help", 
        [&](const std::vector<std::string>& argv) {
            if (argv.size() > 1 && argv.at(1) == "help")
                std::cout << "To see this message.\n";
            else 
            {
                std::cout << "Commands: \n";
                for (const auto& cmd : this->commands)
                {
                    const std::string& cmd_str = cmd.first;
                    std::cout << "  " << green(cmd_str) << "   ";
                    cmd.second({cmd_str, "help"});
                }
            }
        }},
        {"send", 
        [&](const std::vector<std::string>& argv) {
            if (argv.size() > 1 && argv.at(1) == "help")
                std::cout << "Send message to other client.\n";
            else
            {
                std::string friend_id_str;
                uint64_t friend_id = 0;

                std::cout << "To who? ";
                std::getline(std::cin, friend_id_str);

                std::istringstream iss(friend_id_str);
                iss >> friend_id;

                std::string message;
                std::cout << "Message: ";
                std::getline(std::cin, message);

                this->server->Send(Opts::CLIENT_MESSAGE, message.c_str(), friend_id);
            }
        }},
        {"exit", 
        [&](const std::vector<std::string>& argv) {
            if (argv.size() > 1 && argv.at(1) == "help")
                std::cout << "Disconnect from server & exit this program.\n";
            else
            {
                this->server->Send(Opts::SHUTDOWN, "Client type: 'exit'");
                sleep(1);
            }
        }},
        {"create", 
        [&](const std::vector<std::string>& argv) {
            if (argv.size() > 1 && argv.at(1) == "help")
                std::cout << "Create a new account. 'create account'\n";
            else if (argv.size() == 1)
                std::cout << "Create what? account?" << std::endl;
            else
            {
                if (argv.at(1) == "account")
                {
                    std::string username, password;
                    std::cout << "Creating new account...\nUsername: ";
                    std::getline(std::cin, username);
                    std::cout << "Password for '" << username << "': ";
                    std::getline(std::cin, password); 
                    // TODO: encrypt password

                    std::cout << "Sending data to server..." << std::endl;
                    this->server->create_account(username.c_str(), password.c_str());
                }
                else
                {
                    std::cout << "Only can create account..." << std::endl;
                }
            }
        }},
        {"login", 
        [&](const std::vector<std::string>& argv) {
            if (argv.size() > 1 && argv.at(1) == "help")
                std::cout << "Login to account\n";
            else 
            {
                std::condition_variable cv;
                std::mutex m;
                bool failed = true;
                bool server_call_back = false; 

                for (size_t i = 0; i < MAX_LOGIN_TRIES; ++i)
                {
                    std::string username;
                    std::string password;

                    std::cout << "Username: ";
                    std::getline(std::cin, username);
                    std::cout << "Password for '" << username << "': ";
                    std::getline(std::cin, password);

                    User_login_data login_info;
                    strncpy(login_info.username, username.c_str(), USERNAME_SIZE);
                    strncpy(login_info.password, password.c_str(), MAX_PASSWD_SIZE);

                    server->get(Opts::CLIENT_LOGIN, &login_info, [&](Server* server, Packet* packet) {
                        std::cout << "Yo" << std::endl;
                        std::lock_guard<std::mutex> lg(m);

                        switch (packet->opts)
                        {
                        case Opts::OK:
                        {
                            user_data my_info;
                            memcpy(&my_info, packet->data, sizeof(user_data));
                            this->me.change(this->server, my_info.id, my_info.name, my_info.online);
                            failed = false;
                            break;
                        }
                        case Opts::ERROR:
                        {
                            failed = true;
                            break;
                        }
                        default:
                        {
                            failed = true;
                            std::cerr << "??" << std::endl;
                            break;
                        }
                        }
                        server_call_back = true;
                        cv.notify_one();
                    });

                    std::cout << "Waiting..." << std::endl;
                    std::unique_lock<std::mutex> ul(m);
                    cv.wait(ul, [&server_call_back] { return server_call_back; });
                    std::cout << "Done waiting." << std::endl;

                    if (!failed)
                        break;
                }
            }
        }},
    }
{
    // init_friends(); 
    // print_friends();
}

Cli::~Cli()
{
    for (User* user : friends)
        delete user;
}

void Cli::init_friends()
{
    // Kinda hard to explain what server::get do...
    // So you send a request to the server, and add a callback function. So what every the client
    // requested callback function can do something spesific for that request
    server->get(Opts::GET_CLIENT_FRIENDS_ID, nullptr, [&](Server* server, Packet* packet) {
        const size_t max_size = DATA_SIZE / sizeof(uint64_t);
        uint64_t* friends_id = new uint64_t[max_size];
        memcpy(friends_id, packet->data, max_size);

        for (size_t i = 0; i < max_size; ++i)
        {
            if (friends_id[i] != 0)
            {
                uint64_t friendID = friends_id[i];
                server->get(Opts::GET_CLIENT_FRIEND_FROM_ID, reinterpret_cast<char*>(&friends_id[i]), [&](Server* server_in, Packet* packet_in) {
                    user_data user;
                    memcpy(&user, packet_in->data, sizeof(user_data));
                    User* new_user = new User(server, friendID, user.name, user.online);
                    this->friends.push_back(new_user);
                    std::cout << new_user->get_username() << " (" << ((new_user->is_online()) ? "online" : "offline") << ")" << std::endl;
                });
            }
        }
        delete friends_id;
    });
}

void Cli::print_friends() const
{
    const size_t friends_count = this->friends.size();

    std::cout << "Friends list: " << friends_count << std::endl;

    // for (const User* user : this->friends)
      //  std::cout << user->get_username() << " (" << ((user->is_online()) ? "online" : "offline") << ")" << std::endl;
}

void Cli::update()
{

}

void Cli::run()
{
    while (server->is_connected())
    {
        std::string input;
        std::cout << me.get_username() << " > ";
        std::getline(std::cin, input);

        bool success = process_command(input);
        fflush(stdin);
    }
}


bool Cli::process_command(const std::string& command)
{
    std::vector<std::string> args = split(command, ' ');

    if (args.size() == 0)
        return false;

    if (commands.count(args.at(0)))
    {
        commands.at(args.at(0))(args);
        return true;
    }
    else
    {
        std::cout << "Wrong command: '" << args.at(0) << "'\n";
        return false;
    }
}

std::string Cli::get_cmd(const std::string& command)
{
    size_t idx = 0;
    for (const char& c : command)
        if (c != ' ')
            ++idx;
        else
            break;

    char cmd[idx];
    strncpy(cmd, command.c_str(), idx);
    cmd[idx] = 0;

    return cmd;
}


std::vector<std::string> Cli::split(const std::string& str, const char sep)
{
    std::vector<std::string> strs;

    size_t tmp_idx = 0;
    char tmp[str.size()];
    memset(tmp, 0, str.size());

    if (str.find(sep) == EOF)
    {
        strs.push_back(str);
    }
    else
    {
        for (size_t i = 0; i < str.length(); i++)
        {
            if (str.at(i) == sep && str.at(i) != '\n')
            {
                if (i != 0 && tmp[0] != 0)
                {
                    strs.push_back(tmp);
                    memset(tmp, 0, str.length());
                    tmp_idx = 0;
                }
            }
            else
            {
                tmp[tmp_idx] = str.at(i);
                tmp_idx++;
            }
        }
        if (tmp[0] != sep && tmp[0] != 0)
            strs.push_back(tmp);
    }

    return strs;
}