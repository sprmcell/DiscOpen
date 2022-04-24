#pragma once

#include "packet.h"
#include "server.h"
#include "server_user.h"
#include "common.h"
#include <filesystem>
#include <fstream>

#define DISCOPEN_DATA_DIR_NAME "discopen-testing"
#define USER_INFO_FILENAME "info"
#define USER_SECRET_FILENAME "shadow"
#define USER_FRIENDSLIST_FILENAME "friends"
#define USER_STATUS_FILENAME "status"
#define USER_PFP_FILENAME "pfp.png"
#define USER_GROUPS_FILENAME "groups"
#define USER_DMS_FILENAME "dms"

#define DM_MSG_CACHE_DIR "cache"
#define DM_INFO_FILENAME "info"
#define DM_CACHE_MESSAGES "messages_" // messages_#

class ServerFileManager 
{
private:
	void mkdir_dirs() const;
    ServerFileManager();
    
    static ServerFileManager* self;

public:
    void server_create_new_user(const uint64_t id, const char* username, const char* password);

    bool client_exists(const uint64_t id);
    bool client_exists(const char* username, uint64_t& id);

    bool compare_password(const uint64_t id, const char* password);

    ~ServerFileManager();
    static ServerFileManager* create();

public:
	const std::string homedir; 
	const std::string server_dir; 
	const std::string users_dir;
	const std::string dms_dir; 

};


struct fuser_info
{
    uint64_t id;
    char username[USERNAME_SIZE];
    int64_t timestamp;
};

struct fuser_status
{
    bool online;
};

struct fuser_shadow
{
    char secret[MAX_PASSWD_SIZE];
};

struct fuser_friends
{
    size_t friends_count;
    uint64_t* friends_id;
};
