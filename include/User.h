#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "BookShelf.h"
#include "Library.h" // <-- Add this include

class Bookshelf; // Add this forward declaration

class User {
public:
    int id;
    std::string username;
    std::string password;
    bool isAdmin;
    std::vector<std::pair<std::string, int>> borrowedBooks;

    User(const std::string& uname, const std::string& pass, bool admin);
    void borrowBook(Bookshelf* shelf, int bookId);
    void returnBook(Bookshelf* shelf, int bookId);
    void viewBorrowedBooks() const;
};

extern Library* gLibrary;

#endif
