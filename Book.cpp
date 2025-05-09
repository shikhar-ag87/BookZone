#include <iostream>
#include <string>
#include <vector>
#include "Book.h"

using namespace std;

Book::Book(int bookId, const string& title, const string& author)
    : id(bookId), title(title), author(author), next(nullptr) {
}

void Book::addGenre(const string& genre) {
    genres.push_back(genre);
}

const vector<string>& Book::getGenres() const {
    return genres;
}

string Book::getTitle() const {
    return title;
}

string Book::getAuthor() const {
    return author;
}

int Book::getId() const {
    return id;
}
