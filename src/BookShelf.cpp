#include <iostream>
#include <string>
#include <vector>
#include "Library.h"
#include "BookShelf.h"

using namespace std;

Bookshelf::Bookshelf(int shelfId, const string& shelfName)
    : id(shelfId), name(shelfName), head(nullptr), nextBookId(0) {
}

Bookshelf::~Bookshelf() {
    Book* temp;
    while (head) {
        temp = head;
        head = head->next;
        delete temp;
    }
}

void Bookshelf::addBook(const string& title, const string& author, int id, bool silent) {
    int assignedId = (id == -1) ? nextBookId++ : id;
    Book* newBook = new Book(assignedId, title, author);
    if (!head) {
        head = newBook;
    }
    else {
        Book* temp = head;
        while (temp->next)
            temp = temp->next;
        temp->next = newBook;
    }
    if (assignedId >= nextBookId) nextBookId = assignedId + 1;
    if (!silent)
        cout << "\033[1;32m✔ Book added to '\033[1;36m" << name << "\033[1;32m' with id = " << assignedId << ".\033[0m\n";
}

void Bookshelf::addBookWithGenres(const string& title, const string& author, const vector<string>& genres, int id, bool silent) {
    int assignedId = (id == -1) ? nextBookId++ : id;
    Book* newBook = new Book(assignedId, title, author);

    for (const auto& genre : genres) {
        newBook->addGenre(genre);
    }

    if (!head) {
        head = newBook;
    }
    else {
        Book* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newBook;
    }

    bookGraph.addBook(newBook);

    if (assignedId >= nextBookId) nextBookId = assignedId + 1;
    if (!silent)
        cout << "\033[1;32m✔ Book added to '\033[1;36m" << name << "\033[1;32m' with id = " << assignedId << ".\033[0m\n";
}

vector<Book*> Bookshelf::getRecommendations(int bookId, int limit) {
    Book* book = getBookById(bookId);
    if (!book) return vector<Book*>();
    return bookGraph.getRecommendations(book, limit);
}

void Bookshelf::displayBooks() const {
    cout << "\n\033[1;36m📚 Books in '\033[1;34m" << name << "\033[1;36m' shelf:\033[0m\n";
    if (!head) {
        cout << "\033[1;33m⚠️  No books in this shelf.\033[0m\n";
        return;
    }
    Book* temp = head;
    while (temp) {
        cout << "\033[1;35m• ID: \033[1;37m" << temp->getId()
            << "\033[0m, \033[1;34mTitle: \033[0m" << temp->getTitle()
            << "\033[0m, \033[1;36mAuthor: \033[0m" << temp->getAuthor();
        // Show genres
        const auto& genres = temp->getGenres();
        if (!genres.empty()) {
            cout << "\033[0m, \033[1;33mGenres: \033[0m";
            for (size_t i = 0; i < genres.size(); ++i) {
                cout << genres[i];
                if (i < genres.size() - 1) cout << ", ";
            }
        }
        // Optionally show borrow count if gLibrary is available
        extern Library* gLibrary;
        if (gLibrary) {
            int count = gLibrary->getMostBorrowedTree().getCount(temp->getId());
            cout << "\033[0m, \033[1;31mTimes Borrowed: \033[1;37m" << count;
        }
        cout << "\033[0m" << endl;
        temp = temp->next;
    }
}

void Bookshelf::searchBook(int id) const {
    Book* temp = head;
    while (temp) {
        if (temp->getId() == id) {
            cout << "\n\033[1;36m🔍 Found in '\033[1;34m" << name << "\033[1;36m' shelf:\033[0m\n";
            cout << "\033[1;35mID: \033[1;37m" << temp->getId()
                << "\033[0m, \033[1;34mTitle: \033[0m" << temp->getTitle()
                << "\033[0m, \033[1;36mAuthor: \033[0m" << temp->getAuthor();
            // Show genres
            const auto& genres = temp->getGenres();
            if (!genres.empty()) {
                cout << "\033[0m, \033[1;33mGenres: \033[0m";
                for (size_t i = 0; i < genres.size(); ++i) {
                    cout << genres[i];
                    if (i < genres.size() - 1) cout << ", ";
                }
            }
            // Optionally show borrow count if gLibrary is available
            extern Library* gLibrary;
            if (gLibrary) {
                int count = gLibrary->getMostBorrowedTree().getCount(temp->getId());
                cout << "\033[0m, \033[1;31mTimes Borrowed: \033[1;37m" << count;
            }
            cout << "\033[0m" << endl;
            return;
        }
        temp = temp->next;
    }
    cout << "\033[1;31m✖ Book with ID " << id << " not found in '\033[1;34m" << name << "\033[1;31m' shelf.\033[0m\n";
}

void Bookshelf::deleteBook(int id) {
    if (!head) {
        cout << "\033[1;33m⚠️  Shelf is empty.\033[0m\n";
        return;
    }

    if (head->getId() == id) {
        Book* toDelete = head;
        head = head->next;
        delete toDelete;
        cout << "\033[1;31m🗑️  Book deleted from '\033[1;34m" << name << "\033[1;31m' shelf.\033[0m\n";
        return;
    }

    Book* temp = head;
    while (temp->next && temp->next->getId() != id)
        temp = temp->next;

    if (temp->next) {
        Book* toDelete = temp->next;
        temp->next = temp->next->next;
        delete toDelete;
        cout << "\033[1;31m🗑️  Book deleted from '\033[1;34m" << name << "\033[1;31m' shelf.\033[0m\n";
    }
    else {
        cout << "\033[1;31m✖ Book with ID " << id << " not found.\033[0m\n";
    }
}

string Bookshelf::getName() const {
    return name;
}

Book* Bookshelf::getHead() const {
    return head;
}

int Bookshelf::getId() const {
    return id;
}

Book* Bookshelf::getBookById(int bookId) const {
    Book* temp = head;
    while (temp) {
        if (temp->getId() == bookId) {
            return temp;
        }
        temp = temp->next;
    }
    return nullptr;
}
