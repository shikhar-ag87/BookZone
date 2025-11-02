#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>

class Database {
private:
    sqlite3* db;
    std::string dbPath;

    bool executeSQL(const std::string& sql);

public:
    Database(const std::string& path = "data/library.db");
    ~Database();

    bool open();
    void close();
    bool createTables();

    // Bookshelf operations
    bool saveBookshelf(const std::string& name, int id);
    bool deleteBookshelf(const std::string& name);
    std::vector<std::pair<int, std::string>> loadBookshelves();

    // Book operations
    bool saveBook(int bookId, const std::string& shelfName, const std::string& title,
        const std::string& author, const std::vector<std::string>& genres);
    bool deleteBook(int bookId, const std::string& shelfName);
    std::vector<std::tuple<int, std::string, std::string, std::string>> loadBooks(const std::string& shelfName);

    // User operations
    bool saveUser(const std::string& username, const std::string& password, bool isAdmin);
    bool deleteUser(const std::string& username);
    std::vector<std::tuple<std::string, std::string, bool>> loadUsers();

    // Borrowed books operations
    bool saveBorrowedBook(const std::string& username, const std::string& shelfName, int bookId);
    bool deleteBorrowedBook(const std::string& username, const std::string& shelfName, int bookId);
    std::vector<std::pair<std::string, int>> loadBorrowedBooks(const std::string& username);

    // Borrow count operations
    bool saveBorrowCount(int bookId, int count);
    int loadBorrowCount(int bookId);
    std::vector<std::pair<int, int>> loadAllBorrowCounts();
};

#endif
