#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <unordered_map>
#include "User.h"

class UserManager {
private:
    std::unordered_map<std::string, User*> users;
    const std::string filename = "users.txt";

public:
    void saveToFile();
    void loadFromFile();
    UserManager();
    ~UserManager();
    void registerUser();
    User* loginUser();
};

#endif
