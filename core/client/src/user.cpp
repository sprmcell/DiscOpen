#include "user.h"


User::User(Server* server_p, const uint64_t user_id, const char* name, bool online_p)
    : server{server_p}, id{user_id}, online{online_p}, valid_id{true}
{
    strncpy(username, name, sizeof(username));
}


User::~User()
{
}

void User::send_message(const char* message) const
{
    server->Send(Opts::CLIENT_MESSAGE, message, this->id);
}

void User::update(const char* name, bool online)
{
    strncpy(username, name, sizeof(username));
    this->online = online;
}

void User::change(Server* server_p, const uint64_t user_id, const char* name, bool online_p)
{
    server = server_p;
    id = user_id;
    strncpy(username, name, USERNAME_SIZE);
    online = online_p;
    valid_id = true;
}