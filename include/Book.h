#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book {
private:
    int id;
    std::string title;
    std::string author;
    std::vector<std::string> genres;
public:
    Book* next;
    Book(int bookId, const std::string& title, const std::string& author);
    void addGenre(const std::string& genre);
    const std::vector<std::string>& getGenres() const;
    std::string getTitle() const;
    std::string getAuthor() const;
    int getId() const;
};

#endif
