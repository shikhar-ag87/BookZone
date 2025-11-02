#include "Database.h"
#include <iostream>
#include <sstream>

using namespace std;

Database::Database(const string& path) : db(nullptr), dbPath(path) {
    open();
    createTables();
}

Database::~Database() {
    close();
}

bool Database::open() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        cerr << "Cannot open database: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}

void Database::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::executeSQL(const string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::createTables() {
    string sql = R"(
        CREATE TABLE IF NOT EXISTS bookshelves (
            id INTEGER PRIMARY KEY,
            name TEXT UNIQUE NOT NULL
        );
        
        CREATE TABLE IF NOT EXISTS books (
            id INTEGER,
            shelf_name TEXT,
            title TEXT NOT NULL,
            author TEXT NOT NULL,
            genres TEXT,
            PRIMARY KEY (id, shelf_name),
            FOREIGN KEY (shelf_name) REFERENCES bookshelves(name) ON DELETE CASCADE
        );
        
        CREATE TABLE IF NOT EXISTS users (
            username TEXT PRIMARY KEY,
            password TEXT NOT NULL,
            is_admin INTEGER NOT NULL
        );
        
        CREATE TABLE IF NOT EXISTS borrowed_books (
            username TEXT,
            shelf_name TEXT,
            book_id INTEGER,
            PRIMARY KEY (username, shelf_name, book_id),
            FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
        );
        
        CREATE TABLE IF NOT EXISTS borrow_counts (
            book_id INTEGER PRIMARY KEY,
            count INTEGER NOT NULL DEFAULT 0
        );
    )";
    return executeSQL(sql);
}

bool Database::saveBookshelf(const string& name, int id) {
    sqlite3_stmt* stmt;
    string sql = "INSERT OR REPLACE INTO bookshelves (id, name) VALUES (?, ?)";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, name.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteBookshelf(const string& name) {
    string sql = "DELETE FROM bookshelves WHERE name = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<pair<int, string>> Database::loadBookshelves() {
    vector<pair<int, string>> result;
    string sql = "SELECT id, name FROM bookshelves";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        result.push_back({ id, name });
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::saveBook(int bookId, const string& shelfName, const string& title,
    const string& author, const vector<string>& genres) {
    stringstream genreStream;
    for (size_t i = 0; i < genres.size(); ++i) {
        genreStream << genres[i];
        if (i < genres.size() - 1) genreStream << "|";
    }

    string sql = "INSERT OR REPLACE INTO books (id, shelf_name, title, author, genres) VALUES (?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, bookId);
    sqlite3_bind_text(stmt, 2, shelfName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, author.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, genreStream.str().c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteBook(int bookId, const string& shelfName) {
    string sql = "DELETE FROM books WHERE id = ? AND shelf_name = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, bookId);
    sqlite3_bind_text(stmt, 2, shelfName.c_str(), -1, SQLITE_TRANSIENT);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<tuple<int, string, string, string>> Database::loadBooks(const string& shelfName) {
    vector<tuple<int, string, string, string>> result;
    string sql = "SELECT id, title, author, genres FROM books WHERE shelf_name = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, shelfName.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string title = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        string author = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        string genres = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        result.push_back({ id, title, author, genres });
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::saveUser(const string& username, const string& password, bool isAdmin) {
    string sql = "INSERT OR REPLACE INTO users (username, password, is_admin) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, isAdmin ? 1 : 0);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteUser(const string& username) {
    string sql = "DELETE FROM users WHERE username = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<tuple<string, string, bool>> Database::loadUsers() {
    vector<tuple<string, string, bool>> result;
    string sql = "SELECT username, password, is_admin FROM users";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        bool isAdmin = sqlite3_column_int(stmt, 2) == 1;
        result.push_back({ username, password, isAdmin });
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::saveBorrowedBook(const string& username, const string& shelfName, int bookId) {
    string sql = "INSERT OR IGNORE INTO borrowed_books (username, shelf_name, book_id) VALUES (?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, shelfName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, bookId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

bool Database::deleteBorrowedBook(const string& username, const string& shelfName, int bookId) {
    string sql = "DELETE FROM borrowed_books WHERE username = ? AND shelf_name = ? AND book_id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, shelfName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, bookId);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

vector<pair<string, int>> Database::loadBorrowedBooks(const string& username) {
    vector<pair<string, int>> result;
    string sql = "SELECT shelf_name, book_id FROM borrowed_books WHERE username = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        string shelfName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        int bookId = sqlite3_column_int(stmt, 1);
        result.push_back({ shelfName, bookId });
    }

    sqlite3_finalize(stmt);
    return result;
}

bool Database::saveBorrowCount(int bookId, int count) {
    string sql = "INSERT OR REPLACE INTO borrow_counts (book_id, count) VALUES (?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_int(stmt, 1, bookId);
    sqlite3_bind_int(stmt, 2, count);

    bool success = sqlite3_step(stmt) == SQLITE_DONE;
    sqlite3_finalize(stmt);
    return success;
}

int Database::loadBorrowCount(int bookId) {
    string sql = "SELECT count FROM borrow_counts WHERE book_id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 0;
    }

    sqlite3_bind_int(stmt, 1, bookId);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

vector<pair<int, int>> Database::loadAllBorrowCounts() {
    vector<pair<int, int>> result;
    string sql = "SELECT book_id, count FROM borrow_counts";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return result;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int bookId = sqlite3_column_int(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);
        result.push_back({ bookId, count });
    }

    sqlite3_finalize(stmt);
    return result;
}
