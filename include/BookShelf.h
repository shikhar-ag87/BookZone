#ifndef BOOKSHELF_H
#define BOOKSHELF_H

#include <string>
#include <vector>
#include "Book.h"
#include "BookGraph.h" // Must include for member variable

class Book; // Forward declaration is fine for pointer/reference members

class Bookshelf {
private:
    int id;
    int nextBookId;
    Book* head;
    std::string name;
    BookGraph bookGraph;

public:
    Bookshelf(int shelfId, const std::string& shelfName);
    ~Bookshelf();
    void addBook(const std::string& title, const std::string& author, int id = -1, bool silent = false);
    void addBookWithGenres(const std::string& title, const std::string& author, const std::vector<std::string>& genres, int id = -1, bool silent = false);
    std::vector<Book*> getRecommendations(int bookId, int limit = 5);
    void displayBooks() const;
    void searchBook(int id) const;
    void deleteBook(int id);
    std::string getName() const;
    Book* getHead() const;
    int getId() const;
    Book* getBookById(int bookId) const;
};

#endif
