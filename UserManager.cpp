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
                    // Increment borrow count in AVL tree for loaded borrowed books
                    extern Library* gLibrary;
                    if (gLibrary) {
                        gLibrary->getMostBorrowedTree().increment(bookId);
                    }
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
    cout << "\n\033[1;36m=== Register User ===\033[0m\n";
    cout << "Enter username (or type 'cancel' to abort): ";
    cin >> username;
    if (username == "cancel" || username == "CANCEL" || username == "Cancel") {
        cout << "\033[1;33mRegistration cancelled.\033[0m\n";
        return;
    }

    if (users.count(username)) {
        cout << "\033[1;31m✖ Username already exists.\033[0m\n";
        return;
    }

    cout << "Enter password (or type 'cancel' to abort): ";
    cin >> password;
    if (password == "cancel" || password == "CANCEL" || password == "Cancel") {
        cout << "\033[1;33mRegistration cancelled.\033[0m\n";
        return;
    }

    cout << "Is this user an Admin? (1 = Yes, 0 = No, or type 'cancel' to abort): ";
    string roleInput;
    cin >> roleInput;
    if (roleInput == "cancel" || roleInput == "CANCEL" || roleInput == "Cancel") {
        cout << "\033[1;33mRegistration cancelled.\033[0m\n";
        return;
    }
    try {
        role = stoi(roleInput);
    }
    catch (...) {
        cout << "\033[1;31m❗ Invalid choice.\033[0m\n";
        return;
    }

    if (role != 0 && role != 1) {
        cout << "\033[1;31m❗ Invalid choice.\033[0m\n";
        return;
    }

    users[username] = new User(username, password, role);
    cout << "\033[1;32m✔ User registered successfully.\033[0m\n";
}

User* UserManager::loginUser() {
    string username, password;
    cout << "\n\033[1;36m=== Login ===\033[0m\n";
    cout << "Enter username (or type 'cancel' to abort): ";
    cin >> username;
    if (username == "cancel" || username == "CANCEL" || username == "Cancel") {
        cout << "\033[1;33mLogin cancelled.\033[0m\n";
        return nullptr;
    }
    cout << "Enter password (or type 'cancel' to abort): ";
    cin >> password;
    if (password == "cancel" || password == "CANCEL" || password == "Cancel") {
        cout << "\033[1;33mLogin cancelled.\033[0m\n";
        return nullptr;
    }

    auto it = users.find(username);
    if (it != users.end() && it->second->password == password) {
        cout << "\033[1;32m✔ Login successful. Welcome, \033[1;36m" << username << "\033[1;32m!\033[0m\n";
        return it->second;
    }
    else {
        cout << "\033[1;31m✖ Invalid username or password.\033[0m\n";
        return nullptr;
    }
}
