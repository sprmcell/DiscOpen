#pragma once

#include <iostream>
#include <string.h>

#define DATA_SIZE 1024
#define USERNAME_SIZE 32
#define MAX_PASSWD_SIZE 69
#define MIN_PASSWD_SIZE 8

enum Opts
{
    SHUTDOWN,
    ERROR,
    OK,

    CLIENT_MESSAGE,
    CLIENT_STDOUT_MESSAGE,
    CLIENT_CREATE_ACCOUNT,
    CLIENT_DELETE_ACCOUNT,
    CLIENT_COMMAND,
    CLIENT_LOGIN,

    GET_CLIENT_FRIEND_FROM_ID,
    GET_CLIENT_FRIENDS_ID,
};

struct Packet
{
    Opts opts;
    uint64_t to_who;
    uint8_t id;
    uint8_t response;
    char data[DATA_SIZE];
};

extern Packet create_packet(Opts opts, uint64_t to_who, uint8_t response, const void* data);
extern Packet create_packet(Opts opts, const void* data);

struct client_create_account_data // Opts::CLIENT_CREATE_ACCOUNT
{
    char name[USERNAME_SIZE];
    char password[MAX_PASSWD_SIZE]; // TODO: make it more secure.
};

struct user_data
{
    uint64_t id;
    char name[USERNAME_SIZE];
    bool online;
};

struct User_login_data
{
    char username[USERNAME_SIZE];
    char password[MAX_PASSWD_SIZE]; // currently unsecure...
};