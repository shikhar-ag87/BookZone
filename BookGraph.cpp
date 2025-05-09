#include <unordered_map>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <queue>
#include "BookGraph.h"

using namespace std;

void BookGraph::addBook(Book* book) {
    // Add to genre lookup
    for (const auto& genre : book->getGenres()) {
        genreToBooks[genre].insert(book);
    }
    // Build edges in the graph
    buildEdges(book);
}

void BookGraph::buildEdges(Book* book) {
    // For each genre, connect this book to all other books with the same genre
    for (const auto& genre : book->getGenres()) {
        for (Book* other : genreToBooks[genre]) {
            if (other != book) {
                adjList[book].insert(other);
                adjList[other].insert(book);
            }
        }
    }
}

std::vector<Book*> BookGraph::getRecommendations(Book* book, int limit) {
    // BFS to find closest books in the graph (excluding the starting book)
    std::vector<Book*> recommendations;
    std::set<Book*> visited;
    std::queue<Book*> q;

    visited.insert(book);
    q.push(book);

    while (!q.empty() && (int)recommendations.size() < limit) {
        Book* current = q.front();
        q.pop();

        for (Book* neighbor : adjList[current]) {
            if (visited.count(neighbor) == 0) {
                recommendations.push_back(neighbor);
                visited.insert(neighbor);
                q.push(neighbor);
                if ((int)recommendations.size() >= limit)
                    break;
            }
        }
    }
    return recommendations;
}
