#include <string>
#include <vector>
#include <iostream>
#include "User.h"
#include "BookShelf.h" // Add this include
#include "Library.h"

using namespace std;

User::User(const string& uname, const string& pass, bool admin)
    : username(uname), password(pass), isAdmin(admin) {
}

void User::borrowBook(Bookshelf* shelf, int bookId) {
    for (const auto& b : borrowedBooks) {
        if (b.first == shelf->getName() && b.second == bookId) {
            cout << "\033[1;33m⚠️  You already borrowed this book.\033[0m\n";
            return;
        }
    }
    borrowedBooks.push_back({ shelf->getName(), bookId });
    // Increment borrow count in the library's AVL tree
    extern Library* gLibrary;
    if (gLibrary) {
        gLibrary->getMostBorrowedTree().increment(bookId);
    }
    cout << "\033[1;32m✔ Book borrowed successfully.\033[0m\n";
}

void User::returnBook(Bookshelf* shelf, int bookId) {
    auto it = borrowedBooks.begin();
    while (it != borrowedBooks.end()) {
        if (it->first == shelf->getName() && it->second == bookId) {
            borrowedBooks.erase(it);
            cout << "\033[1;32m✔ '\033[1;34m" << shelf->getBookById(bookId)->getTitle() << "\033[1;32m' returned to '\033[1;36m" << shelf->getName() << "\033[1;32m'\033[0m\n";
            return;
        }
        ++it;
    }
    cout << "\033[1;33m⚠️  Book not found in your borrowed list.\033[0m\n";
}

void User::viewBorrowedBooks() const {
    if (borrowedBooks.empty()) {
        cout << "\033[1;33m📭 No books currently borrowed.\033[0m\n";
        return;
    }
    cout << "\n\033[1;36m📖 Your Borrowed Books:\033[0m\n";
    for (const auto& b : borrowedBooks) {
        cout << "\033[1;34m📚 " << b.first << " - Book ID: " << b.second << "\033[0m" << endl;
    }
}
