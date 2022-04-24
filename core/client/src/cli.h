#pragma once

#include "client_server.h"
#include <vector>
#include <string>
#include "user.h"
#include <map>
#include <functional>

class Cli
{
private:
    Server* server;
    std::vector<User*> friends;
    std::map<std::string, std::function<void(const std::vector<std::string>& args)>> commands;

    User me;

private:
    void init_friends();
    void print_friends() const;
    bool process_command(const std::string& command);
    std::string get_cmd(const std::string& command);
    std::vector<std::string> split(const std::string& str, const char sep);

public:
    Cli(Server* server);
    ~Cli();

    void update();
    void run();
};