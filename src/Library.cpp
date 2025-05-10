#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include "Library.h"
#include "AVLTree.h"

using namespace std;

Library::Library() : nextBookShelfId(0) {
    loadFromFile();
}

Library::~Library() {
    saveToFile();
    for (auto& pair : bookshelves)
        delete pair.second;
}

void Library::saveToFile() {
    if (bookshelves.empty()) return;
    ofstream outFile(filename);
    for (const auto& pair : bookshelves) {
        // Save the original shelf name, not the key
        outFile << "#Bookshelf," << pair.second->getName() << "\n";
        Book* current = pair.second->getHead();
        while (current != nullptr) {
            outFile << current->getId() << ","
                << current->getTitle() << ","
                << current->getAuthor() << ",";
            // Save genres with | separator
            const auto& genres = current->getGenres();
            for (size_t i = 0; i < genres.size(); ++i) {
                outFile << genres[i];
                if (i < genres.size() - 1) outFile << "|";
            }
            outFile << "\n";
            current = current->next;
        }
    }
    outFile.close();
}

// Helper function to trim whitespace from both ends of a string
static inline std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

void Library::loadFromFile() {
    ifstream inFile(filename);
    string line;
    Bookshelf* currentShelf = nullptr;

    auto toLower = [](const std::string& s) {
        std::string out = s;
        std::transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
        };

    while (getline(inFile, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.rfind("#Bookshelf,", 0) == 0) {
            string shelfName = trim(line.substr(11));  // after "#Bookshelf,"
            addBookshelf(shelfName, true);
            std::string key = toLower(shelfName);
            currentShelf = bookshelves.count(key) ? bookshelves[key] : nullptr;
        }
        else if (currentShelf) {
            stringstream ss(line);
            string idStr, title, author, genresStr;
            getline(ss, idStr, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, genresStr);

            idStr = trim(idStr);
            title = trim(title);
            author = trim(author);
            genresStr = trim(genresStr);

            // Split genres by |
            vector<string> genres;
            stringstream genreSS(genresStr);
            string genre;
            while (getline(genreSS, genre, '|')) {
                genre = trim(genre);
                if (!genre.empty()) {
                    genres.push_back(genre);
                }
            }

            if (!idStr.empty() && !title.empty() && !author.empty()) {
                int id = stoi(idStr);
                currentShelf->addBookWithGenres(title, author, genres, id, true);
            }
        }
        // else: ignore lines if currentShelf is nullptr (malformed file)
    }

    inFile.close();
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
