#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "Library.h"
#include "AVLTree.h"
#include "Database.h"

using namespace std;

Library::Library() : nextBookShelfId(0), database(new Database()) {
    loadFromFile();
}

Library::~Library() {
    saveToFile();
    for (auto& pair : bookshelves)
        delete pair.second;
    delete database;
}

void Library::saveToFile() {
    if (bookshelves.empty()) return;

    for (const auto& pair : bookshelves) {
        Bookshelf* shelf = pair.second;
        database->saveBookshelf(shelf->getName(), shelf->getId());

        Book* current = shelf->getHead();
        while (current != nullptr) {
            database->saveBook(current->getId(), shelf->getName(),
                current->getTitle(), current->getAuthor(),
                current->getGenres());
            current = current->next;
        }
    }

    // Save borrow counts
    auto counts = mostBorrowedTree.getAll();
    for (const auto& pair : counts) {
        database->saveBorrowCount(pair.first, pair.second);
    }
}

// Helper function to trim whitespace from both ends of a string
static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

void Library::loadFromFile() {
    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
        };

    // Load bookshelves
    auto shelves = database->loadBookshelves();
    for (const auto& shelf : shelves) {
        string name = shelf.second;
        string key = toLower(name);
        Bookshelf* bookShelf = new Bookshelf(shelf.first, name);
        bookshelves[key] = bookShelf;
        if (shelf.first >= nextBookShelfId) {
            nextBookShelfId = shelf.first + 1;
        }

        // Load books for this shelf
        auto books = database->loadBooks(name);
        for (const auto& bookData : books) {
            int id = get<0>(bookData);
            string title = get<1>(bookData);
            string author = get<2>(bookData);
            string genresStr = get<3>(bookData);

            // Parse genres
            vector<string> genres;
            stringstream genreSS(genresStr);
            string genre;
            while (getline(genreSS, genre, '|')) {
                genre = trim(genre);
                if (!genre.empty()) {
                    genres.push_back(genre);
                }
            }

            bookShelf->addBookWithGenres(title, author, genres, id, true);
        }
    }

    // Load borrow counts
    auto counts = database->loadAllBorrowCounts();
    for (const auto& pair : counts) {
        for (int i = 0; i < pair.second; ++i) {
            mostBorrowedTree.increment(pair.first);
        }
    }
}

void Library::addBookshelf(const string& name, bool silent) {
    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
        };
    std::string key = toLower(name);
    if (bookshelves.count(key)) {
        if (!silent)
            cout << "\033[1;33m⚠️  Bookshelf already exists.\033[0m\n";
        return;
    }
    Bookshelf* bookShelf = new Bookshelf(nextBookShelfId++, name);
    bookshelves[key] = bookShelf;
    if (!silent)
        cout << "\033[1;32m✔ Bookshelf '\033[1;36m" << name << "\033[1;32m' added with id = " << bookShelf->getId() << ".\033[0m\n";
}

void Library::removeBookshelf(const string& name) {
    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
        };
    std::string key = toLower(name);
    auto it = bookshelves.find(key);
    if (it == bookshelves.end()) {
        cout << "\033[1;31m✖ Bookshelf not found.\033[0m\n";
        return;
    }
    delete it->second;
    bookshelves.erase(key);
    cout << "\033[1;31m✔ Bookshelf '\033[1;36m" << name << "\033[1;31m' Deleted.\033[0m\n";
}

Bookshelf* Library::getBookshelf(const string& name) {
    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
        };
    std::string key = toLower(name);
    auto it = bookshelves.find(key);
    if (it != bookshelves.end()) {
        return it->second;
    }
    return nullptr;
}

void Library::displayBookshelves() const {
    if (bookshelves.empty()) {
        cout << "\033[1;33m⚠️  No bookshelves available.\033[0m\n";
        return;
    }
    cout << "\n\033[1;36m📚 Available Bookshelves:\033[0m\n";
    for (const auto& pair : bookshelves) {
        cout << "\033[1;34m🔹 " << pair.second->getName() << "\033[0m" << endl;
    }
}

AVLTree& Library::getMostBorrowedTree() {
    return mostBorrowedTree;
}

void Library::displayTopBorrowedBooks(int N) {
    auto top = mostBorrowedTree.getTopN(N);
    if (top.empty()) {
        std::cout << "\033[1;33m⚠️  No books have been borrowed yet.\033[0m\n";
        return;
    }
    std::cout << "\n\033[1;35m🔥 Top " << N << " Most Borrowed Books:\033[0m\n";
    for (const auto& pair : top) {
        // Find the book in any shelf
        for (const auto& shelfPair : bookshelves) {
            Book* b = shelfPair.second->getBookById(pair.first);
            if (b) {
                std::cout << "\033[1;34m• ID: \033[1;37m" << b->getId()
                    << "\033[0m, \033[1;34mTitle: \033[0m" << b->getTitle()
                    << "\033[0m, \033[1;36mAuthor: \033[0m" << b->getAuthor()
                    << "\033[0m, \033[1;31mTimes Borrowed: \033[1;37m" << pair.second << "\033[0m\n";
                break;
            }
        }
    }
}
