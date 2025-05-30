# 📚 Library Management System

A C++ console application for managing a digital library, supporting both regular users and administrators. The system allows for bookshelf and book management, borrowing/returning books, and provides book recommendations and statistics.

---

## 🚀 Features

-   **User Registration & Login**  
    Register as a regular user or admin. User data and borrowing history are persistent.

-   **Bookshelf Management (Admin)**

    -   Add, delete, and view bookshelves (case-insensitive names).
    -   Bookshelves are persistent across sessions.

-   **Book Management (Admin)**

    -   Add, delete, and search books by ID.
    -   Each book has an ID, title, author, and genres.
    -   Book data is persistent.

-   **Borrowing & Returning Books**

    -   Users can borrow and return books from any shelf.
    -   The system tracks borrowed books per user.

-   **Book Recommendations**

    -   Get recommendations based on genre similarity using a book graph.

-   **Most Borrowed Books**

    -   View the top N most borrowed books (tracked with an AVL tree).

-   **Colorful Console UI**
    -   User-friendly, menu-driven interface with colored output.

---

## 🛠️ Build & Run

### Prerequisites

-   C++17 compatible compiler (e.g., g++, clang++, MSVC)
-   CMake (recommended for cross-platform builds)

### Build (with CMake)

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

### Run

```sh
./library_app
```

### Data Files

-   User and library data are stored in the `data/` directory as plain text files.
-   Example files: `data/users.txt`, `data/library.txt`

---

## 👨‍💻 Technologies & Concepts

-   **C++17 STL:** `unordered_map`, `vector`, `string`, etc.
-   **Custom Data Structures:**
    -   **Linked List:** For managing books within each bookshelf.
    -   **AVL Tree:** For efficiently tracking and ranking the most borrowed books.
    -   **Graph:** For book recommendations based on genre similarity.
-   **File I/O:** All data (users, bookshelves, books, borrowing history) is persistent via text files.
-   **Modular OOP Design:** Separate classes for users, books, bookshelves, library, and supporting structures.

---

## 🙏 Credits

-   Developed as a data structures project.
-   Designed for educational purposes.

---

## 📄 License

This project is for educational use. Feel free to modify and use for learning!
