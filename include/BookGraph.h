#ifndef BOOKGRAPH_H
#define BOOKGRAPH_H

#include <unordered_map>
#include <vector>
#include <set>
#include "Book.h"

class BookGraph {
private:
    // Adjacency list: Book* -> set of Book* (neighbors)
    std::unordered_map<Book*, std::set<Book*>> adjList;
    // For fast lookup by genre
    std::unordered_map<std::string, std::set<Book*>> genreToBooks;

public:
    void addBook(Book* book);
    std::vector<Book*> getRecommendations(Book* book, int limit = 5);

private:
    void buildEdges(Book* book);
};

#endif
