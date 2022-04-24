#pragma once

#include "packet.h"
#include "user.h"

class MessageBox
{
private:
    const User* user;
    char message[DATA_SIZE];

public:
    MessageBox(const User* user, const char* message);
    ~MessageBox();
    MessageBox(const MessageBox&) = delete;

    void save() const;

    void edit(const char* new_message);

    const char* get_string() const { return message; }
    const User* get_user() const { return user; }
};