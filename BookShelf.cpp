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

void Bookshelf::addBook(const string& title, const string& author, int id) {
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
    cout << "✅ Book added to '" << name << "' with id = " << assignedId << ".\n";
}

void Bookshelf::addBookWithGenres(const string& title, const string& author, const vector<string>& genres, int id) {
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
    cout << "✅ Book added to '" << name << "' with id = " << assignedId << ".\n";
}

vector<Book*> Bookshelf::getRecommendations(int bookId, int limit) {
    Book* book = getBookById(bookId);
    if (!book) return vector<Book*>();
    return bookGraph.getRecommendations(book, limit);
}

void Bookshelf::displayBooks() const {
    cout << "\n📚 Books in '" << name << "' shelf:\n";
    if (!head) {
        cout << "⚠️  No books in this shelf.\n";
        return;
    }
    Book* temp = head;
    while (temp) {
        cout << "🔸 ID: " << temp->getId()
            << ", Title: " << temp->getTitle()
            << ", Author: " << temp->getAuthor();
        // Show genres
        const auto& genres = temp->getGenres();
        if (!genres.empty()) {
            cout << ", Genres: ";
            for (size_t i = 0; i < genres.size(); ++i) {
                cout << genres[i];
                if (i < genres.size() - 1) cout << ", ";
            }
        }
        // Optionally show borrow count if gLibrary is available
        extern Library* gLibrary;
        if (gLibrary) {
            int count = gLibrary->getMostBorrowedTree().getCount(temp->getId());
            cout << ", Times Borrowed: " << count;
        }
        cout << endl;
        temp = temp->next;
    }
}

void Bookshelf::searchBook(int id) const {
    Book* temp = head;
    while (temp) {
        if (temp->getId() == id) {
            cout << "\n🔍 Found in '" << name << "' shelf:\n";
            cout << "ID: " << temp->getId()
                << ", Title: " << temp->getTitle()
                << ", Author: " << temp->getAuthor();
            // Show genres
            const auto& genres = temp->getGenres();
            if (!genres.empty()) {
                cout << ", Genres: ";
                for (size_t i = 0; i < genres.size(); ++i) {
                    cout << genres[i];
                    if (i < genres.size() - 1) cout << ", ";
                }
            }
            // Optionally show borrow count if gLibrary is available
            extern Library* gLibrary;
            if (gLibrary) {
                int count = gLibrary->getMostBorrowedTree().getCount(temp->getId());
                cout << ", Times Borrowed: " << count;
            }
            cout << endl;
            return;
        }
        temp = temp->next;
    }
    cout << "❌ Book with ID " << id << " not found in '" << name << "' shelf.\n";
}

void Bookshelf::deleteBook(int id) {
    if (!head) {
        cout << "⚠️  Shelf is empty.\n";
        return;
    }

    if (head->getId() == id) {
        Book* toDelete = head;
        head = head->next;
        delete toDelete;
        cout << "🗑️  Book deleted from '" << name << "' shelf.\n";
        return;
    }

    Book* temp = head;
    while (temp->next && temp->next->getId() != id)
        temp = temp->next;

    if (temp->next) {
        Book* toDelete = temp->next;
        temp->next = temp->next->next;
        delete toDelete;
        cout << "🗑️  Book deleted from '" << name << "' shelf.\n";
    }
    else {
        cout << "❌ Book with ID " << id << " not found.\n";
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
