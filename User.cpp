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
            cout << "⚠️  You already borrowed this book.\n";
            return;
        }
    }
    borrowedBooks.push_back({ shelf->getName(), bookId });
    // Increment borrow count in the library's AVL tree
    extern Library* gLibrary;
    if (gLibrary) {
        gLibrary->getMostBorrowedTree().increment(bookId);
    }
    cout << "✅ Book borrowed successfully.\n";
}

void User::returnBook(Bookshelf* shelf, int bookId) {
    auto it = borrowedBooks.begin();
    while (it != borrowedBooks.end()) {
        if (it->first == shelf->getName() && it->second == bookId) {
            borrowedBooks.erase(it);
            cout << "✅ '" << shelf->getBookById(bookId)->getTitle() << "' returned to '" << shelf->getName() << "'\n";
            return;
        }
        ++it;
    }
    cout << "⚠️  Book not found in your borrowed list.\n";
}

void User::viewBorrowedBooks() const {
    if (borrowedBooks.empty()) {
        cout << "📭 No books currently borrowed.\n";
        return;
    }
    cout << "\n📖 Your Borrowed Books:\n";
    for (const auto& b : borrowedBooks) {
        cout << "📚 " << b.first << " - Book ID: " << b.second << endl;
    }
}
