#pragma once

#include <cstdlib>
#include <string.h>
#include "client_server.h"

class User
{
private:
    u_int64_t id;
    Server* server;
    bool valid_id;
    char username[USERNAME_SIZE];
    bool online;

public:
    User() : server{nullptr}, id{0}, valid_id{false}, username{NULL}, online{false} { }
    User(Server* server, const uint64_t id, const char* name, bool online);
    ~User();
    User(const User&) = delete;

    void change(Server* server, const uint64_t id, const char* name, bool online);

    void send_message(const char* message) const;
    void update(const char* name, bool online);

    uint64_t get_id() const { return id; }
    const char* get_username() const { return username; }
    bool is_online() const { return online; }
    bool is_valid() const { return valid_id; }
};