#ifndef LIBRARY_H
#define LIBRARY_H

#include <unordered_map>
#include <string>
#include "BookShelf.h"
#include "AVLTree.h"

class Library {
private:
    int nextBookShelfId;
    std::unordered_map<std::string, Bookshelf*> bookshelves;
    const std::string filename = "library.txt";
    AVLTree mostBorrowedTree;

public:
    Library();
    ~Library();
    void saveToFile();
    void loadFromFile();
    void addBookshelf(const std::string& name, bool silent = false);
    void removeBookshelf(const std::string& name);
    Bookshelf* getBookshelf(const std::string& name);
    void displayBookshelves() const;
    AVLTree& getMostBorrowedTree();
    void displayTopBorrowedBooks(int N);
    const std::unordered_map<std::string, Bookshelf*>& getBookshelves() const { return bookshelves; }
};

#endif
