#include "message.h"

MessageBox::MessageBox(const User* user_p, const char* message)
    : user{user_p}
{
    strncpy(this->message, message, DATA_SIZE);
}

MessageBox::~MessageBox()
{

}

void MessageBox::save() const
{

}

void MessageBox::edit(const char* new_message)
{
    strncpy(message, new_message, DATA_SIZE);
}
