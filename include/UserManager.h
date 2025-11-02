#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <unordered_map>
#include "User.h"
#include "Database.h"

class UserManager {
private:
    std::unordered_map<std::string, User*> users;
    Database* database;

public:
    void saveToFile();
    void loadFromFile();
    UserManager();
    ~UserManager();
    void registerUser();
    User* loginUser();
    Database* getDatabase() { return database; }
};

#endif
