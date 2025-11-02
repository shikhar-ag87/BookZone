# 📊 Database Documentation - BookZone Library Management System

## Overview

BookZone uses **SQLite3**, a self-contained, serverless, zero-configuration relational database engine. This document provides comprehensive information about the database schema, queries, operations, and design decisions.

**Database File**: `data/library.db`  
**Database Engine**: SQLite3 (version 3.x)  
**Connection**: Single persistent connection with proper lifecycle management  
**Language**: C++ with SQLite3 C API  

---

## 📐 Database Schema

### Entity-Relationship Overview

```
┌─────────────┐         ┌──────────┐         ┌────────────────┐
│ bookshelves │────────<│  books   │>────────│ borrowed_books │
└─────────────┘         └──────────┘         └────────────────┘
                              │                       │
                              │                       │
                              └───────────┬───────────┘
                                          │
                                    ┌─────▼─────┐
                                    │   users   │
                                    └───────────┘
                                          │
                                    ┌─────▼──────────┐
                                    │ borrow_counts  │
                                    └────────────────┘
```

---

## 🗃️ Table Definitions

### 1. `bookshelves` Table

Stores library bookshelves (categories/sections).

```sql
CREATE TABLE IF NOT EXISTS bookshelves (
    id INTEGER PRIMARY KEY,
    name TEXT UNIQUE NOT NULL
);
```

| Column | Type    | Constraints           | Description                |
|--------|---------|-----------------------|----------------------------|
| id     | INTEGER | PRIMARY KEY           | Auto-incrementing shelf ID |
| name   | TEXT    | UNIQUE, NOT NULL      | Bookshelf name (unique)    |

**Indexes**: 
- PRIMARY KEY index on `id`
- UNIQUE index on `name`

**Sample Data**:
```sql
INSERT INTO bookshelves (name) VALUES ('Fiction'), ('Science'), ('Programming');
```

---

### 2. `books` Table

Stores individual books with relationships to bookshelves.

```sql
CREATE TABLE IF NOT EXISTS books (
    id INTEGER,
    shelf_name TEXT,
    title TEXT NOT NULL,
    author TEXT NOT NULL,
    genres TEXT,
    PRIMARY KEY (id, shelf_name),
    FOREIGN KEY (shelf_name) REFERENCES bookshelves(name) ON DELETE CASCADE
);
```

| Column     | Type    | Constraints                  | Description                           |
|------------|---------|------------------------------|---------------------------------------|
| id         | INTEGER | PRIMARY KEY (composite)      | Book ID (unique within shelf)         |
| shelf_name | TEXT    | PRIMARY KEY, FOREIGN KEY     | Reference to bookshelves.name         |
| title      | TEXT    | NOT NULL                     | Book title                            |
| author     | TEXT    | NOT NULL                     | Book author name                      |
| genres     | TEXT    | NULL allowed                 | Pipe-delimited genres (e.g., "Fiction\|Drama") |

**Composite Primary Key**: (id, shelf_name) - allows same book ID on different shelves

**Foreign Key Constraint**: 
- `shelf_name` references `bookshelves(name)`
- `ON DELETE CASCADE` - deleting a bookshelf removes all its books automatically

**Sample Data**:
```sql
INSERT INTO books (id, shelf_name, title, author, genres) 
VALUES 
    (0, 'Fiction', 'The Hobbit', 'J.R.R. Tolkien', 'Fantasy|Adventure'),
    (1, 'Fiction', '1984', 'George Orwell', 'Dystopian|Political Fiction');
```

---

### 3. `users` Table

Stores user account information for authentication and authorization.

```sql
CREATE TABLE IF NOT EXISTS users (
    username TEXT PRIMARY KEY,
    password TEXT NOT NULL,
    is_admin INTEGER NOT NULL
);
```

| Column   | Type    | Constraints  | Description                         |
|----------|---------|--------------|-------------------------------------|
| username | TEXT    | PRIMARY KEY  | Unique username                     |
| password | TEXT    | NOT NULL     | User password (plain text)*         |
| is_admin | INTEGER | NOT NULL     | Role flag: 1=Admin, 0=Regular User  |

**Security Note**: *In production, passwords should be hashed using bcrypt or similar. This is a learning project.

**Sample Data**:
```sql
INSERT INTO users (username, password, is_admin) 
VALUES 
    ('admin', 'adminpass', 1),
    ('alice', 'alicepass', 0);
```

---

### 4. `borrowed_books` Table

Junction table tracking which books are borrowed by which users.

```sql
CREATE TABLE IF NOT EXISTS borrowed_books (
    username TEXT,
    shelf_name TEXT,
    book_id INTEGER,
    PRIMARY KEY (username, shelf_name, book_id),
    FOREIGN KEY (username) REFERENCES users(username) ON DELETE CASCADE
);
```

| Column     | Type    | Constraints              | Description                    |
|------------|---------|--------------------------|--------------------------------|
| username   | TEXT    | PRIMARY KEY, FOREIGN KEY | User who borrowed the book     |
| shelf_name | TEXT    | PRIMARY KEY              | Bookshelf name                 |
| book_id    | INTEGER | PRIMARY KEY              | Book ID on that shelf          |

**Composite Primary Key**: (username, shelf_name, book_id) - prevents duplicate borrows

**Foreign Key Constraint**:
- `username` references `users(username)`
- `ON DELETE CASCADE` - deleting a user removes all their borrowing records

**Sample Data**:
```sql
INSERT INTO borrowed_books (username, shelf_name, book_id) 
VALUES 
    ('alice', 'Fiction', 0),
    ('alice', 'Programming', 1);
```

---

### 5. `borrow_counts` Table

Tracks how many times each book has been borrowed (for popularity statistics).

```sql
CREATE TABLE IF NOT EXISTS borrow_counts (
    book_id INTEGER PRIMARY KEY,
    count INTEGER NOT NULL DEFAULT 0
);
```

| Column  | Type    | Constraints          | Description                        |
|---------|---------|----------------------|------------------------------------|
| book_id | INTEGER | PRIMARY KEY          | Book ID (global across shelves)    |
| count   | INTEGER | NOT NULL, DEFAULT 0  | Number of times book was borrowed  |

**Note**: This table tracks book IDs globally, not per-shelf. The count increments each time any copy is borrowed.

**Sample Data**:
```sql
INSERT INTO borrow_counts (book_id, count) VALUES (0, 5), (1, 12), (2, 3);
```

---

## 🔄 CRUD Operations

### CREATE Operations

#### Add Bookshelf
```cpp
bool Database::saveBookshelf(const std::string& name, int id) {
    std::string sql = "INSERT OR IGNORE INTO bookshelves (id, name) VALUES (?, ?);";
    // Uses prepared statement with parameter binding
}
```

**SQL**:
```sql
INSERT OR IGNORE INTO bookshelves (id, name) VALUES (?, ?);
```

#### Add Book
```cpp
bool Database::saveBook(int bookId, const std::string& shelfName, 
                        const std::string& title, const std::string& author, 
                        const std::vector<std::string>& genres)
```

**SQL**:
```sql
INSERT OR IGNORE INTO books (id, shelf_name, title, author, genres) 
VALUES (?, ?, ?, ?, ?);
```

#### Add User
```cpp
bool Database::saveUser(const std::string& username, 
                        const std::string& password, bool isAdmin)
```

**SQL**:
```sql
INSERT OR IGNORE INTO users (username, password, is_admin) 
VALUES (?, ?, ?);
```

#### Record Borrowed Book
```cpp
bool Database::saveBorrowedBook(const std::string& username, 
                                const std::string& shelfName, int bookId)
```

**SQL**:
```sql
INSERT OR IGNORE INTO borrowed_books (username, shelf_name, book_id) 
VALUES (?, ?, ?);

INSERT INTO borrow_counts (book_id, count) VALUES (?, 1) 
ON CONFLICT(book_id) DO UPDATE SET count = count + 1;
```

---

### READ Operations

#### Load All Bookshelves
```cpp
std::vector<std::pair<int, std::string>> Database::loadBookshelves()
```

**SQL**:
```sql
SELECT id, name FROM bookshelves;
```

#### Load Books from a Shelf
```cpp
std::vector<std::tuple<int, std::string, std::string, std::string>> 
Database::loadBooks(const std::string& shelfName)
```

**SQL**:
```sql
SELECT id, title, author, genres FROM books WHERE shelf_name = ?;
```

#### Load All Users
```cpp
std::vector<std::tuple<std::string, std::string, bool>> Database::loadUsers()
```

**SQL**:
```sql
SELECT username, password, is_admin FROM users;
```

#### Load User's Borrowed Books
```cpp
std::vector<std::pair<std::string, int>> 
Database::loadBorrowedBooks(const std::string& username)
```

**SQL**:
```sql
SELECT shelf_name, book_id FROM borrowed_books WHERE username = ?;
```

#### Load Borrow Counts
```cpp
std::vector<std::pair<int, int>> Database::loadAllBorrowCounts()
```

**SQL**:
```sql
SELECT book_id, count FROM borrow_counts ORDER BY count DESC;
```

---

### UPDATE Operations

#### Update Borrow Count
```cpp
bool Database::saveBorrowCount(int bookId, int count)
```

**SQL**:
```sql
INSERT INTO borrow_counts (book_id, count) VALUES (?, ?) 
ON CONFLICT(book_id) DO UPDATE SET count = ?;
```

**UPSERT Pattern**: INSERT if doesn't exist, UPDATE if it does

---

### DELETE Operations

#### Delete Bookshelf
```cpp
bool Database::deleteBookshelf(const std::string& name)
```

**SQL**:
```sql
DELETE FROM bookshelves WHERE name = ?;
```

**Cascade Effect**: All books on this shelf are automatically deleted due to `ON DELETE CASCADE`

#### Delete Book
```cpp
bool Database::deleteBook(int bookId, const std::string& shelfName)
```

**SQL**:
```sql
DELETE FROM books WHERE id = ? AND shelf_name = ?;
```

#### Delete User
```cpp
bool Database::deleteUser(const std::string& username)
```

**SQL**:
```sql
DELETE FROM users WHERE username = ?;
```

**Cascade Effect**: All borrowing records for this user are automatically deleted

#### Return Book (Delete Borrowed Record)
```cpp
bool Database::deleteBorrowedBook(const std::string& username, 
                                  const std::string& shelfName, int bookId)
```

**SQL**:
```sql
DELETE FROM borrowed_books 
WHERE username = ? AND shelf_name = ? AND book_id = ?;
```

---

## 🛡️ SQL Injection Prevention

All database operations use **prepared statements** with parameter binding:

```cpp
sqlite3_stmt* stmt;
sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
sqlite3_bind_int(stmt, 2, bookId);
sqlite3_step(stmt);
sqlite3_finalize(stmt);
```

**Benefits**:
- ✅ Prevents SQL injection attacks
- ✅ Better performance (compiled once, executed many times)
- ✅ Automatic type conversion and escaping

**Bad Practice** (vulnerable):
```cpp
// NEVER DO THIS!
std::string sql = "SELECT * FROM users WHERE username = '" + username + "';";
```

**Good Practice** (safe):
```cpp
// ALWAYS USE PREPARED STATEMENTS
std::string sql = "SELECT * FROM users WHERE username = ?;";
sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
```

---

## 🔐 Data Integrity Constraints

### Primary Key Constraints

| Table           | Primary Key              | Purpose                          |
|-----------------|--------------------------|----------------------------------|
| bookshelves     | id                       | Unique shelf identifier          |
| books           | (id, shelf_name)         | Same book ID allowed per shelf   |
| users           | username                 | Unique user identifier           |
| borrowed_books  | (username, shelf, book)  | Prevent duplicate borrows        |
| borrow_counts   | book_id                  | One count record per book        |

### Foreign Key Constraints

| Child Table     | Foreign Key  | References         | On Delete |
|-----------------|--------------|--------------------|-----------| 
| books           | shelf_name   | bookshelves(name)  | CASCADE   |
| borrowed_books  | username     | users(username)    | CASCADE   |

**CASCADE Behavior**:
- Deleting a bookshelf → All books on that shelf are deleted
- Deleting a user → All their borrowing records are deleted

### Unique Constraints

- `bookshelves.name` - No duplicate bookshelf names
- `users.username` - No duplicate usernames

### NOT NULL Constraints

- All critical fields (title, author, password, etc.) must have values
- Prevents incomplete records

---

## ⚡ Transaction Support

SQLite provides **ACID** properties:

- **Atomicity**: All operations in a transaction succeed or fail together
- **Consistency**: Database remains in a valid state
- **Isolation**: Transactions don't interfere with each other
- **Durability**: Committed data is permanently saved

**Example Transaction** (borrowing a book):
```cpp
// Begin transaction
executeSQL("BEGIN TRANSACTION;");

// Step 1: Record the borrow
saveBorrowedBook(username, shelfName, bookId);

// Step 2: Increment borrow count
int currentCount = loadBorrowCount(bookId);
saveBorrowCount(bookId, currentCount + 1);

// Commit transaction
executeSQL("COMMIT;");

// If error occurs, rollback:
// executeSQL("ROLLBACK;");
```

---

## 📊 Complex Queries

### Query 1: Get All Books with Shelf Info
```sql
SELECT b.id, b.title, b.author, b.genres, bs.name as shelf_name
FROM books b
JOIN bookshelves bs ON b.shelf_name = bs.name
ORDER BY bs.name, b.id;
```

### Query 2: Get Top N Most Borrowed Books
```sql
SELECT bc.book_id, bc.count, b.title, b.author
FROM borrow_counts bc
JOIN books b ON bc.book_id = b.id
ORDER BY bc.count DESC
LIMIT ?;
```

### Query 3: Get User's Borrowed Books with Details
```sql
SELECT bb.book_id, bb.shelf_name, b.title, b.author
FROM borrowed_books bb
JOIN books b ON bb.book_id = b.id AND bb.shelf_name = b.shelf_name
WHERE bb.username = ?;
```

### Query 4: Count Books per Bookshelf
```sql
SELECT bs.name, COUNT(b.id) as book_count
FROM bookshelves bs
LEFT JOIN books b ON bs.name = b.shelf_name
GROUP BY bs.name;
```

### Query 5: Find Users Who Borrowed Specific Book
```sql
SELECT DISTINCT u.username, u.is_admin
FROM borrowed_books bb
JOIN users u ON bb.username = u.username
WHERE bb.book_id = ?;
```

---

## 🔧 Database Utilities

### Integrity Check
```bash
sqlite3 data/library.db 'PRAGMA integrity_check;'
```

Expected output: `ok`

### View Schema
```bash
sqlite3 data/library.db '.schema'
```

### Backup Database
```bash
cp data/library.db data/library_backup_$(date +%Y%m%d).db
```

### Restore Database
```bash
cp data/library_backup_20251102.db data/library.db
```

### Export to SQL
```bash
sqlite3 data/library.db .dump > library_backup.sql
```

### Import from SQL
```bash
sqlite3 data/library.db < library_backup.sql
```

### Database Statistics
```bash
sqlite3 data/library.db "
SELECT 'Bookshelves' as Table, COUNT(*) as Count FROM bookshelves
UNION ALL
SELECT 'Books', COUNT(*) FROM books
UNION ALL
SELECT 'Users', COUNT(*) FROM users
UNION ALL
SELECT 'Borrowed', COUNT(*) FROM borrowed_books
UNION ALL
SELECT 'Borrow Counts', COUNT(*) FROM borrow_counts;
"
```

---

## 📈 Performance Considerations

### Indexes
- **Automatic Indexes**: SQLite automatically creates indexes for:
  - PRIMARY KEY columns
  - UNIQUE constraint columns
- **Query Optimization**: WHERE clauses on indexed columns are fast

### Prepared Statements
- Compiled once, executed multiple times
- Significant performance gain for repeated queries
- Stored in `Database` class for reuse

### Connection Pooling
- Single persistent connection throughout application lifetime
- No overhead of opening/closing connection repeatedly

### Data Types
- SQLite uses **dynamic typing** with type affinity
- INTEGER, TEXT, REAL, BLOB types
- Flexible but type-safe through C++ interface

---

## 🎓 Learning Outcomes

### DBMS Concepts Demonstrated

✅ **Database Design**
- Entity-Relationship modeling
- Schema normalization (3NF)
- Primary and foreign key relationships

✅ **SQL Operations**
- DDL: CREATE TABLE, DROP TABLE
- DML: INSERT, SELECT, UPDATE, DELETE
- Constraints: PRIMARY KEY, FOREIGN KEY, UNIQUE, NOT NULL
- CASCADE operations

✅ **Query Techniques**
- Simple SELECT with WHERE
- JOIN operations (INNER JOIN, LEFT JOIN)
- Aggregation (COUNT, GROUP BY)
- Sorting (ORDER BY, LIMIT)

✅ **Data Integrity**
- Entity integrity (primary keys)
- Referential integrity (foreign keys)
- Domain integrity (data types)

✅ **Security**
- Prepared statements
- SQL injection prevention
- Parameter binding

✅ **Transactions**
- ACID properties
- BEGIN/COMMIT/ROLLBACK
- Atomic operations

---

## 🚀 Future Enhancements

### Database Improvements
- [ ] Add indexes on frequently queried columns
- [ ] Implement full-text search (FTS5) for book titles/authors
- [ ] Add timestamps (created_at, updated_at) to tables
- [ ] Store borrow/return dates for history tracking
- [ ] Implement database versioning/migration system
- [ ] Add views for complex queries
- [ ] Password hashing (bcrypt/Argon2)
- [ ] Database connection pooling for multi-threading

### Advanced Features
- [ ] Stored procedures (if migrating to PostgreSQL/MySQL)
- [ ] Triggers for automatic borrow count updates
- [ ] Multi-user concurrent access
- [ ] Database replication for backup
- [ ] Query caching layer
- [ ] RESTful API for remote access

---

## 📚 References

- **SQLite Documentation**: https://www.sqlite.org/docs.html
- **SQLite3 C/C++ Interface**: https://www.sqlite.org/c3ref/intro.html
- **SQL Tutorial**: https://www.w3schools.com/sql/
- **Database Normalization**: https://en.wikipedia.org/wiki/Database_normalization
- **ACID Properties**: https://en.wikipedia.org/wiki/ACID

---

**Last Updated**: November 2, 2025  
**Database Version**: SQLite 3.x  
**Project**: BookZone Library Management System
