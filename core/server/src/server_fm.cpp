#include "serevr_fm.h"
#include <chrono>


ServerFileManager* ServerFileManager::self = nullptr;

ServerFileManager* ServerFileManager::create()
{
    if (self == nullptr)
        self = new ServerFileManager();
    
    return self;
}

void ServerFileManager::mkdir_dirs() const
{
    std::filesystem::create_directories(server_dir);
    std::filesystem::create_directories(users_dir);
    std::filesystem::create_directories(dms_dir);
}

ServerFileManager::ServerFileManager()
    : homedir{getenv("HOME")}, 
    server_dir{homedir + "/." + DISCOPEN_DATA_DIR_NAME + "/server"}, 
    users_dir{server_dir + "/users"}, 
    dms_dir{server_dir + "/channels/dms"}
{
    mkdir_dirs();
    // std::cout << homedir << std::endl;
    // std::cout << server_dir << std::endl;
    // std::cout << users_dir << std::endl;
    // std::cout << dms_dir << std::endl;
}

ServerFileManager::~ServerFileManager()
{

}

void ServerFileManager::server_create_new_user(const uint64_t id, const char* username, const char* password)
{
    const std::string new_user_dir = users_dir + "/" + std::to_string(id);
    std::filesystem::create_directory(new_user_dir);

    std::ofstream info_file(new_user_dir + "/" + USER_INFO_FILENAME, std::ios::binary);
    fuser_info fuser_info_data = {
        .id = id,
        .timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(),
    };
    strncpy(fuser_info_data.username, username, sizeof(fuser_info_data));
    info_file.write(reinterpret_cast<char*>(&fuser_info_data), sizeof(fuser_info));
    info_file.close();

    // TODO: encrypt password
    std::ofstream shadow_file(new_user_dir + "/" + USER_SECRET_FILENAME, std::ios::binary);
    fuser_shadow password_file = {0};
    strncpy(password_file.secret, password, MAX_PASSWD_SIZE);
    shadow_file.write(reinterpret_cast<char*>(&password_file), sizeof(fuser_shadow));
    shadow_file.close();

    std::ofstream groups_file(new_user_dir + "/" + USER_GROUPS_FILENAME, std::ios::binary);
    groups_file << 0;
    groups_file.close();
    
    std::ofstream friendslist_file(new_user_dir + "/" + USER_FRIENDSLIST_FILENAME, std::ios::binary);
    friendslist_file << 0;
    friendslist_file.close();

    std::ofstream status_file(new_user_dir + "/" + USER_STATUS_FILENAME, std::ios::binary);
    status_file << 0;
    status_file.close();

    std::ofstream dms_file(new_user_dir + "/" + USER_DMS_FILENAME, std::ios::binary);
    dms_file << 0;
    dms_file.close();
}

bool ServerFileManager::client_exists(const uint64_t id)
{
    for (const auto& dir : std::filesystem::directory_iterator(users_dir))
    {
        std::fstream info_file(dir.path().string() + "/" + USER_INFO_FILENAME);
        fuser_info user_info;
        info_file.read(reinterpret_cast<char*>(&user_info), sizeof(fuser_info));
        info_file.close();

        if (user_info.id == id)
            return true;
    }

    return false;
}

bool ServerFileManager::client_exists(const char* username, uint64_t& id)
{
    for (const auto& dir : std::filesystem::directory_iterator(users_dir))
    {
        std::fstream info_file(dir.path().string() + "/" + USER_INFO_FILENAME);
        fuser_info user_info;
        info_file.read(reinterpret_cast<char*>(&user_info), sizeof(fuser_info));
        info_file.close();

        if (!strncmp(user_info.username, username, USERNAME_SIZE))
        {
            id = user_info.id;
            return true;
        }
    }

    return false;
}


bool ServerFileManager::compare_password(const uint64_t id, const char* password)
{
    for (const auto& dir : std::filesystem::directory_iterator(users_dir))
    {
        std::fstream info_file(dir.path().string() + "/" + USER_INFO_FILENAME);
        fuser_info user_info;
        info_file.read(reinterpret_cast<char*>(&user_info), sizeof(fuser_info));
        info_file.close();

        if (user_info.id == id)
        {
            std::fstream shadow_info(dir.path().string() + "/" + USER_SECRET_FILENAME);
            fuser_shadow read_password;
            shadow_info.read(reinterpret_cast<char*>(&read_password), sizeof(fuser_shadow));
            shadow_info.close();

            if (!strncmp(read_password.secret, password, MAX_PASSWD_SIZE))
                return true;
            else
                return false;
        }
    }

    return false;
}