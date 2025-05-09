#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <iostream>
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
        outFile << "#Bookshelf," << pair.first << "\n";
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

void Library::loadFromFile() {
    ifstream inFile(filename);
    string line;
    Bookshelf* currentShelf = nullptr;

    while (getline(inFile, line)) {
        if (line.rfind("#Bookshelf,", 0) == 0) {
            string shelfName = line.substr(11);  // after "#Bookshelf,"
            currentShelf = new Bookshelf(nextBookShelfId++, shelfName);
            bookshelves[shelfName] = currentShelf;
        }
        else if (currentShelf) {
            stringstream ss(line);
            string idStr, title, author, genresStr;
            getline(ss, idStr, ',');
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, genresStr);

            // Split genres by |
            vector<string> genres;
            stringstream genreSS(genresStr);
            string genre;
            while (getline(genreSS, genre, '|')) {
                if (!genre.empty()) {
                    genres.push_back(genre);
                }
            }

            int id = stoi(idStr);
            currentShelf->addBookWithGenres(title, author, genres, id);
        }
    }

    inFile.close();
}

void Library::addBookshelf(const string& name) {
    if (bookshelves.count(name)) {
        cout << "⚠️  Bookshelf already exists.\n";
        return;
    }
    Bookshelf* bookShelf = new Bookshelf(nextBookShelfId++, name);
    bookshelves[name] = bookShelf;
    cout << "✅ Bookshelf '" << name << "' added with id = " << bookShelf->getId() << ".\n";
}

void Library::removeBookshelf(const string& name) {
    auto it = bookshelves.find(name);
    if (it == bookshelves.end()) {
        cout << "❌ Bookshelf not found.\n";
        return;
    }
    delete it->second;
    bookshelves.erase(name);
    cout << "✅ Bookshelf '" << name << "' Deleted.\n";
}

Bookshelf* Library::getBookshelf(const string& name) {
    auto it = bookshelves.find(name);
    return (it != bookshelves.end()) ? it->second : nullptr;
}

void Library::displayBookshelves() const {
    if (bookshelves.empty()) {
        cout << "⚠️  No bookshelves available.\n";
        return;
    }
    cout << "\n📚 Available Bookshelves:\n";
    for (const auto& pair : bookshelves) {
        cout << "🔹 " << pair.first << endl;
    }
}

AVLTree& Library::getMostBorrowedTree() {
    return mostBorrowedTree;
}

void Library::displayTopBorrowedBooks(int N) {
    auto top = mostBorrowedTree.getTopN(N);
    if (top.empty()) {
        std::cout << "⚠️  No books have been borrowed yet.\n";
        return;
    }
    std::cout << "\n🔥 Top " << N << " Most Borrowed Books:\n";
    for (const auto& pair : top) {
        // Find the book in any shelf
        for (const auto& shelfPair : bookshelves) {
            Book* b = shelfPair.second->getBookById(pair.first);
            if (b) {
                std::cout << "🔸 ID: " << b->getId()
                    << ", Title: " << b->getTitle()
                    << ", Author: " << b->getAuthor()
                    << ", Times Borrowed: " << pair.second << "\n";
                break;
            }
        }
    }
}
