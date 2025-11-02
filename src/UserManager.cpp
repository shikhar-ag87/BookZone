#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "UserManager.h"

using namespace std;

void UserManager::saveToFile() {
    for (const auto& pair : users) {
        User* u = pair.second;
        database->saveUser(u->username, u->password, u->isAdmin);

        // Save borrowed books
        for (const auto& book : u->borrowedBooks) {
            database->saveBorrowedBook(u->username, book.first, book.second);
        }
    }
}

void UserManager::loadFromFile() {
    auto usersData = database->loadUsers();

    for (const auto& userData : usersData) {
        string username = get<0>(userData);
        string password = get<1>(userData);
        bool isAdmin = get<2>(userData);

        User* u = new User(username, password, isAdmin);

        // Load borrowed books
        auto borrowedBooks = database->loadBorrowedBooks(username);
        for (const auto& book : borrowedBooks) {
            u->borrowedBooks.emplace_back(book.first, book.second);
        }

        users[username] = u;
    }
}

UserManager::UserManager() : database(new Database()) {
    loadFromFile();
}

UserManager::~UserManager() {
    saveToFile();
    for (auto& pair : users)
        delete pair.second;
    delete database;
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
    database->saveUser(username, password, role);
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
