#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "UserManager.h"

using namespace std;

void UserManager::saveToFile() {
    ofstream outFile(filename);
    for (const auto& pair : users) {
        User* u = pair.second;
        outFile << u->username << "," << u->password << "," << u->isAdmin << "|";
        for (const auto& book : u->borrowedBooks) {
            outFile << book.first << ":" << book.second << ";";
        }
        outFile << "\n";
    }
}

void UserManager::loadFromFile() {
    ifstream inFile(filename);
    string line;

    while (getline(inFile, line)) {
        size_t sep = line.find('|');
        string userInfo = line.substr(0, sep);
        string borrowData = (sep != string::npos) ? line.substr(sep + 1) : "";

        stringstream userSS(userInfo);
        string username, password, isAdminStr;
        getline(userSS, username, ',');
        getline(userSS, password, ',');
        getline(userSS, isAdminStr, ',');

        bool isAdmin = (isAdminStr == "1");
        User* u = new User(username, password, isAdmin);

        if (!borrowData.empty()) {
            stringstream borrowSS(borrowData);
            string entry;
            while (getline(borrowSS, entry, ';')) {
                size_t colon = entry.find(':');
                if (colon != string::npos) {
                    string shelf = entry.substr(0, colon);
                    int bookId = stoi(entry.substr(colon + 1));
                    u->borrowedBooks.emplace_back(shelf, bookId);
                }
            }
        }

        users[username] = u;
    }

    inFile.close();
}

UserManager::UserManager() {
    loadFromFile();
}

UserManager::~UserManager() {
    saveToFile();
    for (auto& pair : users)
        delete pair.second;
}

void UserManager::registerUser() {
    string username, password;
    int role;
    cout << "\n=== Register User ===\n";
    cout << "Enter username (or type 'cancel' to abort): ";
    cin >> username;
    if (username == "cancel" || username == "CANCEL" || username == "Cancel") {
        cout << "Registration cancelled.\n";
        return;
    }

    if (users.count(username)) {
        cout << "❌ Username already exists.\n";
        return;
    }

    cout << "Enter password (or type 'cancel' to abort): ";
    cin >> password;
    if (password == "cancel" || password == "CANCEL" || password == "Cancel") {
        cout << "Registration cancelled.\n";
        return;
    }

    cout << "Is this user an Admin? (1 = Yes, 0 = No, or type 'cancel' to abort): ";
    string roleInput;
    cin >> roleInput;
    if (roleInput == "cancel" || roleInput == "CANCEL" || roleInput == "Cancel") {
        cout << "Registration cancelled.\n";
        return;
    }
    try {
        role = stoi(roleInput);
    }
    catch (...) {
        cout << "❗ Invalid choice.\n";
        return;
    }

    if (role != 0 && role != 1) {
        cout << "❗ Invalid choice.\n";
        return;
    }

    users[username] = new User(username, password, role);
    cout << "✅ User registered successfully.\n";
}

User* UserManager::loginUser() {
    string username, password;
    cout << "\n=== Login ===\n";
    cout << "Enter username (or type 'cancel' to abort): ";
    cin >> username;
    if (username == "cancel" || username == "CANCEL" || username == "Cancel") {
        cout << "Login cancelled.\n";
        return nullptr;
    }
    cout << "Enter password (or type 'cancel' to abort): ";
    cin >> password;
    if (password == "cancel" || password == "CANCEL" || password == "Cancel") {
        cout << "Login cancelled.\n";
        return nullptr;
    }

    auto it = users.find(username);
    if (it != users.end() && it->second->password == password) {
        cout << "✅ Login successful. Welcome, " << username << "!\n";
        return it->second;
    }
    else {
        cout << "❌ Invalid username or password.\n";
        return nullptr;
    }
}
